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
/*************************************************************
 * pen.h                                                     *
 *************************************************************/
#ifndef __pen_h
#define __pen_h

#ifndef __grafix_h
#include "grafix.h"
#endif

struct pen
{
	Point	 position;
	int	 color;
	int	 width;
};

extern pen *curpen;

inline void setcolor(int color)
{
	curpen->color = color;
}

inline void moveto(int x, int y)
{
	curpen->position = Point(x,y);
}

void lineto(int x, int y );
void line(int x1, int y1, int x2, int y2, int color);


#endif