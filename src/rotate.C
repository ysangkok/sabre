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
/****************************************************************
 * rotate.cpp                                                   *
 * 2D Point Rotation                                            *
 ****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <values.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include "defs.h"
#include "grafix.h"
#include "vga_13.h"
#include "pen.h"
#include "convpoly.h"
#include "vmath.h"
#include "rotate.h"

Point point_rotate(Point p, REAL_TYPE cx, REAL_TYPE cy,
		   REAL_TYPE sin_ang, REAL_TYPE cos_ang)
{
  REAL_TYPE rx,ry,rx1,ry1;
  Point result;

  rx = REAL_TYPE(p.x) - cx;
  ry = REAL_TYPE(p.y) - cy;
  rx1 = (rx * cos_ang) + (ry * sin_ang) + cx;
  ry1 = (((-rx) * sin_ang) + (ry * cos_ang))  + cy;
  result.x = static_cast<int>(rx1);
  result.y = static_cast<int>(ry1);
  return result;
}

Point point_rotate(Point p, Point c, REAL_TYPE sin_ang, REAL_TYPE cos_ang)
{
  REAL_TYPE cx,cy;
  REAL_TYPE rx,ry,rx1,ry1;
  Point result;

  cx = REAL_TYPE(c.x);
  cy = REAL_TYPE(c.y);
  rx = REAL_TYPE(p.x) - cx;
  ry = REAL_TYPE(p.y) - cy;

  rx1 = (rx * cos_ang) + (ry * sin_ang) + cx;
  ry1 = (((-rx) * sin_ang) + (ry * cos_ang))  + cy;

  result.x = static_cast<int>(rx1);
  result.y = static_cast<int>(ry1);
  return result;
}

Point point_rotate(Point p, Point c, REAL_TYPE ang)
{
  return point_rotate(p,c,REAL_TYPE(sin(ang)),REAL_TYPE(cos(ang)));
}
