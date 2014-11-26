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
#include <stdio.h>
#include <iostream>
#include <fstream>
#include "colorspc.h"

std::istream &operator >>(std::istream &is, color_spec &cs)
{
  char c;
  is >> c;
  while (is && c != '(')
    is >> c;
  is >> cs.color >> cs.red >> cs.green >> cs.blue >> c;
  while (is && c!=')')
    is >> c;
  set_rgb_value(cs.color, static_cast<char>(cs.red), static_cast<char>(cs.green), static_cast<char>(cs.blue));
  return is;
}
