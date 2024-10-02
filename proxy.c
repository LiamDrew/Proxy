#include "proxy.h"

void error(char *msg)
{
    perror(msg);
    exit(0);
}

void free_Server_Address_T(Server_Address_T **server)
{
    free((*server)->hostname);
    (*server)->hostname = NULL;

    free(*server);
    *server = NULL;
}

Buffer_T *read_get_request(int childfd)
{
    char ch;
    int status;
    int i = 0;
    bool prior_CRLF = false;

    // NOTE: I'm going to allocate the buffer on the heap here.
    // Using calloc to initialize every slot to 0
    char *buf = calloc(BUFSIZE, sizeof(char));
    unsigned long capacity = BUFSIZE;
    unsigned long old_capacity = BUFSIZE;

    // GET requests must be less than BUFSIZE right now
    while (true)
    {
        if (i == capacity - 1) {
            // if at capacity, expand the buffer
            old_capacity = capacity;
            capacity *= 2;
            char *new_buf = calloc(capacity, sizeof(char));
            for (int j = 0; j < old_capacity; j++) {
                new_buf[j] = buf[j];
            }

            free(buf);
            buf = new_buf;
        }

        status = read(childfd, &ch, 1);

        if (status < 0)
            error("ERROR reading from socket");

        buf[i] = ch;
        i++;

        if (ch == '\r')
        {
            status = read(childfd, &ch, 1);
            buf[i] = ch;
            i++;

            if (ch == '\n')
            {
                if (prior_CRLF)
                {
                    // two carriage returns in a row means end of request
                    // printf("\nRequest completed\n");
                    break;
                }

                prior_CRLF = true;
            }
            else
            {
                printf("Bad formatting\n");
            }
        }
        else
        {
            prior_CRLF = false;
        }
    }

    // printf("Proxy received %d bytes: \n\n%s\n", i, buf);
    // printf("Proxy received %d bytes\n", i);
    Buffer_T *buffer = new_Buffer_T(buf, i);

    return buffer;
}

bool validate_get_request(const char *buffer)
{
    const char *get_string = "GET";
    char get_buf[4];
    strncpy(get_buf, buffer, 3);
    get_buf[3] = '\0';

    return strcmp(get_buf, get_string) == 0;
}

char *extract_host_field(const char *buffer)
{
    const char *host_string = "Host:";

    // Finding first occurrence of "Host:" if it exists
    char *host_start = strstr(buffer, host_string);

    if (host_start == NULL) {
        return NULL;
    }

    host_start += strlen(host_string);
    while (*host_start == ' ') {
        host_start++;
    }


    char *host_end = strstr(host_start, "\r\n");
    if (host_end == NULL) {
        return NULL;
    }

    unsigned int host_len = host_end - host_start;

    char *host_line = calloc(host_len + 1, sizeof(char));

    strncpy(host_line, host_start, host_len);
    host_line[host_len] = '\0';

    return host_line;
}

Server_Address_T *parse_host_and_port(const char *host_line)
{
    char *hostname = calloc(BUFSIZE, sizeof(char));
    int portno = 80;

    // Search for colon to determine if port is specified
    const char *colon_ptr = strchr(host_line, ':');
    if (colon_ptr != NULL) {
        unsigned int hostname_len = colon_ptr - host_line;

        strncpy(hostname, host_line, hostname_len);
        hostname[hostname_len] = '\0';

        // Convert portno to an integer
        portno = atoi(colon_ptr + 1);
    }

    // There is no port number specified, leave portno to default as 80
    else {
        unsigned int host_len = strlen(host_line);
        strncpy(hostname, host_line, host_len);
        hostname[host_len] = '\0';
    }

    Server_Address_T *server = malloc(sizeof(Server_Address_T));
    server->portno = portno;
    server->hostname = hostname;
    return server;
}

char *concat_host_and_port(char *hostname, int portno)
{
    char str[20];
    // will automatically be null terminated
    sprintf(str, "%d", portno);


    int size = strlen(hostname) + strlen(str) + 2;
    char *id = calloc(size, sizeof(char));

    int i = 0;
    for (; i < strlen(hostname); i++) {
        id[i] = hostname[i];
    }

    id[i] = ':';
    i++;

    for (int j = 0; j < strlen(str); j++) {
        id[i] = str[j];
        i++;
    }

    id[i] = '\0';

    return id;
}

Server_Address_T *parse_get_request(Buffer_T *request)
{
    // printf(" HTTP Request:\n\n%s\n", request->buf);
    // printf("____________________________________\n\n");

    // Ensure the request starts with "GET"
    if (!validate_get_request(request->buf))
    {
        error("HTTP request did not start with GET\n");
        return NULL;
    }

    // Get the contents of the Host field
    char *host_line = extract_host_field(request->buf);
    if (host_line == NULL)
    {
        error("Host header not found or badly formatted\n");
        return NULL;
    }

    // Parse the host and port from the Host field
    Server_Address_T *server_address = parse_host_and_port(host_line);
    if (server_address == NULL)
    {
        error("Failed to parse host and port from Host header\n");
        return NULL;
    }

    free(host_line);

    return server_address;
}

Buffer_T *get_from_server(Server_Address_T *server_info, Buffer_T *request)
{
    int server_sockfd;
    struct sockaddr_in serveraddr;
    struct hostent *server;

    /* socket: create the socket */
    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sockfd < 0)
        error("ERROR opening socket");

    /* gethostbyname: get the server's DNS entry */
    server = gethostbyname(server_info->hostname);
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host as %s\n", server_info->hostname);
        exit(0);
    }

    /* build the server's Internet address */
    bzero((char *)&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(server_info->portno);

    /* connect: create a connection with the server */
    if (connect(server_sockfd, (struct sockaddr *)&serveraddr, 
                sizeof(serveraddr)) < 0) {
        error("ERROR connecting");
    }

    // write the GET request to the server
    int g = write(server_sockfd, request->buf, request->size);
    if (g < 0)
        error("ERROR writing to socket");

    // read the RESPONSE back from the server
    // the filesize will not be longer than 10 MB
    unsigned int capacity = FILESIZE;
    char *buf = calloc(capacity, sizeof(char));
    assert(buf != NULL);

    unsigned int total_bytes_read = 0;
    int n;

    while (true) {
        n = read(server_sockfd, buf + total_bytes_read, BUFSIZE);

        if (n < 0) {
            printf("Read failed\n");
            assert(false);
        }

        else if (n == 0) {
            // all bytes have been read
            break;
        }

        total_bytes_read += n;
    }

    Buffer_T *buffer = new_Buffer_T(buf, total_bytes_read);

    close(server_sockfd);

    return buffer;
}

int get_max_age(Buffer_T *response)
{
    const char *cache_text = "Cache-Control: max-age=";
    char *cache_field = strstr(response->buf, cache_text);

    // if it exists, parse it
    if (cache_field != NULL) {
        char int_buf[20];
        int i = cache_field + strlen(cache_text) - response->buf;
        int j = 0;

        while (response->buf[i] >= '0' && response->buf[i] <= '9') {
            if (j > 18) {
                error("cache time way too long\n");
            }

            int_buf[j] = response->buf[i];
            i++;
            j++;
        }

        int_buf[j] = '\0';

        int max_age_int = atoi(int_buf);

        if (max_age_int > 0) {
            return max_age_int;
        }
    }

    // if it does't exist, return 3600 (seconds in an hour)
    return 3600;
}

Buffer_T *add_age_to_response(int max_age, Buffer_T *response)
{
    char age_field[60];
    // Formatting the way age goes into the buffer
    snprintf(age_field, sizeof(age_field), "Age: %d\r\n", max_age);

    unsigned int age_field_length = strlen(age_field);
    unsigned int new_size = response->size + age_field_length;

    char *new_buf = (char *)calloc(new_size + 1, sizeof(char));

    unsigned int i = 0;
    unsigned int j = 0;
    bool inserted = false;

    while (i < response->size) {
        if (!inserted 
            && response->buf[i] == '\r' 
            && response->buf[i + 1] == '\n' 
            && response->buf[i + 2] == '\r' 
            && response->buf[i + 3] == '\n') {

            new_buf[j++] = response->buf[i++];
            new_buf[j++] = response->buf[i++];

            strcpy(new_buf + j, age_field);
            j += age_field_length;

            new_buf[j++] = response->buf[i++];
            new_buf[j++] = response->buf[i++];

            inserted = true;
        }
        else {
            new_buf[j++] = response->buf[i++];
        }
    }

    if (!inserted) {
        strcpy(new_buf + j, age_field);
    }

    Buffer_T *buffer = new_Buffer_T(new_buf, new_size);

    return buffer;
}
//write to client
void respond_to_client(int childfd, Buffer_T *response)
{
    int n = write(childfd, response->buf, response->size);
    if (n < 0)
        error("ERROR writing to socket");

}