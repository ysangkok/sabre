/*
    SABRE Fighter Plane Simulator 
    Copyright (c) 1997 Dan Hammer
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
#include "rndrpoly.h"
#include "ddaline.h"
#include "brsnline.h"

/***********************************************************
 * Textured poly with z-buffering                          *
 ***********************************************************/

#define LOW_VALUE R2D_TYPE(-INT_MAX)
#define HIGH_VALUE R2D_TYPE(INT_MAX)
#define YMAX SCREEN_HEIGHT
#define XMAX SCREEN_WIDTH  - 1

int xmax,ymax;

TR_2DPoint *tr_l_edge = NULL; // [YMAX];
TR_2DPoint *tr_r_edge= NULL; //[YMAX];

R2D_TYPE min_y,max_y;
R2D_TYPE min_x,max_x;
R2D_TYPE min_z,max_z;
R2D_TYPE bnds_l, bnds_r;
R2D_TYPE bnds_t, bnds_b;
REAL_TYPE dz;

R2D_TYPE *zbuff = NULL;
R2D_TYPE ztrans;
int zbuff_size;
R2D_TYPE zbias = R2D_TYPE(1L);
int zbff_flag = 0;
REAL_TYPE zbuff_min = 1.0;

int rz_which_line = 0;

int rz_steps = 0;

// #define TPREC 16
#define TPREC 20

int TSTPS  = 0;

void tr_init()
{
  int i;
  xmax = SCREEN_WIDTH - 1;
  ymax = SCREEN_HEIGHT;

  if (tr_r_edge != NULL)
    delete tr_r_edge;
  if (tr_l_edge != NULL)
    delete tr_l_edge;

  tr_r_edge = new TR_2DPoint[ymax];
  MYCHECK(tr_r_edge != NULL);

  tr_l_edge = new TR_2DPoint[ymax];
  MYCHECK(tr_l_edge != NULL);

  for (i=0;i<ymax;i++)
    {
      tr_l_edge[i].x = HIGH_VALUE;
      tr_l_edge[i].y = R2D_TYPE(i);

      tr_r_edge[i].x = LOW_VALUE;
      tr_r_edge[i].y = 0;
      tr_r_edge[i].z = 0;

      tr_l_edge[i].y = R2D_TYPE(i);
    }

  if (zbuff != NULL)
    free(zbuff);

  zbuff_size = SCREEN_WIDTH * SCREEN_HEIGHT * sizeof(R2D_TYPE);
  zbuff = (R2D_TYPE *) malloc(zbuff_size);
  if (zbuff == NULL)
    error_jump("Unable to allocate %d bytes for zbuffer\n",
	       zbuff_size);
  else
    sim_printf("%d bytes allocated for zbuffer\n",zbuff_size);
  ztrans = R2D_TYPE(1.0 * zfact);
  clear_zbuff();
  zbff_flag = 1;
  zbias = R2D_TYPE(1L << Z_PREC);
}

void clear_zbuff()
{
  if (zbuff)
    {
      memset(zbuff,'\0',zbuff_size);
      zbff_flag = 1;
    }
  ztrans = R2D_TYPE(0);
}

/*****************************************************************
 * z-buffered texture-mapped poly rendering                      *
 *****************************************************************/

int tr_rendpoly(R_3DPoint *poly, TxtPoint *txtr, int n, 
		Port_3D &port, int fillcolor, 
		TextrMap *tmap, TR_2DPoint *spoints)
{
  TR_3DPoint ppoly[RENDMAX];
  TR_3DPoint cpoly[RENDMAX];
  TR_3DPoint *polyptr;

  int zclipit = 0;
  int nvis = 0;
  int i,np;
  REAL_TYPE zmin = 1.0;
  for (i=0;i<n;i++)
    {
      port.world2port(poly[i],txtr[i],&ppoly[i]);
      if (ppoly[i].r.z < zmin)
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
  return (tr_project_poly(polyptr,np,port,fillcolor,tmap,spoints));
}
/***************************************************************
 * If spoints is not null, don't render the poly, just return  *
 * the converted points screen points if there are some        *
 ***************************************************************/
int tr_project_poly(TR_3DPoint *poly, int n,
		    Port_3D &port, int fillcolor, 
		    TextrMap *tmap, TR_2DPoint *spoints )
{
  TR_2DPoint scpoints[RENDMAX];
  TR_2DPoint cpoints[RENDMAX];
  TR_2DPoint *pnts;
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
	  if (tr_poly_clip(scpoints,spoints,n,&clip_n,
			   &port.screen))
	    {
	      clip_n++;
	      spoints[clip_n-1] = spoints[0];
	    }
	}
      else if (tr_poly_clip(scpoints,cpoints,n,&clip_n,
			    &port.screen))
	{
	  clip_n++;
	  cpoints[clip_n-1] = cpoints[0];
	  if (tmap != NULL)
	    tr_fill_convpoly(cpoints,clip_n,tmap,fillcolor);
	}
    }
  return clip_n;
}

inline void tr_set_edge(R2D_TYPE x, R2D_TYPE y, R2D_TYPE z, R2D_TYPE u, R2D_TYPE v)
{

  if (y < 0 || y > ymax - 1)
    return;

  if (x < 0)
    x = 0;
  if (x > xmax)
    x = xmax;

  if (y < min_y)
    min_y = y;
  if (y > max_y)
    max_y = y;
  if (x < min_x)
    min_x = x;
  if (x > max_x)
    max_x = x;

  if (tr_l_edge[y].x > x)
    {
      tr_l_edge[y].x = x;
      tr_l_edge[y].z = z;
      tr_l_edge[y].u = u;
      tr_l_edge[y].v = v;
    }
  if (tr_r_edge[y].x < x)
    {
      tr_r_edge[y].x = x;
      tr_r_edge[y].z = z;
      tr_r_edge[y].u = u;
      tr_r_edge[y].v = v;
    }
}

void rendzline(const R_2DPoint &, const R_2DPoint &, int);

void tr_build_edge_array(TR_2DPoint *points, int n)
{
  int i;
  TR_2DPoint *p0,*p1;
  R2D_TYPE zstp,zz,ustp,uu,vstp,vv;

  min_x = R2D_TYPE(INT_MAX);
  max_x = R2D_TYPE(INT_MIN);
  min_y = R2D_TYPE(INT_MAX);
  max_y = R2D_TYPE(INT_MIN);

  p0 = points;
  for (i = 0; i < (n - 1); i++)
    {
      p1 = p0 + 1;

      zz = p0->z;
      uu = p0->u;
      vv = p0->v;

      if ((p0->x==p1->x) && (p0->y==p1->y))
	{
	  tr_set_edge(p0->x,p0->y,zz,uu,vv);
	  p0++;
	  continue;
	}


      /* 
	 Can't decide upon which line algorythm is
	 better (Bresenham or DDA),
	 so let's waste a cpu cycle and make it
	 a per-frame option (')' key toggles). DDA
	 clearly gives a more accurate line, but 
	 Bresenham gives a smoother edge. Defaults
	 Bresenham 
	 */

      if (rz_which_line)
	{
	  DDALine dline(p0->x,p0->y,p1->x,p1->y);
	  zstp = (p1->z - p0->z) / dline.STEPS();
	  ustp = (p1->u - p0->u) / dline.STEPS();
	  vstp = (p1->v - p0->v) / dline.STEPS();
	  do
	    {
	      tr_set_edge(dline.X(),dline.Y(),zz,uu,vv);
	      zz = zz + zstp;
	      uu = uu + ustp;
	      vv = vv + vstp;
	    } while (dline.step());
	}
      else
	{
	  BresenhamLine dline(p0->x,p0->y,p1->x,p1->y);
	  zstp = (p1->z - p0->z) / dline.STEPS();
	  ustp = (p1->u - p0->u) / dline.STEPS();
	  vstp = (p1->v - p0->v) / dline.STEPS();
	  do
	    {
	      tr_set_edge(dline.X(),dline.Y(),zz,uu,vv);
	      zz = zz + zstp;
	      uu = uu + ustp;
	      vv = vv + vstp;
	    } while (dline.step());
	}
      p0++;
    }
}

extern int frame_switch;
extern void frame_convpoly(int *, int, int);

void tr_frame_convpoly(TR_2DPoint *points, int n)
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
  frame_convpoly(ipoints,n,10);
}

/**************************************************************
 * Texture mapping routines                                   *
 **************************************************************/
/* No perspective correction */
void tr_fill_convpoly_nc(TR_2DPoint *points, int n, TextrMap *tmap, 
			 int fillcolor);
/* <n> pixels perspective corrected */
void tr_fill_convpoly_sc(TR_2DPoint *points, int n, TextrMap *tmap, 
			 int fillcolor);
/* all pixels perspective corrected */
void tr_fill_convpoly_c(TR_2DPoint *points, int n, TextrMap *tmap, 
			int fillcolor);

static unsigned char *buffer_ptr;

void tr_fill_convpoly(TR_2DPoint *points, int n, TextrMap *tmap, 
		      int fillcolor)
{
#ifdef USES_DDRAW
  extern void wvgad3_rndrpoly(TR_2DPoint *points, int n, TextrMap *tmap, 
			      int fillcolor);
  wvgad3_rndrpoly(points,n-1,tmap,fillcolor);
  return;
#endif
  if (frame_switch)
    {
      tr_frame_convpoly(points,n);
      return;
    }
  buffer_ptr = lock_xbuff();
  if (TSTPS)
    tr_fill_convpoly_nc(points,n,tmap,fillcolor);
  else if (rz_steps > 1)
    tr_fill_convpoly_sc(points,n,tmap,fillcolor);
  else
    tr_fill_convpoly_c(points,n,tmap,fillcolor);
  free_xbuff();
}

/****************************************************************
 * All pixels perspective-corrected by dividing by z-value      *
 ****************************************************************/
void tr_fill_convpoly_c(TR_2DPoint *points, int n, TextrMap *tmap, 
			int fillcolor)
{
  int j,stps;
  int u,v;
  unsigned char *b_ptr;
  R2D_TYPE *z_ptr;
  R2D_TYPE *z_ptr2;
  R2D_TYPE zstp,ustp,vstp;
  R2D_TYPE zz,zr,uu,ur,vv,vr;
  R2D_TYPE zb,zt;
  int tw,th,tw_1,th_1;
  int trans_color;
  int tflag;
  int xl;
  unsigned char *tbytes;
  unsigned char clr;
  unsigned char *b_ptr2;


  tw = tmap->map_w;
  th = tmap->map_h;
  tw_1 = tw - 1;
  th_1 = th - 1;
  trans_color = tmap->trans_colr;

  tbytes = tmap->getBytes();
  if (tbytes == NULL)
    return;

  tflag = trans_color > -1;

  tr_build_edge_array(points,n);

  b_ptr = buffer_ptr + (((int)min_y) * SCREEN_PITCH);
  z_ptr = zbuff + (((int)min_y) * SCREEN_WIDTH);

  zb = zbias + ztrans;

  // Do for each row 
  for (j = (int) min_y; j <= (int)max_y; j++)
    {
      xl = (int) tr_l_edge[j].x;
      stps = (int) ((tr_r_edge[j].x - xl) + 1) ;
      zz = tr_l_edge[j].z;
      zr = tr_r_edge[j].z;
      uu = tr_l_edge[j].u;
      ur = tr_r_edge[j].u;
      vv = tr_l_edge[j].v;
      vr = tr_r_edge[j].v;
      zstp = (zr - zz) / stps;
      ustp = (ur - uu) / stps;
      vstp = (vr - vv) / stps;

      b_ptr2 = (unsigned char *) b_ptr + xl;
      z_ptr2 = z_ptr + xl;

      // Do for each column
      while (stps--)
	{
	  zt = zz + zb;
	  if (zt > *z_ptr2)
	    {
	      u = (int) (uu / zz );
	      v = (int) (vv / zz );
	      if (u > tw_1)
		u = tw_1;
	      else if (u < 0)
		u = 0;
	      if (v > th_1)
		v = th_1;
	      else if (v < 0)
		v = 0;
	      clr = (char) tmap->getColorMap(*(tbytes + v * tw + u));
	      if (tflag)
		{
		  if (clr != trans_color)
		    {
		      *b_ptr2 = clr;
		      *z_ptr2 = zt;
		    }
		  else if (fillcolor != -1)
		    {
		      *b_ptr2 = (char )fillcolor;
		      *z_ptr2 = zt;
		    }
		}
	      else
		{
		  *b_ptr2 = clr;
		  *z_ptr2 = zt;
		}
	    }
	  zz += zstp;
	  uu += ustp;
	  vv += vstp;
	  z_ptr2++;
	  b_ptr2++;
	}
      // reset row hi/lo 
      tr_r_edge[j].x = R2D_TYPE(LOW_VALUE);
      tr_l_edge[j].x = R2D_TYPE(HIGH_VALUE);
      // next row
      b_ptr += SCREEN_PITCH;
      z_ptr += SCREEN_WIDTH;
    }
}

/****************************************************************
 * correct <n> pixels for perspective, interpolate between      *
 ****************************************************************/
void tr_fill_convpoly_sc(TR_2DPoint *points, int n, TextrMap *tmap, 
			 int fillcolor)
{
  int j,stps,stpsi,stpsu;
  int u,v;
  unsigned char *b_ptr;

  R2D_TYPE *z_ptr;
  R2D_TYPE *z_ptr2;

  R2D_TYPE zstp=0,ustp=0,vstp=0;
  R2D_TYPE zz,zr,uu,ur,vv,vr;
  R2D_TYPE zb,zt;

  R2D_TYPE zstpi,ustpi,vstpi;
  R2D_TYPE zzi,zri,uui,uri,vvi,vri;

  int tw,th,tw_1,th_1;
  int trans_color;
  int tflag;
  int xl;
  unsigned char *tbytes;
  unsigned char clr;
  unsigned char *b_ptr2;

  tw = tmap->map_w;
  th = tmap->map_h;
  tw_1 = tw - 1;
  th_1 = th - 1;
  trans_color = tmap->trans_colr;

  tbytes = tmap->getBytes();
  if (tbytes == NULL)
    return;

  tflag = trans_color > -1;

  tr_build_edge_array(points,n);

  b_ptr = buffer_ptr + (((int)min_y) * SCREEN_PITCH);
  z_ptr = zbuff + (((int)min_y) * SCREEN_WIDTH);

  zb = zbias + ztrans;

  for (j = (int) min_y; j <= (int)max_y; j++)
    {
      xl = (int) tr_l_edge[j].x;
      stps = (int) ((tr_r_edge[j].x - xl) + 1) ;

      stpsu = (stps / rz_steps);

      zz = tr_l_edge[j].z;
      zr = tr_r_edge[j].z;
      uu = tr_l_edge[j].u;
      ur = tr_r_edge[j].u;
      vv = tr_l_edge[j].v;
      vr = tr_r_edge[j].v;

      if (stpsu > 0)
	{
	  zstp = (zr - zz) / stpsu;
	  ustp = (ur - uu) / stpsu;
	  vstp = (vr - vv) / stpsu;
	}

      b_ptr2 = (unsigned char *) b_ptr + xl;
      z_ptr2 = z_ptr + xl;

      while (stpsu--)
	{
	  zzi = zz;
	  zz  += zstp;
	  zri = zz;

	  uui = (uu / zzi) << TPREC;
	  uu  += ustp;
	  uri = (uu / zri) << TPREC;
	  vvi = (vv / zzi) << TPREC;
	  vv  += vstp;
	  vri = (vv / zri) << TPREC;

	  stpsi = rz_steps;
	  zstpi = (zri - zzi) / stpsi;
	  ustpi = (uri - uui) / stpsi;
	  vstpi = (vri - vvi) / stpsi;

	  while (stpsi--)
	    {
	      zt = zzi + zb;
	      if (zt > *z_ptr2)
		{
		  u = uui >> TPREC;
		  v = vvi >> TPREC;
		  if (u > tw_1)
		    u = tw_1;
		  else if (u < 0)
		    u = 0;
		  if (v > th_1)
		    v = th_1;
		  else if (v < 0)
		    v = 0;

		  clr = (char) tmap->getColorMap(*(tbytes + v * tw + u));
		  if (tflag)
		    {
		      if (clr != trans_color)
			{
			  *b_ptr2 = clr;
			  *z_ptr2 = zt;
			}
		      else if (fillcolor != -1)
			{
			  *b_ptr2 = (char )fillcolor;
			  *z_ptr2 = zt;
			}
		    }
		  else
		    {
		      *b_ptr2 = clr;
		      *z_ptr2 = zt;
		    }
		}
	      z_ptr2++;
	      b_ptr2++;
	      zzi += zstpi;
	      uui += ustpi;
	      vvi += vstpi;
	      stps--;
	    }
	}

      if (stps)
	{
	  zr = tr_r_edge[j].z;
	  ur = tr_r_edge[j].u;
	  vr = tr_r_edge[j].v;

	  uu = (uu / zz) << TPREC;
	  vv = (vv / zz) << TPREC;
	  ur = (ur / zr) << TPREC;
	  vr = (vr / zr) << TPREC;

	  zstp = (zr - zz) / stps;
	  ustp = (ur - uu) / stps;
	  vstp = (vr - vv) / stps;

	  while (stps--)
	    {
	      zt = zz + zb;
	      if (zt > *z_ptr2)
		{
		  u = uu >> TPREC;
		  v = vv >> TPREC;
		  if (u > tw_1)
		    u = tw_1;
		  else if (u < 0)
		    u = 0;
		  if (v > th_1)
		    v = th_1;
		  else if (v < 0)
		    v = 0;
		  clr = (char) tmap->getColorMap(*(tbytes + v * tw + u));
		  if (tflag)
		    {
		      if (clr != trans_color)
			{
			  *b_ptr2 = clr;
			  *z_ptr2 = zt;
			}
		      else if (fillcolor != -1)
			{
			  *b_ptr2 = (char )fillcolor;
			  *z_ptr2 = zt;
			}
		    }
		  else
		    {
		      *b_ptr2 = clr;
		      *z_ptr2 = zt;
		    }
		}
	      zz += zstp;
	      uu += ustp;
	      vv += vstp;
	      z_ptr2++;
	      b_ptr2++;
	    }
	}

      tr_r_edge[j].x = R2D_TYPE(LOW_VALUE);
      tr_l_edge[j].x = R2D_TYPE(HIGH_VALUE);
      b_ptr += SCREEN_PITCH;
      z_ptr += SCREEN_WIDTH;
    }
}

/****************************************************************
 * no perspective correction                                    *
 ****************************************************************/
void tr_fill_convpoly_nc(TR_2DPoint *points, int n, TextrMap *tmap,
			 int fillcolor)
{
  int j,stps;
  int u,v;
  unsigned char *b_ptr;

  R2D_TYPE *z_ptr;
  R2D_TYPE *z_ptr2;

  R2D_TYPE zstp,ustp,vstp;
  R2D_TYPE zz,zr,uu,ur,vv,vr;
  R2D_TYPE zb,zt;

  int tw,th,tw_1,th_1;
  int trans_color;
  int tflag;
  int xl;
  unsigned char *tbytes;
  unsigned char clr;
  unsigned char *b_ptr2;

  tw = tmap->map_w;
  th = tmap->map_h;
  tw_1 = tw - 1;
  th_1 = th - 1;
  trans_color = tmap->trans_colr;

  tbytes = tmap->getBytes();
  if (tbytes == NULL)
    return;

  tflag = trans_color > -1;

  tr_build_edge_array(points,n);

  b_ptr = buffer_ptr + (((int)min_y) * SCREEN_PITCH);
  z_ptr = zbuff + (((int)min_y) * SCREEN_WIDTH);

  zb = zbias + ztrans;

  for (j = (int) min_y; j <= (int)max_y; j++)
    {
      xl = (int) tr_l_edge[j].x;
      stps = (int) ((tr_r_edge[j].x - xl) + 1) ;
      zz = tr_l_edge[j].z;
      zr = tr_r_edge[j].z;
      uu = tr_l_edge[j].u;
      ur = tr_r_edge[j].u;
      vv = tr_l_edge[j].v;
      vr = tr_r_edge[j].v;

      uu = (uu / zz) << TPREC;
      vv = (vv / zz) << TPREC;
      ur = (ur / zr) << TPREC;
      vr = (vr / zr) << TPREC;

      zstp = (zr - zz) / stps;
      ustp = (ur - uu) / stps;
      vstp = (vr - vv) / stps;

      b_ptr2 = (unsigned char *) b_ptr + xl;
      z_ptr2 = z_ptr + xl;

      while (stps--)
	{
	  zt = zz + zb;
	  if (zt > *z_ptr2)
	    {
	      u = uu >> TPREC;
	      v = vv >> TPREC;
	      if (u > tw_1)
		u = tw_1;
	      else if (u < 0)
		u = 0;
	      if (v > th_1)
		v = th_1;
	      else if (v < 0)
		v = 0;

	      clr = (char) tmap->getColorMap(*(tbytes + (v * tw) + u));
	      if (tflag)
		{
		  if (clr != trans_color)
		    {
		      *b_ptr2 = clr;
		      *z_ptr2 = zt;
		    }
		  else if (fillcolor != -1)
		    {
		      *b_ptr2 = (char )fillcolor;
		      *z_ptr2 = zt;
		    }
		}
	      else
		{
		  *b_ptr2 = clr;
		  *z_ptr2 = zt;
		}
	    }
	  zz += zstp;
	  uu += ustp;
	  vv += vstp;
	  z_ptr2++;
	  b_ptr2++;
	}
      tr_r_edge[j].x = R2D_TYPE(LOW_VALUE);
      tr_l_edge[j].x = R2D_TYPE(HIGH_VALUE);
      b_ptr += SCREEN_PITCH;
      z_ptr += SCREEN_WIDTH;
    }
}

