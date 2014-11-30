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
/**********************************************************
 * plltt.cpp                                              *
 **********************************************************/
#include <stdio.h>
#include <string.h>
#include "simerr.h"
#include "plltt.h"

Palette_256::Palette_256()
{
  save_palette = new char[PALETTE_SIZE];
  MYCHECK(save_palette != NULL);
  get_palette(0,256,(char *)save_palette);
  tmp_palette = NULL;
}

Palette_256::~Palette_256()
{
  if (save_palette != NULL)
    {
      set_palette(0,256,save_palette);
      delete [] save_palette;
    }

  if (tmp_palette != NULL)
    delete [] tmp_palette;
}

void Palette_Effect::do_effect(void (  *funct) (Palette_Effect &), 
			       float e_time)
{
  effect_time = e_time;
  elapsed_time = 0.0;
  if (tmp_palette != NULL)
    delete [] tmp_palette;
  tmp_palette = new char[PALETTE_SIZE];
  memcpy(tmp_palette,save_palette,PALETTE_SIZE);
  (*funct)(*this);
  set_palette(0,256,(char *)tmp_palette);
  delete [] tmp_palette;
  tmp_palette = NULL;
  effect_on = 1;
}

bool Palette_Effect::is_done(float t, int reset)
{
  bool result = 0;
  elapsed_time += t;
  if (elapsed_time >= effect_time)
    {
      result = 1;
      effect_on = 0;
    }
  if (result && reset)
    set_palette(0,256,(char *)save_palette);
  return result;
}

void red_effect(Palette_Effect &pe)
{
  int i;
  for (i=1;i<256;i++)
    pe.tmp_palette[i*3] = 63;
}
