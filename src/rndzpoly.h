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
#ifndef RNDZPOLY_H
#define RNDZPOLY_H

extern R2D_TYPE *zbuff;
extern R2D_TYPE ztrans;
extern int zbuff_size;
extern R2D_TYPE zx;

int rendzpoly(R_3DPoint *poly, int n, int color, 
	      Port_3D &port, R_2DPoint *spoints = NULL);

int project_zpoly(R_3DPoint *poly, int n, int color,
		 Port_3D &port, R_2DPoint *spoints = NULL);

void r_fill_convpoly(R_2DPoint *points, int n, int colr);
void zline(const R_3DPoint &p0, const R_3DPoint &p1, int colr, Port_3D &port);
void rendzline(const R_2DPoint &p0, const R_2DPoint &p1, int colr);
#endif
