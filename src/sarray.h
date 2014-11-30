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
#ifndef __sarray_h
#define __sarray_h

#ifdef __cplusplus
#include <stdio.h>
#include "sobject.h"

class sObjectArray : public sObject
{
  sObject **objs;       // pointers to objects
  unsigned int size;    // size of array (max) 
  unsigned int n;       // how many assigned so far
  bool    owns;         // do we delete objects on Destroy()
public:
  void Create(unsigned int size, bool owns = true);
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

  sObjectArray(unsigned int siz, bool own = true)
  {
    n = 0;
    objs = NULL;
    Create(siz,own);
  }

  ~sObjectArray();

  sObject * operator[](unsigned int idx) const;

  unsigned int Count() const
  {
    return (n);
  }

  unsigned int Size() const
  {
    return (size);
  }

  unsigned int Add(sObject *obj);
  unsigned int Append(sObject *obj);

  bool Owns() const
  {
    return (owns);
  }

  void SetOwnership(bool own)
  {
	this->owns = own;
  }

  unsigned int Remove(unsigned int idx);
};

#endif
#endif
