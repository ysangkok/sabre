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
 * File   : colormap.C                           *
 * Date   : July,1997                            *
 * Author : Dan Hammer                           *
 *************************************************/
#include <stdio.h>
#include <values.h>
#include <math.h>
#include <iostream>
#include <string.h>
#include "simerr.h"
#include "simfile.h"
#include "colormap.h"

void ColorMap::read(std::istream &is)
{
  char c;
  READ_TOKI('(',is,c)
    is >> id_color >> base_color >> n;
  mapped_color = base_color;
  READ_TOK(')',is,c)
}

void ColorMap::write(std::ostream &os)
{
  os << "( " << id_color << ' ' << base_color << ' ' << n << " )";
}

int ColorMap::calcMappedColor(float r)
{
  if (r < 0)
    r = -r;
  if (r > 1.0)
    r = 1.0;
  mapped_color = base_color + (int) (r * n);
  return (mapped_color);
}

int ColorMap::calcMappedColor(int base, int range, int col)
{
  float r;

  r = ((float)(col - base)) / ((float)range);
  return (calcMappedColor(r));
}
