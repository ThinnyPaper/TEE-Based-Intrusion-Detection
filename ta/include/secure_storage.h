#ifndef SECURE_STORAGE_H_INCLUDE
#define SECURE_STORAGE_H_INCLUDE

#include "config.h"


typedef struct {
    char* path;   
    bool is_occupied;
} HashIndexEntry;

#define TABLE_SIZE MAX_FILE_NUMEBER
HashIndexEntry hash_table[TABLE_SIZE];
unsigned int cnt_file=0;


#endif