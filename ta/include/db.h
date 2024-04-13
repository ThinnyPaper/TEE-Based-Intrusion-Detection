#ifndef _DB_H_INCLUDED
#define _DB_H_INCLUDED

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
//#include <sys/stat.h>
#include <limits.h>
#include "list.h"
/*
typedef struct db_line {
  //char* filename;
  char fullpath[PATH_MAX];

  unsigned int perm; //权限
  unsigned int uid; //文件所有者的用户ID
  unsigned int gid; //文件所属组的组ID
  long long atime; //文件的最后访问时间
  long long ctime; //文件状态最后更改的时间
  long long mtime; //文件内容最后修改的时间
  unsigned long long inode; //文件的inode编号
  unsigned int nlink; //链接到该文件的硬链接数
  long long size; //文件大小
  long long bcount;
  //byte* hashsums[num_hashes];//计算所有
  //unsigned char hash_md5[20];
  unsigned char* sha256;
  unsigned char hash_sha256[32];
  unsigned char hash_whirlpool[64];


} db_line;*/
typedef struct db_line {
  //char* filename;
  //char fullpath[PATH_MAX]; //TODO：改成指针，在avoke_command中分开传递

  unsigned int perm; //权限
  uid_t uid; //文件所有者的用户ID
  gid_t gid; //文件所属组的组ID
  time_t atime; //文件的最后访问时间
  time_t ctime; //文件状态最后更改的时间
  time_t mtime; //文件内容最后修改的时间
  ino_t inode; //文件的inode编号
  nlink_t nlink; //链接到该文件的硬链接数
  off_t size; //文件大小
  blkcnt_t bcount;
  //byte* hashsums[num_hashes];//计算所有
  //unsigned char hash_md5[20];
  //unsigned char* sha256;
  unsigned char hash_sha256[32];
  unsigned char hash_whirlpool[64];


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



#endif
