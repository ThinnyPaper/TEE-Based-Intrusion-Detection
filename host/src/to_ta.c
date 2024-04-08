#include <tee_client_api.h>

#include "seltree_struct.h"
#include "seltree.h"
#include "db.h"



void* gen_tree_to_mm(seltree* node){



}


void gen_tree(seltree* node){
    list* r=NULL;
    if (node->checked&DB_NEW) {
        db_writeline(node->new_data,conf);
        if (node->new_data==NULL||conf==NULL) return RETOK;
        //append to mm

      }
    }
  return RETFAIL;


        
        if (node->checked&NODE_FREE) {
            free_db_line(node->new_data);
            free(node->new_data);
            node->new_data=NULL;
        }
    }
    for (r=node->childs;r;r=r->next) {
        gen_tree((seltree*)r->data);
    }
}