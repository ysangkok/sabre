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
 * File   : sarray.h                             *
 * Date   : April, 1998                          *
 * Author : Dan Hammer                           *
 *************************************************/
#ifndef SARRAY_H
#define SARRAY_H

#ifdef __cplusplus
#include <stdio.h>
#include "sobject.h"

class sObjectArray : public sObject
{
  sObject **objs;       // pointers to objects
  int     size;         // size of array (max) 
  int     n;            // how many assigned so far
  int     owns;         // do we delete objects on Destroy()
public:
  void Create(int size, bool owns = true);
  void Destroy();
  void Flush();

  sObjectArray()
  {
    n = 0;
    size = 0;
    owns = 0;
    objs = NULL;
  }

  bool IsA() const
  {
    return (sOBJECT_ARRAY_T);
  }

  sObjectArray(int siz, bool own = true)
  {
    n = 0;
    objs = NULL;
    Create(siz,own);
  }

  ~sObjectArray();

  sObject * operator[](int idx) const;

  int Count() const
  {
    return (n);
  }

  int Size() const
  {
    return (size);
  }

  int Add(sObject *obj);
  int Append(sObject *obj);

  bool Owns() const
  {
    return (owns);
  }

  void SetOwnership(bool own)
  {
	this->owns = own;
  }

  int Remove(int idx);
};

#endif
#endif
