#ifndef _LIST_H
#define _LIST_H

typedef struct node {
  struct node* next;
  struct node* prev;

  struct list_header* header;

  /*
  struct list* head;
  struct list* tail;
  */

  void* data;
} node;

typedef struct list_header{
  
  struct node* head;
  struct node* tail;
  
}list_header;

//list* list_sorted_insert(list* listp, void* data, int (*compare) (const void*, const void*));

node* list_append(node* listp,void*data);



node* list_delete_item(node* item);

#endif 


