
typedef struct db_line {
  byte* hashsums[num_hashes];
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
