#ifndef _DB_H_INCLUDED
#define _DB_H_INCLUDED

#define RETOK 0
#define RETFAIL -1

#define DO_INIT     (1<<0)
#define DO_CHECKALL (1<<1)
#define DO_CHECK     (1<<2)

#include <unistd.h>
#include <stdio.h>
//#include <sys/types.h>
//#include <sys/stat.h>
#include <limits.h>
#include "list.h"
/*
hashsum_t hashsums[] = {
    { attr_md5,             16 },
    { attr_sha1,            20 },
    { attr_sha256,          32 },
    { attr_sha512,          64 },
    { attr_rmd160,          20 },
    { attr_tiger,           24 },
    { attr_crc32,           4  },
    { attr_crc32b,          4  },
    { attr_haval,           32 },
    { attr_whirlpool,       64 },
    { attr_gostr3411_94,    32 },
    { attr_stribog256,      32 },
    { attr_stribog512,      64 },
};
#ifdef WITH_MHASH
int algorithms[] = { /* order must match hashsums array 
  MHASH_MD5,
  MHASH_SHA1,
  MHASH_SHA256,
  MHASH_SHA512,
  MHASH_RIPEMD160,
  MHASH_TIGER,
  MHASH_CRC32,
  MHASH_CRC32B,
  MHASH_HAVAL,
#ifdef HAVE_MHASH_WHIRLPOOL
  MHASH_WHIRLPOOL,
#else
  -1,
#endif
  MHASH_GOST,
  -1, /* stribog256 not available 
  -1, /* stribog512 not available 
};
#endif
*/

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
