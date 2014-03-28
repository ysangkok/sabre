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
/*******************************************************
 * font8x8.h                                           *
 *******************************************************/
#ifndef __font8x8_h
#define __font8x8_h

#include "defs.h"

#define DEFAULT_FONT_FILE "font8x8.fnt"

#define INVERSE 0
#define NORMAL 1
#define MASK 2
#define ERASE 3
#define ERASE_INVERSE 4
class Rect;
class fontdev;

class SimFont
{
public:
  int width;
  int height;
  int option;
  int put_width;
  int put_height;

  SimFont()
    {
      option = NORMAL;
    }

  void set_option(int opt)
    {
      option = opt;
    }

  virtual void put_char(unsigned char c, int x, int y, int color,
			int dim = 0) = 0;

  void put_string(char *str, int x, int y, int color);
  void font_sprintf(int x, int y, int color, int opt, const char *format, ...);
};

class Font8x8 : public SimFont
{
public:
  char *title;
  int  font_dim;
  byte font[256][8];
  Font8x8(char *);
  virtual void put_char(unsigned char c, int x, int y, int color,
			int dim = 0);
  
};

class Font6x6 : public Font8x8
{
public:
  Font6x6(char *f)
    : Font8x8(f)
  {
    width = height = 8;
    put_width = put_height = 6;
  }

};

class ConsoleFont : public SimFont
{
public:
  fontdev *fdev;
  char *path;
  
  ConsoleFont(const char *);
  virtual ~ConsoleFont();
  void put_char(unsigned char c, int x, int y, int color,
		int dim = 0);
};

#endif
