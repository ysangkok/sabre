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
 * colorspc.h
 **********************************************************/

#ifndef __colorspc_h
#define __colorspc_h

extern void set_rgb_value(int, char, char, char);
extern void get_rgb_value(int, char *, char *, char *);
class color_spec
{
public:

  int color;
  int red,green,blue;

  color_spec()
    {
      color = 0;
      red = green = blue = 0;
    }

  color_spec(int colr)
    {
      char r,g,b;
      color = colr;
      get_rgb_value(color,&r,&g,&b);
      red = r; green = g; blue = b;
    }

  friend istream &operator >>(istream &is, color_spec &cs);
  friend ostream &operator <<(ostream &os, color_spec &cs)
    {
      os << '(' << ' ' << cs.color << ' ' << cs.red << ' ';
      os << cs.green << ' ' << cs.blue << " )";
      return os;
    }
};



#endif

