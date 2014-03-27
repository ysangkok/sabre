/*
    Sabre Fighter Plane Simulator 
    Copyright (c) 1997/1998 Dan Hammer
    Portions Donated By Antti Barck
	
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 1, or (at your option)
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
/*************************************************
 *         Sabre Fighter Plane Simulator         *
 * File   : dhlist.C                             *
 * Date   : April, 1998                          *
 * Author : Dan Hammer                           *
 *************************************************/
#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include "dhlist.h"

Dhlist *new_dhlist(Dhlist *list)
{
  Dhlist *theDhlist;

  if (list != NULL)
    {
      theDhlist = list;
      theDhlist->on_heap = 0;
    }
  else
    {
      theDhlist = (Dhlist*) malloc(sizeof(Dhlist));
      if (theDhlist != NULL)
	theDhlist->on_heap = 1;
    }
  if (theDhlist != NULL)
    {
      theDhlist->nodes = theDhlist->cur_node = theDhlist->last_node = NULL;
      theDhlist->n_nodes = 0;
    }
  return theDhlist;
}

int dhlist_count(Dhlist *this)
{
  return (this->n_nodes);
}

int delete_node(DhlistNode *n)
{
  if (n != NULL)
    free(n);
  return 1;
}

int delete_node_object(DhlistNode *n)
{
  if ((n != NULL) && (n->object != NULL))
    {
      if (n->delete_f)
	(*(n->delete_f))(n->object);
      else
	free(n->object);
      n->object = NULL;
    }
  return 1;
}

int dhlist_remove_node(Dhlist *this, DhlistNode *n, int free)
{
  DhlistNode *cur;
  DhlistNode *nxt = NULL;
  DhlistNode *prev = NULL;
  
  if (n == this->nodes)
    {
      nxt = n->next;
      if (free)
	delete_node_object(n);
      delete_node(n);
      this->nodes = nxt;
      this->n_nodes--;
      return 1;
    }
  prev = this->nodes;
  cur = this->nodes->next;
  while (cur != NULL)
    {
      nxt = cur->next;
      if (cur == n)
	{
	  if (free)
	    delete_node_object(n);
	  delete_node(n);
	  prev->next = nxt;
	  this->n_nodes--;
	  return 1;
	}
      prev = cur;
      cur = nxt;
    }
  return 0;
}

void dhlist_do_for_each(Dhlist *this, doeach_function f)
{
  DhlistNode *n = this->nodes;
  DhlistNode *nxt;

  while (n != NULL)
    {
      nxt = n->next;
      if (!((*f)(n)))
	break;
      n = nxt;
    }
}

void dhlist_do_for_each_object(Dhlist *this, doeach_object_function f)
{
  DhlistNode *n ;
  DhlistNode *nxt;

  n = this->nodes;
  while (n != NULL)
    {
      nxt = n->next;
      if (!((*f)(n->object)))
	break;
      n = nxt;
    }
}

void *dhlist_find(Dhlist *this, compare_object_function f, void *v)
{
  DhlistNode *n;
  void *result = NULL;

  n = this->nodes;
  while (n)
    {
      if ((*f)(n->object,v))
	{
	  result = n->object;
	  break;
	}
      n = n->next;
    }
  return result;
}

void dhlist_free(Dhlist *this, int fr)
{
  if (fr)
    dhlist_do_for_each(this,delete_node_object);
  dhlist_do_for_each(this,delete_node);
  if (this->on_heap)
    free(this);
}

void dhlist_flush(Dhlist *this, int free)
{
  if (free)
    dhlist_do_for_each(this,delete_node_object);
  dhlist_do_for_each(this,delete_node);
  this->nodes = this->last_node = this->cur_node = NULL;
  this->n_nodes = 0;
}

int dhlist_append(Dhlist *this, void *object, delete_object_function *df)
{
  DhlistNode *new_node = (DhlistNode *) malloc(sizeof(DhlistNode));

  if (new_node == NULL)
    return 0;
  new_node->next = NULL;
  new_node->object = object;
  new_node->delete_f = df;
  if (this->nodes == NULL)
    this->nodes = this->last_node = new_node;
  else
    {
      this->last_node->next = new_node;
      this->last_node = new_node;
    }
  this->n_nodes++;
  return 1;
}

int dhlist_delete(Dhlist *this, void *object, int fr)
{
  DhlistNode Dhlist_top,*a,*b;
  int result = 0;

  Dhlist_top.next = this->nodes;
  a = &Dhlist_top;
  b = a->next;
  while (b)
    {
      if (b->object == object)
	{
	  result = 1;
	  if (fr)
	    {
	      if (b->delete_f)
		(*(b->delete_f))(object);
	      else
		free(object);
	    }
	  a->next = b->next;
	  if (b == this->last_node)
	    this->last_node = a;
	  if (b == this->cur_node)
	    this->cur_node = b->next;
	  free(b);
	  this->n_nodes--;
	  break;
	}
      a = b;
      b = a->next;
    }
  if (result)
    this->nodes = Dhlist_top.next;
  return result;
}

void dhlist_start_iterator(Dhlist *this)
{
  this->cur_node = this->nodes;
}

void *dhlist_next_object(Dhlist *this)
{
  void *result = NULL;

  if (this->cur_node != NULL)
    {
      result = this->cur_node->object;
      this->cur_node = this->cur_node->next;
    }
  return result;
}

void dhlist_start_xiterator(Dhlist *this, DhlistIter *iter)
{
  iter->dhlist = this;
  iter->cur_node = this->nodes;
}

void *dhlist_next_xobject(DhlistIter *iter)
{
  void *result = NULL;

  if (iter->cur_node != NULL)
    {
      result = iter->cur_node->object;
      iter->cur_node = iter->cur_node->next;
    }
  return result;
}

void *dhlist_start_iterator_at(Dhlist *this, void *thing)
{
  void *v;
  void *result = NULL;

  dhlist_start_iterator(this);
  while ((v = dhlist_next_object(this)) != NULL)
    {
      if (v == thing)
	{
	  result = v;
	  break;
	}
    }
  return result;
}

void *dhlist_get_head(Dhlist *this)
{
  if (this->nodes)
    return (this->nodes->object);
  else
    return NULL;
}


DhlistNode *dhlist_next_node(Dhlist *this)
{
  DhlistNode *result = NULL;

  if (this->cur_node != NULL)
    {
      result = this->cur_node;
      this->cur_node = this->cur_node->next;
    }
  return result;
}

void dhlist_sort(Dhlist *this, compare_object_function compare_f)
{
  DhlistNode *y,*d,*u,*x;
  DhlistNode Dhlist_top;
  int change;

  Dhlist_top.next = this->nodes;

  y = Dhlist_top.next;
  d = y;
  change = 0;
  while (y != NULL)
    {
      u = &Dhlist_top;
      x = u->next;
      while ((x != y) && (!change))
	{
	  /* functions returns true if x > y */
	  if ((*compare_f)(x->object,y->object))
	    change = 1;
	  if (!change)
	    {
	      u = x;
	      x = u->next;
	    }
	}
      if (change)
	{
	  u->next = y;
	  d->next = y->next;
	  y->next = x;
	  y = d->next;
	  change = 0;
	}
      else
	{
	  d = y;
	  y = d->next;
	}
    }
  this->nodes = Dhlist_top.next;
}


#if 0
static int print_dhlist(void *str)
{
  printf((char *)(str));
  return 1;
}

static int compare_dhlist(void *v, void *v1)
{
  char *str1 = (char *) v;
  char *str2 = (char *) v1;
  return (strcmp(str1,str2) > 0);
}

main()
{
  void *obj;
  Dhlist *theDhlist = new_dhlist(NULL);
  char *fourth_item = "Fourth Item\n";
  if (theDhlist)
    {
      dhlist_append(theDhlist,"First Item\n",NULL);
      dhlist_append(theDhlist,"Second Item\n",NULL);
      dhlist_append(theDhlist,"Third Item\n",NULL);
      dhlist_append(theDhlist,fourth_item,NULL);
      dhlist_append(theDhlist,"Fifth Item\n",NULL);
      dhlist_append(theDhlist,"Another Item\n",NULL);
      dhlist_append(theDhlist,"another little item\n",NULL);
      dhlist_append(theDhlist,"bye the bye\n",NULL);
      dhlist_append(theDhlist,"012345\n",NULL);
      printf("Getting Dhlist head\n");
      obj = dhlist_get_head(theDhlist);
      if (obj)
	printf("Head: %s\n", (char *)obj);
      printf("Testing do_for_each_object function\n");
      dhlist_do_for_each_object(theDhlist,print_dhlist);
      printf("Testing iteration\n");
      dhlist_start_iterator(theDhlist);
      while ((obj = dhlist_next_object(theDhlist)) != NULL)
	printf((char *)obj);
      printf("Testing sort\n");
      dhlist_sort(theDhlist,compare_dhlist); 
      printf("Printing sort\n");
      dhlist_do_for_each_object(theDhlist,print_dhlist);
      printf("Testing dhlist_start_iterator at\n");
      obj = dhlist_start_iterator_at(theDhlist,fourth_item);
      printf((char *)obj);
      while ((obj = dhlist_next_object(theDhlist)) != NULL) 
	printf((char *)obj);
    }
}
#endif
