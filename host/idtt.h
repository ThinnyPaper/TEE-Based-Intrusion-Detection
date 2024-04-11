#ifndef _IDTT_H_INCLUDED
#define _IDTT_H_INCLUDED

#include <config.h>
#include <stdlib.h>
#include <list.h>


typedef struct db_config {
  char *hostname;

  node* filelist;
  
  char* config_file;

  /* What are we supposed to do */
  int action;

} db_config;


#endif