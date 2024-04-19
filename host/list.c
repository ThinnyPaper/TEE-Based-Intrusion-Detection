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

#include <stdlib.h>
#include <stdio.h>
#include "list.h"
#include "util.h"

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
