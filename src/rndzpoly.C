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
 * File   : rndzpoly.C                           *
 * Date   : March, 1997                          *
 * Author : Dan Hammer                           *
 * Zbuffered drawing for flat colored polygons   *
 *************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <limits.h>
#include <values.h>
#include "defs.h"
#include "grafix.h"
#include "traveler.h"
#include "pen.h"
#include "vga_13.h"
#include "vmath.h"
#include "port_3d.h"
#include "cpoly.h"
#include "colorspc.h"
#include "clip.h"
#include "convpoly.h"
#include "simerr.h"
#include "simfile.h"
#include "txtrmap.h"
#include "rendpoly.h"
#include "rndzpoly.h"
#include "ddaline.h"
#include "brsnline.h"

#define LOW_VALUE R2D_TYPE(-INT_MAX)
#define HIGH_VALUE R2D_TYPE(INT_MAX)


extern TR_2DPoint *tr_l_edge;
extern TR_2DPoint *tr_r_edge;
extern int xmax,ymax;
extern R2D_TYPE min_y,max_y;
extern R2D_TYPE min_x,max_x;
extern R2D_TYPE min_z,max_z;
extern R2D_TYPE bnds_l, bnds_r;
extern R2D_TYPE bnds_t, bnds_b;
extern REAL_TYPE dz;

extern int rz_which_line;

/*************************************************************
 * z-buffered flat colored poly rendering                    *
 *************************************************************/

int rendzpoly(R_3DPoint *poly, int n, int color, 
	      Port_3D &port, R_2DPoint *spoints)
{
  R_3DPoint ppoly[RENDMAX];
  R_3DPoint cpoly[RENDMAX];
  R_3DPoint *polyptr;

  int zclipit = 0;
  int nvis = 0;
  int i,np;
  REAL_TYPE zmin = 1.0;

  for (i=0;i<n;i++)
    {
      port.world2port(poly[i],&ppoly[i]);
      if (ppoly[i].z < zmin)
	// This guy will need z-clipping
	zclipit = 1;
      else
	// I found at least one point in front of us
	nvis++;
    }
  // All points behind us
  if (nvis <= 0)
    return (0);
  // Needs z-clipping
  if (zclipit)
    {
      zpoly_clip(ppoly,n,cpoly,&np,zmin);
      if (np <= 0)
	return(0);
      polyptr = cpoly;
    }
  else
    {
      np = n;
      polyptr = ppoly;
    }
  return (project_zpoly(polyptr, np,color, port, spoints));
}

int project_zpoly(R_3DPoint *poly, int n, int color,
		 Port_3D &port, R_2DPoint *spoints)
{
  R_2DPoint scpoints[RENDMAX];
  R_2DPoint cpoints[RENDMAX];
  R_2DPoint *pnts;
  int i;
  int clip_n = 0;

  if (n > 0 && n < RENDMAX)
    {
      pnts = scpoints;

      min_x = R2D_TYPE(INT_MAX);
      max_x = R2D_TYPE(INT_MIN);
      min_y = R2D_TYPE(INT_MAX);
      max_y = R2D_TYPE(INT_MIN);
      min_z = R2D_TYPE(LONG_MAX);
      max_z = R2D_TYPE(-LONG_MAX);

      for (i = 0; i < n;i++)
	{
	  port.port2screen(poly[i],pnts);
	  if (pnts->x < min_x)
	    min_x = pnts->x;
	  if (pnts->x > max_x)
	    max_x = pnts->x;
	  if (pnts->y < min_y)
	    min_y = pnts->y;
	  if (pnts->y > max_y)
	    max_y = pnts->y;
	  if (pnts->z < min_z)
	    min_z = pnts->z;
	  if (pnts->z > max_z)
	    max_z = pnts->z;
	  pnts++;
	}

      bnds_l = R2D_TYPE(port.screen.left());
      bnds_r = R2D_TYPE(port.screen.right());
      bnds_t = R2D_TYPE(port.screen.top());
      bnds_b = R2D_TYPE(port.screen.bottom());

      if (max_x < bnds_l)
	return 0;
      if (min_x > bnds_r)
	return 0;
      if (max_y < bnds_t)
	return 0;
      if (min_y > bnds_b)
	return 0;

      dz = max_z - min_z;

      if (spoints != NULL)
	{
	  if (r_poly_clip(scpoints,spoints,n,&clip_n,
			  &port.screen))
	    {
	      clip_n++;
	      spoints[clip_n-1] = spoints[0];
	    }
	}
      else if (r_poly_clip(scpoints,cpoints,n,&clip_n,
			   &port.screen))
	{
	  clip_n++;
	  cpoints[clip_n-1] = cpoints[0];
	  r_fill_convpoly(cpoints,clip_n,color);
	}
    }
  return clip_n;
}

inline void r_set_edge(R2D_TYPE x, int y, R2D_TYPE z)
{
  if (y < 0 || y > ymax - 1)
    return;
  if (x < 0)
    x = 0;
  if (x > xmax)
    x = xmax;

  R2D_TYPE yt = R2D_TYPE(y);
  if (yt < min_y)
    min_y = yt;
  if (yt > max_y)
    max_y = yt;
  if (x < min_x)
    min_x = x;
  if (x > max_x)
    max_x = x;

  if (tr_l_edge[y].x > x)
    {
      tr_l_edge[y].x = x;
      tr_l_edge[y].z = z;
    }
  if (tr_r_edge[y].x < x)
    {
      tr_r_edge[y].x = x;
      tr_r_edge[y].z = z;
    }
}

void r_build_edge_array(R_2DPoint *points, int n)
{
  int i;
  R_2DPoint *p0,*p1;
  R2D_TYPE zstp,zz;

  min_x = R2D_TYPE(INT_MAX);
  max_x = R2D_TYPE(INT_MIN);
  min_y = R2D_TYPE(INT_MAX);
  max_y = R2D_TYPE(INT_MIN);

  p0 = points;
  for (i = 0; i < (n - 1); i++)
    {
      p1 = p0 + 1;
      // Get vertical z step
      zz = p0->z;
      if (rz_which_line)
	{
	  DDALine dline(p0->x,p0->y,p1->x,p1->y);
	  zstp = (p1->z - p0->z) / dline.STEPS();
	  do
	    {
	      r_set_edge(R2D_TYPE(dline.X()),dline.Y(),zz);
	      zz += zstp;
	    } while (dline.step());
	}
      else
	{
	  BresenhamLine dline(p0->x,p0->y,p1->x,p1->y);
	  zstp = (p1->z - p0->z) / dline.STEPS();
	  do
	    {
	      r_set_edge(R2D_TYPE(dline.X()),dline.Y(),zz);
	      zz += zstp;
	    } while (dline.step());
	}
      p0++;
    }
}

extern int frame_switch;
extern int frame_color;
extern void frame_convpoly(int *, int, int);

void r_frame_convpoly(R_2DPoint *points, int n)
{
  if (n >= RENDMAX)
    return;
  int ipoints[RENDMAX * 2];
  int *iptr = ipoints;
  int i;
  for (i=0;i<n;i++)
    {
      *iptr = (int) points[i].x;
      *(iptr + 1) = (int) points[i].y;
      iptr += 2;
    }
  frame_convpoly(ipoints,n,frame_color);
}

void r_fill_convpoly(R_2DPoint *points, int n, int colr)
{
  int j,stps;
  unsigned char *b_ptr;
  unsigned char *buffer_ptr;

  unsigned char *b_ptr2;
  R2D_TYPE *z_ptr;
  R2D_TYPE *z_ptr2;

  R2D_TYPE zstp;
  R2D_TYPE zz,zr;


  if (frame_switch)
    {
      r_frame_convpoly(points,n);
      return;
    }

  r_build_edge_array(points,n);
 
  buffer_ptr = lock_xbuff();
  b_ptr = buffer_ptr + (((int)min_y) * SCREEN_PITCH);
  
  z_ptr = zbuff + (((int)min_y) * SCREEN_WIDTH);

  for (j = (int)min_y; j <= (int)max_y; j++)
    {
      int xl = (int) tr_l_edge[j].x;
      stps = (int) ((tr_r_edge[j].x - xl) + 1) ;

      zz = tr_l_edge[j].z;
      zr = tr_r_edge[j].z;

      zstp = (zr - zz) / stps;

      b_ptr2 = b_ptr + xl;
      z_ptr2 = z_ptr + xl;
    
      while(stps--)
	{
	  if (zz + zbias + ztrans > *z_ptr2)
	    {
	      *b_ptr2 = (char) colr;
	      *z_ptr2 = zz + zbias + ztrans;
	    }
	  zz += zstp;
	  z_ptr2++;
	  b_ptr2++;
	}
      tr_r_edge[j].x = R2D_TYPE(LOW_VALUE);
      tr_l_edge[j].x = R2D_TYPE(HIGH_VALUE);
      b_ptr += SCREEN_PITCH;
      z_ptr += SCREEN_WIDTH;
    }
  free_xbuff();
}
                 
void zline(const R_3DPoint &w0, const R_3DPoint &w1, int colr, Port_3D &port)
{
  R_3DPoint p0,p1;
  R_2DPoint s0,s1;

  port.world2port(w0,&p0);
  port.world2port(w1,&p1);
  if (zline_clip(p0,p1,1.0))
    {
      port.port2screen(p0,&s0);
      port.port2screen(p1,&s1);
      if (ch_zclip(s0,s1,&port.screen))
	rendzline(s0,s1,colr);
    }
}

#define SETZBUFF {   zptr = zbuff + (traveler.here.y * SCREEN_WIDTH)\
  + traveler.here.x;\
   if (zz + zbias + ztrans > *zptr)\
     { putpixel(traveler.here.x,traveler.here.y,colr);\
	*zptr = zz + zbias + ztrans; }\
}

/* This should also use "ddaline.h", leaving it for now */
void rendzline(const R_2DPoint &s0, const R_2DPoint &s1, int colr)
{
  R2D_TYPE zstp,zz,d;
  R2D_TYPE *zptr;

  zz = s0.z;
  
  Point pp0((int)s0.x,(int)s0.y);
  Point pp1((int)s1.x,(int)s1.y);
  LineTraveler traveler(pp0,pp1,1);
  if (traveler.majAct)
    d = R2D_TYPE(traveler.there.x) - R2D_TYPE(traveler.here.x);
  else
    d = R2D_TYPE(traveler.there.y) - R2D_TYPE(traveler.here.y);
  
  if (d < 0)
    d = -d;
  d += 1;
#ifdef FLOAT_Z
  d += eps;
#endif
  zstp = (s1.z - s0.z) / d;
  
  SETZBUFF

    while (traveler.here != traveler.there)
      {
	if (traveler.majAct)
	  {
	    traveler.here.x += traveler.xdelta;
	  }
	else
	  traveler.here.y += traveler.ydelta;
	traveler.n += traveler.minorDelta;
	if (traveler.n >= traveler.majorDelta)
	  {
	    if (traveler.majAct)
	      traveler.here.y += traveler.ydelta;
	    else
	      traveler.here.x += traveler.xdelta;
	    traveler.n = traveler.n - traveler.majorDelta;
	  }
	zz = zz + zstp;
	SETZBUFF 
      }

  zz = zz + zstp;
  SETZBUFF 
  
}







