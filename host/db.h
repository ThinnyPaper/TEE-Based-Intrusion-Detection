#ifndef _DB_H_INCLUDED
#define _DB_H_INCLUDED

#define RETOK 0
#define RETFAIL -1

#define DO_INIT     (1<<0)
#define DO_CHECKALL (1<<1)
#define DO_CHECK     (1<<2)

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "list.h"
typedef struct db_line {
  //byte* hashsums[num_hashes];//计算所有
  mode_t perm; //权限
  long uid; //文件所有者的用户ID
  long gid; //文件所属组的组ID
  time_t atime; //文件的最后访问时间
  time_t ctime; //文件状态最后更改的时间
  time_t mtime; //文件内容最后修改的时间
  long inode; //文件的inode编号
  long nlink; //链接到该文件的硬链接数
  long long size; //文件大小

  char* filename;
  char* fullpath;


} db_line;

typedef struct database {

    char *filename;
    int linenumber;
    char *linebuf;

    void *fp;

    long lineno;
    //ATTRIBUTE* fields;
    int num_fields;
    void *buffer_state;
    //struct md_container *mdc;
    struct db_line *db_line;

} database;

typedef struct db_config {
  char *hostname;

  node* filelist;
  
  char* config_file;

  /* What are we supposed to do */
  int action;

} db_config;

#endif
