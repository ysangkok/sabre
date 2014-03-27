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
/***************************************************************
 * plltt.h                                                     *
 ***************************************************************/
#ifndef __plltt_h
#define __plltt_h

#include "vga_13.h"

class Palette_256
{
public:
  char *save_palette;
  char *tmp_palette;
  Palette_256();
  ~Palette_256();
} ;

class Palette_Effect : public Palette_256
{
public:
  float effect_time;
  float elapsed_time;
  int effect_on;
  void do_effect(void ( *)(Palette_Effect &), float = 0.1 );
  int is_done(float t, int reset );
  Palette_Effect()
    {
      effect_time = 0.1;
      elapsed_time = 0.0;
      effect_on = 0;
    }
} ;

void red_effect(Palette_Effect &);


#endif
