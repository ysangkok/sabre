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
#ifndef __rotate_h
#define __rotate_h
Point point_rotate(Point p, REAL_TYPE cx, REAL_TYPE cy,
		   REAL_TYPE sin_ang, REAL_TYPE cos_ang);
Point point_rotate(Point p, Point c, REAL_TYPE sin_ang,
		   REAL_TYPE cos_ang);
Point point_rotate(Point p, Point c, REAL_TYPE ang);
#endif
