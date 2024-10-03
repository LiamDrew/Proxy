#include "proxy.h"

#define CACHE_SIZE 10

int main(int argc, char **argv)
{
    int portno;
    int parentfd;

    struct sockaddr_in serveraddr;
    struct sockaddr_in clientaddr;
    struct hostent *hostp; 
    char *hostaddrp;

    // check command line arguments
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }

    // get port number from program input
    portno = atoi(argv[1]);
    printf("Listening on port %d\n", portno);

    parentfd = socket(AF_INET, SOCK_STREAM, 0);
    if (parentfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    /* setsockopt: Handy debugging trick that lets
     * us rerun the server immediately after we kill it;
     * otherwise we have to wait about 20 secs.
     * Eliminates "ERROR on binding: Address already in use" error.
     */
    int optval = 1;
    setsockopt(parentfd, SOL_SOCKET, SO_REUSEADDR,
               (const void *)&optval, sizeof(int));

    /* build the server's Internet address */
    bzero((char *)&serveraddr, sizeof(serveraddr));

    /* this is an Internet address */
    serveraddr.sin_family = AF_INET;

    /* let the system figure out our IP address */
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    /* this is the port we will listen on */
    serveraddr.sin_port = htons((unsigned short)portno);

    /* bind: associate the parent socket with a port */
    if (bind(parentfd, (struct sockaddr *)&serveraddr,
             sizeof(serveraddr)) < 0)
        perror("ERROR on binding");

    /* listen: make this socket ready to accept connection requests */

    if (listen(parentfd, 0) < 0) /* allow 5 requests to queue up */
        perror("ERROR on listen");


    int childfd;
    int clientlen;
    clientlen = sizeof(clientaddr);

    // make a cache
    Cache_T *cache = create_cache(CACHE_SIZE);

    // The proxy should run indefinitely, per the spec
    while (true)
    {
        printf("\n\n\n\nWaiting for connection requests:\n");

        /* accept: wait for a connection request*/
        childfd = accept(parentfd, (struct sockaddr *)&clientaddr, &clientlen);
        if (childfd < 0) {
            error("ERROR on accept");
        }

        /* gethostbyaddr: determine who sent the message */
        hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr,
                              sizeof(clientaddr.sin_addr.s_addr), AF_INET);
        if (hostp == NULL) {
            error("ERROR on gethostbyaddr");
        }

        hostaddrp = inet_ntoa(clientaddr.sin_addr);
        if (hostaddrp == NULL) {
            error("ERROR on inet_ntoa\n");
        }

        printf("Server established connection with %s (%s)\n",
               hostp->h_name, hostaddrp);



        // NOTE: BUFSIZE is the upper limit to the size of get Request that can 
        // be parsed (can change this)
        // NOTE: What is the max size of a get request that can come in?
        
        // Read the GET request from the client.
        Buffer_T *request = read_get_request(childfd);

        Server_Address_T *server_info = parse_get_request(request);

        // printf("Server hostname is: %s\n", server_info->hostname);
        // printf("Portno is: %d\n", server_info->portno);

        char *host_port = concat_host_and_port(server_info->hostname, 
            server_info->portno);
        printf("Unique address: %s\n", host_port);

        //Entry_T *entry = find_response_in_cache(cache, host_port)
        bool found = find_response_in_cache(cache, host_port);

        if (found) {
            // 1. Response is in the cache
            bool stale = is_entry_stale(cache, host_port);

            if (stale) {
                // 1a. Response is stale

                // Response is stale, so we have to refetch it from tje server
                Buffer_T *response = get_from_server(server_info, request);
                free_Buffer_T(&request);
                free_Server_Address_T(&server_info);
                // HERE: at this point, we are done talking to the server.

                int max_age = get_max_age(response);

                // Update the response in the cache
                // Need to get the time that is now
                time_t current_time = time(NULL);
                unsigned long now = (unsigned long)current_time;

                update_response_in_cache(cache, host_port, response, now, 
                    max_age);

                // The data was just fetched, so we don't need to include
                // Max age
                respond_to_client(childfd, response);
            }

            else {
                // 1b. Response is NOT stale
                
                // NOTE: make sure we update access time here

                // Get the response and its age from the cache
                Cache_Response_T *cached_response = 
                    get_response_from_cache(cache, host_port);

                // Serving from cache, soadd the age to the response
                Buffer_T *updated_response = 
                    add_age_to_response(cached_response->age, 
                        cached_response->buf);

                respond_to_client(childfd, updated_response);
                free_Buffer_T(&updated_response);
            }
        } 
        
        else { // else not found
            // 2. Response is NOT in the cache
            Buffer_T *response = get_from_server(server_info, request);
            free_Buffer_T(&request);
            free_Server_Address_T(&server_info);
            // HERE: at this point, we are done talking to the server.

            // read and look for the Cache-Control field to find age
            // if not, get max age will default to 3600
            int max_age = get_max_age(response);

            time_t current_time = time(NULL);
            unsigned long now = (unsigned long)current_time;

            put_response_in_cache(cache, host_port, response, now, max_age);

            // NOTE: Do not add age to response, it is fresh
            respond_to_client(childfd, response);
        }

        // After writing the response back to the client, close the socket
        close(childfd);
    }

    free_cache(&cache);
}