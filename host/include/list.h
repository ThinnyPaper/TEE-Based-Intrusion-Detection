#ifndef _LIST_H
#define _LIST_H

typedef struct node {
  struct node* next;
  struct node* prev;
  struct list_header* header;
  void* data;
} node;

typedef struct list_header{
  struct node* head;
  struct node* tail;
}list_header;

node* list_append(node* listp,void*data);
node* list_delete_item(node* item);

#endif 


