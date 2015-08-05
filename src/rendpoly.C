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
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <limits.h>
#include <values.h>
#include "defs.h"
#include "vga_13.h"
#include "vmath.h"
#include "port_3d.h"
#include "cpoly.h"
#include "colorspc.h"
#include "clip.h"
#include "convpoly.h"
#include "simerr.h"
#include "simfile.h"
#include "rendpoly.h"
#include "port_3d.h"

/**********************************************************
 * Non z-buffered, flat color poly rendering             *
 **********************************************************/

extern REAL_TYPE world_scale;

int rend2(R_3DPoint *poly, int n, int color, Port_3D &port);

int rendpoly(R_3DPoint *poly, int n, int color,
	     Port_3D &port, bool wants_zclip)
{
  R_3DPoint ppoly[RENDMAX];
  R_3DPoint cpoly[RENDMAX];
  R_3DPoint *polyptr;
  bool zclipit = 0;
  bool skipit = 1;
  int i,np;
  REAL_TYPE z_min;

  z_min = port.z_min;
  //  	z_min = 0.00212;
  for (i=0;i<n;i++)
    {
      port.world2port(poly[i],&ppoly[i]);
      if (ppoly[i].z <= z_min)
	// This guy will need z-clipping
	zclipit = 1;
      else
	// I found at least one point in front of us
	skipit = 0;
    }
  // All points behind us
  if (skipit)
    return 0;
  // Needs z-clipping
  if (zclipit)
    {
      if (!wants_zclip)
	return 0;
      // First, just clip on the minimum z
      bounding_cube bc;
      bc.min_z = z_min;
      poly_clip_3D(ppoly,n,cpoly,&np,&bc,pz_min);
      polyptr = cpoly;
    }
  else
    {
      np = n;
      polyptr = ppoly;
    }
  return (project_poly(polyptr, np,color, port));
}

int rend2(R_3DPoint *poly, int n, int color, Port_3D &port)
{
  float spoints[RENDMAX];
  float cpoints[RENDMAX];
  int ipoints[RENDMAX];
  float *pnts;
  int clip_n = 0,cn = 0;

  pnts = spoints;
  for (int i = 0; i < n;i++)
    {
      port.port2screen(poly[i],pnts,pnts+1);
      pnts += 2;
    }
  if (f_poly_clip(spoints,cpoints,n,&clip_n,
		  &port.screen))
    {
      cn = (clip_n + 1) * 2;
      cpoints[cn - 1] = cpoints[1];
      cpoints[cn - 2] = cpoints[0];
      for (int i=0;i<cn;i++)
	ipoints[i] = static_cast<int>(cpoints[i]);
      fill_convpoly(ipoints,clip_n + 1,color);
    }
  return cn;
}

int project_poly(R_3DPoint *poly, int n, int color, Port_3D &port)
{
  int spoints[RENDMAX];
  int cpoints[RENDMAX];
  int *pnts,i;
  int clip_n = 0;

  if (n > 0 && n < RENDMAX)
    {
      pnts = spoints;
      int min_x = INT_MAX;
      int max_x = INT_MIN;
      int min_y = INT_MAX;
      int max_y = INT_MIN;

      for (i = 0; i < n;i++)
	{
	  port.port2screen(poly[i],pnts,pnts+1);

	  if (port.over_flow)
	    {
	      port.over_flow = 0;
	      return (rend2(poly,n,color,port));
	    }
	  if (*pnts < min_x)
	    min_x = *pnts;
	  if (*pnts > max_x)
	    max_x = *pnts;
	  if (*(pnts + 1) < min_y)
	    min_y = *(pnts + 1);
	  if (*(pnts + 1) > max_y)
	    max_y = *(pnts + 1);
	  pnts += 2;
	}
      if (max_x < port.screen.topLeft.x)
	return 0;
      if (min_x > port.screen.botRight.x)
	return 0;
      if (max_y < port.screen.topLeft.y)
	return 0;
      if (min_y > port.screen.botRight.y)
	return 0;
      int cn;
      if (poly_clip(spoints,cpoints,n,&clip_n,
		    &port.screen))
	{
	  cn = (clip_n + 1) * 2;
	  cpoints[cn - 1] = cpoints[1];
	  cpoints[cn - 2] = cpoints[0];
	  fill_convpoly(cpoints,clip_n + 1,color);
	}
    }
  return clip_n;
}
