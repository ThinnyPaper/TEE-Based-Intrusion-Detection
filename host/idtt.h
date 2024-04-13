#ifndef _IDTT_H_INCLUDED
#define _IDTT_H_INCLUDED

#include <config.h>
#include <stdlib.h>
#include <list.h>

#define RETOK 1
#define RETFAIL 0

#define NO_ACTION 0
#define DO_INIT (1<<0)
#define DO_CHECKALL (1<<1)
#define DO_CHECK (1<<2)

typedef struct idtt_config {
  char *hostname;

  node* filelist;
  
  char* config_file;

  /* What are we supposed to do */
  int action;

} idtt_config;


#endif