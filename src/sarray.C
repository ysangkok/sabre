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
 * File   : sarray.C                             *
 * Date   : April, 1998                          *
 * Author : Dan Hammer                           *
 * Simple array functions                        *
 *************************************************/
#include "sarray.h"

void sObjectArray::Create(unsigned int siz, bool own)
{
  if (objs)
    Destroy();
  this->size = siz;
  this->owns = own;
  objs = new sObject *[size];
  for (int i=0;i<size;i++)
    objs[i] = NULL;
  n = 0;
}

sObjectArray::~sObjectArray()
{
  Destroy();
}

void sObjectArray::Destroy()
{
  if (objs)
    {
      if (owns)
	{
	  for (int i=0;i<n;i++)
	    {
	      if (objs[i])
		delete objs[i];
	    }
	}
      delete [] objs;
      objs = NULL;
    }
  n = 0;
}

void sObjectArray::Flush()
{
  if (objs)
    {
      for (int i=0;i<size;i++)
	{
	  if (objs[i] != NULL)
	    {
	      if (owns)
		delete objs[i];
	      objs[i] = NULL;
	    }
	}
    }
  n = 0;
}

unsigned int sObjectArray::Add(sObject *obj)
{
  int result = -1;
  /*
   * First check for room in any
   * previously assigned slots
   */
  for (int i=0;i<n;i++)
    {
      if (objs[i] == NULL)
	{
	  result = i;
	  objs[i] = obj;
	  break;
	}
    }
  /*
   * if no room, add to end
   */
  if (result == -1)
    result = Append(obj);
  return result;
}

unsigned int sObjectArray::Append(sObject *obj)
{
  int result = -1;
  if (n < size)
    {
      result = n;
      objs[n++] = obj;
    }
  return result;
}

unsigned int sObjectArray::Remove(unsigned int idx)
{
  int result = -1;
  if (idx < n)
    {
      if (objs[idx] != NULL)
	{
	  if (owns)
	    delete objs[idx];
	  objs[idx] = NULL;
	  result = idx;
	}
    }
  return result;
}

sObject *sObjectArray::operator [](unsigned int idx) const
{
  if (idx < n)
    return (objs[idx]);
  else
    return (NULL);
}

