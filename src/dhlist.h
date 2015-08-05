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
 * File   : dhlist.h                             *
 * Date   : April, 1998                          *
 * Author : Dan Hammer                           *
 * Linked list implementation in C               *
 *************************************************/
#ifndef DHLIST_H
#define DHLIST_H

typedef void delete_object_function(void *);
typedef int compare_object_function(void *, void *);

typedef struct _DhlistNode
{
  struct _DhlistNode *next;
  void *object;
  delete_object_function *delete_f;  
} DhlistNode;

typedef struct _Dhlist
{
  DhlistNode *nodes;
  DhlistNode *cur_node;
  DhlistNode *last_node;
  int on_heap;
  int n_nodes;
} Dhlist;

typedef struct _DhlistIter
{
  DhlistNode *cur_node;
  Dhlist *dhlist;
} DhlistIter;

typedef int doeach_function (DhlistNode *);
typedef int doeach_object_function (void *);

#ifdef __cplusplus
extern "C" {
#endif
Dhlist *new_dhlist(Dhlist *list);
int dhlist_count(Dhlist *list);
void dhlist_free(Dhlist *list, int free);
void dhlist_flush(Dhlist *list, int free);
int dhlist_append(Dhlist *list, void *thing, delete_object_function *df);
int dhlist_delete(Dhlist *list, void *thing, int free);
int dhlist_insert_after(Dhlist *list, void *thing, void *after);
int dhlist_insert_before(Dhlist *list, void *thing, void *before);
void dhlist_do_for_each(Dhlist *list, doeach_function);
void dhlist_do_for_each_object(Dhlist *list, doeach_object_function);
void dhlist_start_iterator(Dhlist *list);
void *dhlist_start_iterator_at(Dhlist *list, void *thing);
void *dhlist_next_object(Dhlist *list);
DhlistNode *dhlist_next_node(Dhlist *list);
int dhlist_remove_node(Dhlist *list, DhlistNode *n, int free);
int delete_node(DhlistNode *n);
int delete_node_object(DhlistNode *n);
void *dhlist_find(Dhlist *list, compare_object_function, void *);
void dhlist_sort(Dhlist *list, compare_object_function);
void *dhlist_get_head(Dhlist *list);
void dhlist_start_xiterator(Dhlist *list, DhlistIter *iter);
void *dhlist_next_xobject(DhlistIter *iter);
#ifdef __cplusplus
}
#endif
#endif




