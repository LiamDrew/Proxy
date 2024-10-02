#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <assert.h>

typedef struct {
    char *buf;
    int size;
} Buffer_T;

void free_Buffer_T(Buffer_T **buffer);

Buffer_T *new_Buffer_T(char *buf, int sizef);



typedef struct {
    unsigned char *key;
    Buffer_T *response;
    unsigned long max_age;
    unsigned long insert_time;
    unsigned long access_time;
} Entry_T;

Entry_T *create_entry(unsigned char *key, Buffer_T *response, 
    unsigned long max_age, unsigned long insert_time, 
    unsigned long access_time);

void free_entry(Entry_T **entry);

typedef struct
{
    unsigned long buckets;
    unsigned long size;
    Entry_T **entries;
} HashTable_T;

HashTable_T *create_table(unsigned capacity);

void free_table(HashTable_T **table);

bool add_entry_to_table(HashTable_T *table, Entry_T *new_entry);

bool update_entry_in_table(HashTable_T *table, Entry_T *entry);

Entry_T *get_entry_from_table(HashTable_T *table, unsigned char *key);

bool delete_entry_from_table(HashTable_T *table, Entry_T *to_delete);

Entry_T *find_entry_to_delete(HashTable_T *table);