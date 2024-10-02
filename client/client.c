
/*
 * tcpclient.c - A simple TCP client
 * usage: tcpclient <host> <port>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUFSIZE 1024

/*
 * error - wrapper for perror
 */
void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char **argv)
{
    int sockfd, portno, n;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char *hostname;
    char buf[BUFSIZE];

    /* check command line arguments */
    if (argc != 3)
    {
        fprintf(stderr, "usage: %s <hostname> <port>\n", argv[0]);
        exit(0);
    }
    hostname = argv[1];
    portno = atoi(argv[2]);

    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    /* gethostbyname: get the server's DNS entry */
    server = gethostbyname(hostname);
    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host as %s\n", hostname);
        exit(0);
    }

    /* build the server's Internet address */
    bzero((char *)&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(portno);

    /* connect: create a connection with the server */
    if (connect(sockfd, &serveraddr, sizeof(serveraddr)) < 0)
        error("ERROR connecting");

    /* get message line from the user */
    printf("Please enter msg: ");
    bzero(buf, BUFSIZE);

    //NOTE: this will have to be updated to be a GET request
    fgets(buf, BUFSIZE, stdin);

    /* send the message line to the server */
    n = write(sockfd, buf, strlen(buf));
    if (n < 0)
        error("ERROR writing to socket");

    /* print the server's reply */
    bzero(buf, BUFSIZE);
    n = read(sockfd, buf, BUFSIZE);
    if (n < 0)
        error("ERROR reading from socket");
    printf("Echo from server: %s", buf);
    close(sockfd);
    return 0;
}

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <arpa/inet.h>

// #define PORT 9050 // Server port
// #define BUFFER_SIZE 1024

// int main()
// {
//     int sockfd;
//     struct sockaddr_in server_addr;
//     char buffer[BUFFER_SIZE];

//     // Create socket
//     sockfd = socket(AF_INET, SOCK_STREAM, 0);
//     if (sockfd < 0)
//     {
//         perror("ERROR opening socket");
//         exit(EXIT_FAILURE);
//     }

//     // Set up the server address
//     memset(&server_addr, 0, sizeof(server_addr));
//     server_addr.sin_family = AF_INET;
//     server_addr.sin_addr.s_addr = inet_addr("10.4.2.21");
//     server_addr.sin_port = htons(PORT);

//     // Connect to the server
//     if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
//     {
//         perror("ERROR connecting");
//         close(sockfd);
//         exit(EXIT_FAILURE);
//     }

//     // Communicate with the server
//     while (1)
//     {
//         printf("Enter message: ");
//         fgets(buffer, BUFFER_SIZE, stdin);
//         send(sockfd, buffer, strlen(buffer), 0);
//         memset(buffer, 0, BUFFER_SIZE);
//         recv(sockfd, buffer, BUFFER_SIZE - 1, 0);
//         printf("Server: %s\n", buffer);
//     }

//     close(sockfd);
//     return 0;
// }

//My attempt
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <sys/types.h>
// #include <netinet/in.h>
// #include <arpa/inet.h>
// #include <netdb.h>
// #include <assert.h>

// #define BUFSIZE 1024

// #if 0
// //Structs exported from in.h
// /* Internet address */
// struct in_addr {
//     unsigned int s_addr;
// };

// /* Internet style socket address */
// struct sockaddr_in {
//     unsigned short int sin_family; /* Address family */
//     unsigned short int sin_port;   /* Port number */
//     struct in_addr sin_addr;       /* IP address */
//     unsigned char sin_zero[8];     /* Pad to size of 'struct sockaddr' */
// };

// // Struct exported from netdb.h
// /* Domain name service (DNS) host entry */
// struct hostent {
//     char *h_name;       /* official name of host */
//     char **h_aliases;   /* alias list */
//     int h_addrtype;     /* host address type */
//     int h_length;       /* length of address */
//     char **h_addr_list; /* list of addresses */
// };
// #endif

// int main (int argc, char **argv)
// {
//     int sockfd, port_number;
//     char *hostname = "10.4.2.21";
//     char buf[BUFSIZE];

//     // check command line args
//     if (argc != 2) {
//         fprintf(stderr, "usage: %s <port>\n", argv[0]);
//         exit(0);
//     }

//     // need to define the hostname and get port number
//     port_number = atoi(argv[1]);

//     // create the socket
//     sockfd = socket(AF_INET, SOCK_STREAM, 0);
//     if (sockfd < 0) {
//         perror("ERROR opening socket.");
//     }

//     // get the server's DNS entry
//     struct hostent *server = gethostbyname(hostname);
//     if (server == NULL) {
//         fprintf(stderr, "ERROR, no such host as %s\n", hostname);
//     }

//     // build the server's internet address.
//     struct sockaddr_in *server_address = malloc(sizeof(struct sockaddr_in));
//     assert(server_address != NULL);
//     server_address->sin_family = AF_INET;
//     server_address->sin_port = htons(port_number);
//     server_address->sin_addr.s_addr = inet_addr(hostname);

//     // try to connect to the server:
//     // Step 3: Connect to the server
//     if (connect(sockfd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
//     {
//         perror("ERROR connecting");
//         close(sockfd);
//         exit(EXIT_FAILURE);
//     }

//     free(server_address);

//     return 0;
// }