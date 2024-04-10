#ifndef _DB_CONFIG_H_INCLUDED
#define _DB_CONFIG_H_INCLUDED
#include "config.h"
#include "attributes.h"
#include "report.h"
#include "types.h"
#include <unistd.h>
#include <stdio.h>
#include <pcre.h>

#define E2O(n) (1<<n)

#include "list.h"

#ifdef WITH_XATTR /* Do generic user Xattrs. */
#include <sys/xattr.h>
#include <attr/attributes.h>
#ifndef ENOATTR
# define ENOATTR ENODATA
#endif
#endif

typedef struct xattr_node 
{
 char *key;
 byte *val;
 size_t vsz;
} xattr_node;

typedef struct xattrs_type
{
  size_t num;
  size_t sz;
  struct xattr_node *ents;
} xattrs_type;

#ifdef WITH_CAPABILITIES
#include <sys/capability.h>
#endif

#ifdef WITH_MHASH
#include <mhash.h>
#endif


#define RETOK 0
#define RETFAIL -1

#define DO_INIT      (1<<0)
#define DO_CHECKALL  (1<<1)
#define DO_CHECK     (1<<2)

#include "url.h"

/* TIMEBUFSIZE should be exactly ceil(sizeof(time_t)*8*ln(2)/ln(10))
 * Now it is ceil(sizeof(time_t)*2.5)
 * And of course we add one for end of string char
 */

#define TIMEBUFSIZE (((sizeof(time_t)*5+1)>>1)+1)


#include "seltree.h"

#include "hashsum.h"

typedef struct db_line {
  byte* hashsums[num_hashes];

  /* Something here.. */

  mode_t perm;
  mode_t perm_o; /* Permission for tree traverse */
  long uid; /* uid_t */
  long gid; /* gid_t */
  time_t atime;
  time_t ctime;
  time_t mtime;
  long inode; /* ino_t */
  long nlink; /* nlink_t */

  long long size; /* off_t */
  long long bcount; /* blkcnt_t */
  char* filename;
  char* fullpath;
  char* linkname;

  char *cntx;

  xattrs_type* xattrs;


  char* capabilities;

  /* Attributes .... */
  DB_ATTR_TYPE attr;

} db_line;

typedef struct database {
    url_t* url;

    char *filename;
    int linenumber;
    char *linebuf;

    void *fp;

    long lineno;
    ATTRIBUTE* fields;
    int num_fields;
    void *buffer_state;
    struct md_container *mdc;
    struct db_line *db_line;

} database;

typedef struct db_config {
  char *hostname;

  DB_ATTR_TYPE db_out_attrs;

  char *check_path;
  RESTRICTION_TYPE check_file_type;
  
  char* config_file;

  int database_add_metadata;
  int report_detailed_init;
  int report_base16;
  int report_quiet;
  bool report_append;

  DB_ATTR_TYPE report_ignore_added_attrs;
  DB_ATTR_TYPE report_ignore_removed_attrs;
  DB_ATTR_TYPE report_ignore_changed_attrs;
  DB_ATTR_TYPE report_force_attrs;

  list* report_urls;
  REPORT_LEVEL report_level;

  /* defsyms is a list of symba*s */
  list* defsyms;
  /* so is groupsyms */
  list* groupsyms;

  /* What are we supposed to do */
  int action;

  /* Should we catch errors from mmapping */
  int catch_mmap;

  time_t start_time;
  time_t end_time;

  int symlinks_found;
  DB_ATTR_TYPE attr;

  int warn_dead_symlinks;

  int report_grouped;

  int report_summarize_changes;

  char* root_prefix;
  int root_prefix_length;

  char* limit;
  pcre* limit_crx;

  struct seltree* tree;

} db_config;

#endif
