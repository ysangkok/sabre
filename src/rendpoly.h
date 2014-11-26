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
#ifndef __rendpoly_h
#define __rendpoly_h

#define RENDMAX 30
#include "grafix.h"
#include "clip.h"
#include "convpoly.h"
#include "port_3d.h"

int rendpoly(R_3DPoint *poly, int n, int color, Port_3D &port, int zclip = 1);
int project_poly(R_3DPoint *poly, int n, int color,
		 Port_3D &port);

inline void rendply(int *poly, int n, int color, Rect *bounds)
{
  int clipped_points[MAX_CLIP];
  int cn;
  int clip_n;
  if (poly_clip(poly,clipped_points,n,&clip_n,bounds))
    {
      cn = (clip_n + 1) * 2;
      clipped_points[cn - 1] = clipped_points[1];
      clipped_points[cn - 2] = clipped_points[0];
      fill_convpoly(clipped_points,clip_n + 1,color);
    }
}
#endif

