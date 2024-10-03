#include "cache.h"

#define BUFSIZE 1024

// MAX filesize is 10MBs
#define FILESIZE 10000001

typedef struct {
    char *hostname;
    int portno;
} Server_Address_T;

void error(char *msg);

void free_Server_Address_T(Server_Address_T **server);


Buffer_T *read_get_request(int childfd);


// parse get request for necessary server information
Server_Address_T *parse_get_request(Buffer_T *request);

// talk to the server
Buffer_T *get_from_server(Server_Address_T *server_info, Buffer_T *request);

int get_max_age(Buffer_T *response);

Buffer_T *add_age_to_response(int max_age, Buffer_T *response);

// write to the client
void respond_to_client(int childfd, Buffer_T *response);

char *concat_host_and_port(char *hostname, int portno);