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
#ifndef __rndrpoly_h
#define __rndrpoly_h

#include "vga_13.h"
#include "txtrmap.h"

extern int TSTPS;

enum { persp, linear, zshade, solid };

int tr_rendpoly(R_3DPoint *poly, TxtPoint *txtr, unsigned int n, 
	     Port_3D &port,  int fillcolor = -1, 
	     TextrMap *tmap = NULL, TR_2DPoint *spoints = NULL);
int tr_project_poly(TR_3DPoint *poly,unsigned  int n,
		 Port_3D &port, int fillcolor = -1,
		 TextrMap *tmap = NULL, TR_2DPoint *spoints = NULL);
void tr_fill_convpoly(TR_2DPoint *points,unsigned  int n, 
		      TextrMap *tmap, int fillcolor = -1);
extern REAL_TYPE zbuff_min;
extern R2D_TYPE *zbuff;
extern R2D_TYPE ztrans;
extern int zbuff_size;
extern R2D_TYPE zbias;
extern int zbff_flag;
void clear_zbuff();
void tr_init();

inline int setzbuff(int x, int y, REAL_TYPE zval)
{
  if (zval < 1.0)
    zval = 1.0;
  R2D_TYPE *zptr = zbuff + (y * static_cast<int>(SCREEN_WIDTH)) + x;
  R2D_TYPE zinv = R2D_TYPE((1 / zval) * zfact);
  if (zinv + zbias + ztrans > *zptr)
    {
      *zptr = zinv + zbias + ztrans;
      return (1);
    }
  return (0);
}

#endif
