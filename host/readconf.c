#include "readconf.h"

#include <libconfig.h>
#include <string.h>
#include <stdlib.h>
#include <glob.h>
#include "log.h"
#include "db.h"
#include "util.h"


static node* deal_regex_path(const char* path, node* pathlist){
  //TODO：获取绝对路径

  
  glob_t* glob_result=checked_malloc(sizeof(glob_result));

    int rt = glob(path, 0, NULL, glob_result);
    if (rt == 0) {
        printf("Found %zu matching file(s):\n", glob_result->gl_pathc);
        for (size_t i = 0; i < glob_result->gl_pathc; ++i) {
            //char* fullpath="asd";//glob_result->gl_pathv[i];
            pathlist=list_append(pathlist,glob_result->gl_pathv[i]);
            printf("Found match file: %s\n", glob_result->gl_pathv[i]);
            log_msg(LOG_LEVEL_INFO, "Found match file: %s", glob_result->gl_pathv[i]);
        }
    } else {
        printf("No match file found using %s\n", path);
        log_msg(LOG_LEVEL_INFO, "No match file found using %s", path);
    }
    free(glob_result);
  return pathlist;
}

void readconf(char* configfile, db_config* conf){
  //读文件列表
  config_t cfg;
  config_setting_t *setting;
  const char *file_path;

  config_init(&cfg);

  if (!config_read_file(&cfg, configfile))
  {
      log_msg(LOG_LEVEL_ERROR,"read conig file error: %s\n", config_error_file(&cfg));
      config_destroy(&cfg);
      exit(1);
  }

  // 读取 file_path 数组
  setting = config_lookup(&cfg, "file_path");
  if (setting != NULL)
  {
      int length = config_setting_length(setting);
      for(int i = 0; i < length; ++i)
      {
          file_path = config_setting_get_string_elem(setting, i);
          conf->filelist=deal_regex_path(file_path, conf->filelist);
      }
  }


  config_destroy(&cfg);

}
