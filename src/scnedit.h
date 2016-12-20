/*
    SABRE Fighter Plane Simulator 
    Copyright (c) 1997 Dan Hammer
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
 *           SABRE Fighter Plane Simulator              *
 * Version: 0.1                                  *
 * File   : scnedit.h                            *
 * Date   : March, 1997                          *
 * Author : Dan Hammer                           *
 *************************************************/

#ifndef SCNEDIT_H
#define SCNEDIT_H

class SceneEdit
{
public:
  const char             *world_file;
  Earth_Watch2     earth;
  int              zz;
  Rect             cliprect;
  Port_3D          port;
  MTimer				 timer;
  int              (*messageHook)(void);

  C_3DInfoManager  info_manager;
  C_DrawList2      clist;

  SceneEdit(const char *world_file,	Mouse *mouse, int (*) (void) = NULL);
  ~SceneEdit();
  void doEdit();
  void setup();
  void readWorldFile();
  void edit();
  void drawScene(Port_3D &);
  void show_message(int bot, char *str, ... );
};

#endif
