/*
 * IDTT (Intrusion Detection Tool Based on TEE)
 *
 * Copyright (C) 2024 Yang Zhe
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "readconf.h"

#include <libconfig.h>
#include <string.h>
#include <stdlib.h>
#include <glob.h>
#include "util.h"


static node* deal_regex_path(const char* path, node* pathlist){
  //TODO：处理～/路径

  
  glob_t* glob_result=checked_malloc(sizeof(glob_result));

    int rt = glob(path, GLOB_ERR, NULL, glob_result);
    if (rt == 0) {
        printf("Found %zu matching file(s):\n", glob_result->gl_pathc);
        for (size_t i = 0; i < glob_result->gl_pathc; ++i) {
            //char* fullpath="asd";//glob_result->gl_pathv[i];
            pathlist=list_append(pathlist,glob_result->gl_pathv[i]);
            printf("Found match file: %s\n", glob_result->gl_pathv[i]);
        }
    } else if(rt==GLOB_ABORTED){
        printf("Can not open path: %s\n", path);
    } else{
        printf("No match file found using %s%d\n", path,GLOB_ABORTED);
    }
    free(glob_result);
  return pathlist;
}

void readconf(char* configfile, idtt_config* conf){
  //读文件列表
  config_t cfg;
  config_setting_t *setting;
  const char *file_path;

  config_init(&cfg);

  if (!config_read_file(&cfg, configfile))
  {
      printf("read conig file error: %s\n", config_error_file(&cfg));
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
