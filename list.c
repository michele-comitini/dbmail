/* $Id$
 * functions to create lists and add/delete items
 * (c) 2001 eelco@eelco.com */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debug.h"
#include "list.h"

extern void func_memtst (char filename[255],int line,int tst);

void list_init (struct list *tlist)
{
  tlist->start=NULL;
  tlist->current=NULL;
  tlist->itptr=NULL;
  tlist->total_nodes=0;
  tlist->list_inited=1;
}

/*
 * list_freelist()
 *
 * frees a list and all the memory associated with it
 */
void list_freelist(struct list *list)
{
  struct element *start = list_getstart(list);

  /* check if list exists */
  if (!list)
    return;

  /* free rest of list */
  list_freelist(start->nextnode);
  
  /* free this item */
  free(start->data);
  free(start);
  start = NULL;
}

 
/* 
 * list_nodeadd()
 *
 * Adds a node to a linked list (list structure). 
 * New item will be FIRST element of new linked list.
 *
 * returns NULL on failure or first element on success
 */
struct element *list_nodeadd(struct list *tlist, void *data,
			     size_t dsize)
{
  struct element *p;

  if (!tlist)
    return NULL; /* cannot add to non-existing list */
   
  p=tlist->start;
	
  tlist->start=(struct element *)malloc(sizeof(struct element));
	
  /* allocating memory */
#ifdef USE_EXIT_ON_ERROR
  memtst(tlist->start==NULL);
  memtst((tlist->start->data=(void *)malloc(dsize))==NULL);
#else
  if (!tlist->start)
    return NULL;

  tlist->start->data=(void *)malloc(dsize);
  if (!tlist->start->data)
    return NULL;

#endif
  
  /* copy data */
  tlist->start->data = memcpy(tlist->start->data,data,dsize);

  tlist->start->nextnode=p;

	/* updating node count */
  tlist->total_nodes++;
  return tlist->start;
}


/*
 * list_nodedel()
 *
 * removes the item containing 'data' from the list preserving a valid linked-list structure.
 *
 * returns
 */
struct element *list_nodedel(struct list *tlist,void *data)
{
  struct element *temp;
  struct element *item;
  item=NULL;

  if (!tlist)
    return NULL;

  temp=tlist->start;

  /* checking if lists exist else return NULL*/
  if (temp==NULL) return NULL;
	
  while (temp!=NULL) /* walk the list */
    { 
      if (temp->data==data)
	{
	  if (item==NULL)
	    {
	      tlist->start=temp->nextnode;
	      free (temp->data);
	      free ((struct element *)temp);
	      break;
	    }
	  else 
	    {
	      item->nextnode=temp->nextnode;
	      free(temp->data); /* freeing memory */
	      free ((struct element *)temp);
	      break;
	    }
	  /* updating node count */
	  tlist->total_nodes--;
	} 
      item=temp;
      temp=temp->nextnode;
    }        

  return NULL;
}

struct element *list_getstart(struct list *tlist)
{
  return (tlist) ? tlist->start : NULL;
}

long list_totalnodes(struct list *tlist)
{
  return (tlist) ? tlist->total_nodes : -1; /* a NULL ptr doesnt even have zero nodes (?) */
}

void list_showlist(struct list *tlist)
{
  struct element *temp;

  if (!tlist)
    {
      trace(TRACE_MESSAGE,"list_showlist(): NULL ptr received\n");
      return;
    }

  temp=tlist->start;
  while (temp!=NULL)
    {
      trace (TRACE_MESSAGE,"list_showlist():item found [%s]\n",(char *)temp->data);
      temp=temp->nextnode;
    }
} 
