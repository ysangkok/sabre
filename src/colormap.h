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
 * File   : colormap.h                           *
 * Date   : July, 1997                           *
 * Author : Dan Hammer                           *
 *************************************************/
#ifndef __colormap_h
#define __colormap_h

class ColorMap
{
public:
  int id_color;
  int base_color;
  int n;
  int mapped_color;

  ColorMap()
    {
      id_color = -1;
      base_color = 0;
      n = 0;
      mapped_color = 0;
    }

  ColorMap(int id_color, int base_color, int n)
    {
      this->id_color = id_color;
      this->base_color = base_color;
      this->n = n;
      mapped_color = base_color;
    }
  
  ColorMap(const ColorMap &cm)
    {
      copy(cm);
    }

  ColorMap & operator = (const ColorMap &cm)
    {
      copy(cm);
      return(*this);
    }

  void copy(const ColorMap &cm)
    {
      id_color = cm.id_color;
      base_color = cm.base_color;
      n = cm.n;
    }

  void read(istream &is);
  void write(ostream &os);
  
  friend istream &operator >>(istream &is, ColorMap &cm)
    {
      cm.read(is);
      return(is);
    }

  friend ostream &operator <<(ostream &os, ColorMap &cm)
    {
      cm.write(os);
      return(os);
    }

  int calcMappedColor(float perc);
  int calcMappedColor(int, int, int);
  
};

#endif
