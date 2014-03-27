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
/**********************************************
 * hud.h                                      *
 **********************************************/
#ifndef __hud_h
#define __hud_h

#include "starget.h"

class Hud
{
public:
  char hud_stuff[8];
  int hud_color;
  int hud_x;
  int hud_y;
  int vector_color;
  int look_at_color;
  int target_color;
  int gunsight_color1;
  int gunsight_color2;
  // where to show altitude, speed, & g_force
  int flt1_x,flt1_y;
  // where to show throttle setting & indicators
  int flt2_x,flt2_y;
  // where to show flap settings, rate of climb, heading
  int flt3_x,flt3_y;
  int flt4_x,flt4_y;
  // where to show battle info, i.e. damage & rounds remaining
  int gns_x,gns_y;
  // where to show target tracking info
  int trg_x,trg_y;
  SimFont *the_font;
  Hud(SimFont *fnt, char *path = NULL)
    : 
    hud_color(4),
    hud_x(5),
    hud_y(0),
    vector_color(90),
    the_font(fnt)
    {
      memset(hud_stuff,' ',sizeof(hud_stuff));
      hud_stuff[7] = '\0';
      if (path)
			read_file(path);
    }

  void show_selected_target(Flight &, Port_3D &, Pilot &);
  void show_vector(Flight &, Port_3D &);
  void show_controls(Flight &, Port_3D &, Pilot &);
  void show_gunsight(Pilot &, Port_3D &);
  void do_hud(Flight_Node &);
  void do_hud_stuff(int, int, Flight &);
  friend istream & operator >>(istream &, Hud &);
  void read_file(char *path);

  void do_hud_formation_geometry(const sTargetGeometry &, Port_3D &);
  void do_hud_target_geometry(const sTargetGeometry &, Port_3D &);
  void do_hud_target_flags(int, int, const sTargetFlags &);
  void show_target_guides(const R_3DPoint &targetLoc, Port_3D &port, int flag);

};

#endif
