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
#include "defs.h"
#include "grafix.h"
#include "traveler.h"
#include "vga_13.h"
#include "pen.h"

pen _pen;
pen *curpen = &_pen;

void line(int x1, int y1, int x2, int y2, int color)
{
	// Test for a horizontal line.
//	if (y1 == y2)
//		bar_fill(x1, y1, x2, y2, color);
//	else
 //	{
		LineTraveler traveler(Point(x1,y1), Point(x2,y2), 1);
		while (!traveler.Done())
		{
			putpixel(traveler.here.x, traveler.here.y, color);
			traveler.NextStep();
		}
//	}
}

void lineto(int x, int y)
{
	line(curpen->position.x, curpen->position.y, x, y, curpen->color);
	curpen->position.x = x;
	curpen->position.y = y;
}