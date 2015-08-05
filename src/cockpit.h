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
 * File   : cockpit.h                            *
 * Date   : March, 1997                          *
 * Author : Dan Hammer                           *
 *************************************************/
#ifndef COCKPIT_H
#define COCKPIT_H

class Instrument_Panel;
class trans_table;

extern int cockpit_x,cockpit_y,y_clip;

class Cockpit
{
public:
  unsigned char *cbytes;
  int x_size,y_size;
  int trans_color;
  Instrument_Panel *ipanel;
  trans_table *t_table;
  int cockpit_y,cockpit_x;
  int y_clip;

  Cockpit(unsigned char *byts,
	  int xs,
	  int ys,
	  int tclr,
	  char *ipath);
  
  Cockpit(const char *tmap_id, const char *ipath);

  ~Cockpit();

  void draw(Flight &);

};


#endif
