#include <stdlib.h>
#include "list.h"
#include "util.h"

/* list
 * limitations:
 * Only the head knows where the tail is
 * Every item knows where the head is
 
 * And that is not true anymore. 
 * Now list has header which knows head and tail.
 * Every item knows header.
 
 */



/* list_append()
 * append an item to list
 * returns the head
 * The first argument is the head of the list
 * The second argument is the data to be added
 * Returns list head
 */


/* 
 * Some way to handle mallocs failure would be nice.
 */

node* list_append(node* listp,void*data)
{
  node* newitem=NULL;
  newitem = checked_malloc(sizeof(node));
  
  if(listp==NULL){
    list_header* header = checked_malloc(sizeof(list_header));
    
    newitem->data=data;
    newitem->header=header;
    newitem->next=NULL;
    newitem->prev=NULL;

    header->head=newitem;
    header->tail=newitem;

    return newitem;
  }else {
    
    /* We have nonempty list.
     * add to last
     */
    
    newitem->prev=listp->header->tail;
    newitem->next=NULL;
    newitem->data=data;
    newitem->header=listp->header;
    
    listp->header->tail->next=newitem;
    listp->header->tail=newitem;
    return listp;
  }
  /* Not reached */
  return NULL;
}

/*
 * delete_list_item()
 * delete a item from list
 * returns head of a list.
 */

node* list_delete_item(node* item){
  node* r;


  if (item==NULL) {
      printf("tried to remove from empty list");
    return item;
  }
  
  if (item->header->head==item->header->tail) {
    /*
     * Ollaan poistamassa listan ainoaa alkiota.
     * T�ll�in palautetaan NULL
     */
    free(item->header);
    free(item);
    return NULL;
  }
  
  /* 
   * Nyt meill� on listassa ainakin kaksi alkiota 
   *  
   */

  /* poistetaan listan viimeist� alkiota */

  if (item==item->header->tail){
    
    r=item->prev;
    item->header->tail=r;
    r->next=NULL;
    r=r->header->head;
    free(item);
    return r;
  }

  /*
   */  
  if (item==item->header->head) {
    
    r=item->next;
    item->header->head=r;
    r->prev=NULL;
    r=r->header->head;
    
    free(item);
    return r;
  }
  
  r=item->prev;

  item->prev->next=item->next;
  item->next->prev=item->prev;
  
  free(item);
  r=r->header->head;
  
  return r;
  
}
