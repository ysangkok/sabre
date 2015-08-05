/*
    Sabre Fighter Plane Simulator 
    Copyright (c) 1997/98 Dan Hammer
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
 *           Sabre Fighter Plane Simulator              *
 * Version: 0.1                                  *
 * File   : group_3d.h
 * Date   : March, 1997                          *
 * Author : Dan Hammer                           *
 *************************************************/
#ifndef GROUP_3D_H
#define GROUP_3D_H


class C_3DObject_Group : public C_3DObject_List
{
public:
  bool visible;
  bounding_cube bounds;
  static int min_pixel_width;

  C_3DObject_Group();
  
  void addObject(C_3DObject_Base *);
  void addDrawList(DrawList &dlist, Port_3D &port);
  bool isVisible(Port_3D &port);

  int getNObjects()
    {
      return (n_objects);
    }

  C_3DObject_Base *getObject(int n)
    {
      if (n >= 0 && n < n_objects)
	return (objects[n]);
      else
	return (NULL);
    }
} ;

bool isVisible(Port_3D &port, bounding_cube &bounds);
bool isVisible(Port_3D &port, Port_3D &ref_port, bounding_cube &bounds);

#endif


