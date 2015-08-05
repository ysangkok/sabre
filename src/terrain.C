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
/*************************************************
 *           SABRE Fighter Plane Simulator              *
 * Version: 0.1                                  *
 * File   : terrain.C                            *
 * Date   : June, 1997                           *
 * Author : Dan Hammer                           *
 *************************************************/
#include <stdio.h>
#include <values.h>
#include <math.h>
#include <iostream>
#include <string.h>
#include "vmath.h"
#include "rendpoly.h"
#include "rndrpoly.h"
#include "rndzpoly.h"
#include "simerr.h"
#include "sim.h"
#include "simfile.h"
#include "terrain.h"
#include "group_3d.h"
#include "grndlevl.h"

typedef REAL_TYPE C;

extern Vector world_light_source;

bool Terrain_Shape::txtr_flag = 1;
int Terrain_Shape::color = 154;
int Terrain_Shape::color_range = 100;

//static int * const ttxtflg = & Terrain_Shape::txtr_flag;

#define NTPOLYS 2

void LineEquat::set(R_3DPoint p0, R_3DPoint p1, R_3DPoint p2)
{
  if (p1.x - p0.x == 0.0)
    {
      xslope_flag = 1;
      xx = p1.x;
      if (p2.x <= xx)
	oprtr = le;
      else
	oprtr = ge;
    }
  else
    {
      REAL_TYPE yy;
      xslope_flag = 0;
      m = (p1.y - p0.y ) / (p1.x - p0.x);
      b = -m * p1.x + p1.y;
      yy = m * p2.x + b;
      if (p2.y  <= yy)
	oprtr = le;
      else
	oprtr = ge;
   }
}

int LineEquat::bounded(R_3DPoint &p)
{
  if (xslope_flag)
    {
      if (oprtr == le)
	return (p.x <= xx);
      else
	return (p.x >= xx);
    }

  REAL_TYPE yy = m * p.x + b;
  if (oprtr == le)
    return(p.y <= yy);
  else
    return(p.y >= yy);
}


void TPoly::setPoints(const R_3DPoint &p0, const R_3DPoint &p1, const R_3DPoint &p2)
{
  P0() = p0;
  P1() = p1;
  P2() = p2;
  bcube.reset();
  bcube.set(P0());
  bcube.set(P1());
  bcube.set(P2());
  eqs[0].set(p0,p1,p2);
  eqs[1].set(p1,p2,p0);
  eqs[2].set(p2,p0,p1);
  computePlaneConstant();
  Vector lt = world_light_source;
  lt.Normalize();
  calcIntensity(lt);

  R_3DPoint p;

  p.x = C(p0.x + ((p1.x - p0.x) / 2.0));
  p.y = C(p0.y + ((p1.y - p0.y) / 2.0));
  p.z = C(p0.z + ((p1.z - p0.z) / 2.0));

  cp.x = C(p.x + ((p2.x - p.x) / 2.0));
  cp.y = C(p.y + ((p2.y - p.y) / 2.0));
  cp.z = C(p.z + ((p2.z - p.z) / 2.0));

}

void TPoly::computePlaneConstant()
{
  Vector side1,side2;
  side1 = P1() - P0();
  side2 = P2() - P1();
  surface_normal = side1.Cross(side2);
  surface_normal.Normalize();
  plane_constant = surface_normal.Dot(P0());
}  

int TPoly::calcVisible(Port_3D &port)
{
  Vector point = Vector(port.look_from);
  REAL_TYPE dt1 = point.Dot(surface_normal);
  if (dt1 - plane_constant >= 0)
    visible_flag = 1;
  else
    visible_flag = 0;
  return(visible_flag);
}

REAL_TYPE TPoly::calcIntensity(Vector &light)
{
  intensity = light.Dot(surface_normal) ;
//  intensity *= intensity;
  if (intensity > 1.0)
    intensity = 1.0;
  if (intensity < 0.0)
	  intensity = 0.0;
  return(intensity);
}

void TPoly::fitTexture(TextrMap *tmap, bounding_cube &cube)
{
REAL_TYPE xspan,yspan;
REAL_TYPE min_x,min_y;
REAL_TYPE dx,dy;
REAL_TYPE tw;
REAL_TYPE th;


  tw = C(tmap->map_w - 1.0);
  th = C(tmap->map_h - 1.0);
  min_x = cube.min_x;
  min_y = cube.min_y;
  xspan = cube.max_x - cube.min_x;
  yspan = cube.max_y - cube.min_y;
  
  for (int i=0;i<3;i++)
    {
      R_3DPoint &p = points[i];
      dx = C(fabs((p.x - min_x) / xspan));
      dy = C(fabs((p.y - min_y) / yspan));
      tpoints[i].u = tw * dx;
      tpoints[i].v = th * dy;
    }
}

REAL_TYPE TPoly::calcZ(R_3DPoint &p)
{
  REAL_TYPE result;
  ggtpoly = this;

  ggNormal = surface_normal;
  ggplane_constant = plane_constant;

  result = ((
	    surface_normal.X * (P0().x - p.x) +
	    surface_normal.Y * (P0().y - p.y)
	    ) 
	    / surface_normal.Z)
            + P0().z;
  return (result);
}

int TPoly::getPortPoints(Port_3D &port)
{
  np = 0;
  int zflg = 0;
  TR_3DPoint pp[3];

  port.world2port(P0(),T0(),&pp[0]);
  if (pp[0].r.z < zbuff_min)
    zflg++;
  port.world2port(P1(),T1(),&pp[1]);
  if (pp[1].r.z < zbuff_min)
    zflg++;
  port.world2port(P2(),T2(),&pp[2]);
  if (pp[2].r.z < zbuff_min)
    zflg++;
  if (zflg == 3)
      np = 0;
  else if (zflg == 0)
    {
      np = 3;
      port_points[0] = pp[0];
      port_points[1] = pp[1];
      port_points[2] = pp[2];
    }
  else
    zpoly_clip(pp,3,port_points,&np,zbuff_min);
  return (np);
}

int TPoly::getScreenPoints(Port_3D &port)
{
  int i;
  sp = 0;
  MYCHECK(np <= NTP_MAXPTS);
#ifdef USES_DDRAW
  for (i=0;i<np;i++)
    port.port2screen(port_points[i],&screen_points_ncf[i]);
	trf_poly_clip(screen_points_ncf,screen_pointsf,np,&sp,&port.screen);
#else
  for (i=0;i<np;i++)
    port.port2screen(port_points[i],&screen_points_nc[i]);
  if (tr_poly_clip(screen_points_nc,screen_points,np,&sp,&port.screen))
    {
      MYCHECK(sp <= NTP_MAXPTS - 1);
      sp++;
      screen_points[sp-1] = screen_points[0];
    }
#endif
  return (sp);
}

int TPoly::draw_prep(Port_3D &port)
{
  sp = np = 0;
  if (calcVisible(port))
    {
      visible_flag = 0;
      if (getPortPoints(port) > 0)
	if (getScreenPoints(port) > 0)
	  visible_flag = 1;
    }
  return (visible_flag);
}

void TPoly::draw(Port_3D &, TextrMap *tmap)
{
#ifdef USES_DDRAW
  int color;
  float d = 29.0 * intensity;
  d += 100.0;
  color = (int) d;
/*
  tr_fill_convpoly(screen_points,sp+1,tmap,color);
*/
  extern void wvgad3_rndrpolyf(TRF_2DPoint *points, int np, TextrMap *tmap,  int fillcolor);
//	wvgad3_rndrpolyf(screen_pointsf,sp,tmap,color);
	wvgad3_rndrpolyf(screen_points_ncf,np,tmap,color);
#else
  tr_fill_convpoly(screen_points,sp,tmap,-1);
#endif
}

void TPoly::draw(Port_3D &port, int base_color, int color_range)
{
  int color;
  float d = static_cast<float>(color_range) * intensity;
  d += static_cast<float>(base_color);
  color = static_cast<int>(d);

  rendzpoly(points,3,color,port);
 
}

extern int frame_switch;
extern int frame_color;
void TPoly::hilite(Port_3D & port, int color)
{
  int fs,fc;

  fs = frame_switch;
  fc = frame_color;

  frame_color = color;
  frame_switch = 1;
  draw(port,color,0);
  frame_switch = fs;
  frame_color = fc;
}

int TPoly::xybounds(R_3DPoint &p)
{
  return (
	  eqs[0].bounded(p) &&
	  eqs[1].bounded(p) &&
	  eqs[2].bounded(p)
	  );
}

void Terrain_Shape::setup(R_3DPoint & a_location,
			     REAL_TYPE a_width,
			     REAL_TYPE a_length,
			     REAL_TYPE *a_z_levels,
			     REAL_TYPE *,
			     TextrMap  *a_tmap
			  )
{
  int i;
  width  = a_width,
  length = a_length,
  tmap = a_tmap;
  location = a_location;
  for (i=0;i<8;i++)
    z_levels[i] = a_z_levels[i];
  for (i=0;i<4;i++)
    percents[i] = 0.0;
  //    percents[i] = a_percents[i];
  bounding_sphere = 0.0;
  createPolys();
  calc_bounding_sphere();
  location.z = C(bcube.min_z + ((bcube.max_z - bcube.min_z) / 2.0));
  fitTextures();
  //  dab_min *= dab_min;
}

inline REAL_TYPE MID(REAL_TYPE min, REAL_TYPE max)
{
  return C(min + ((max - min) / 2.0));
}

void Terrain_Shape::createPolys()
{
  R_3DPoint qbase[8];
  R_3DPoint pbase[4];
  REAL_TYPE w2 = C(width / 2.0);
  REAL_TYPE l2 = C(length / 2.0);

  qbase[q1] = R_3DPoint(location.x-w2,location.y+l2,z_levels[0]);
  qbase[q3] = R_3DPoint(location.x+w2,location.y+l2,z_levels[1]);
  qbase[q2] = R_3DPoint(C(qbase[q1].x + ((qbase[q3].x - qbase[q1].x) / 2.0)),
			location.y+l2,
			MID(z_levels[0],z_levels[1]));

  qbase[q5] = R_3DPoint(location.x+w2,location.y-l2,z_levels[2]);
  qbase[q4] = R_3DPoint(location.x+w2,
			C(qbase[q5].y + ((qbase[q3].y - qbase[q5].y) / 2.0)),
			MID(z_levels[1],z_levels[2]));

  qbase[q7] = R_3DPoint(C(location.x-w2),location.y-l2,z_levels[3]);
  qbase[q6] = R_3DPoint(C(qbase[q7].x + ((qbase[q5].x - qbase[q7].x) / 2.0)),
			location.y-l2,
			MID(z_levels[2],z_levels[3]));
  qbase[q8] = R_3DPoint(location.x-w2,
			qbase[q1].y + ((qbase[q7].y - qbase[q1].y) / 2),
			MID(z_levels[3],z_levels[0]));


  pbase[p1] = R_3DPoint(C(location.x-(w2 / 2.0)),
			C(location.y+(l2 / 2.0)),
			z_levels[4]);

  pbase[p2] = R_3DPoint(C(location.x+(w2 / 2.0)),
			C(location.y+(l2 / 2.0)),
			z_levels[5]);

  pbase[p3] = R_3DPoint(C(location.x+(w2 / 2.0)),
			C(location.y-(l2 / 2.0)),
			z_levels[6]);

  pbase[p4] = R_3DPoint(C(location.x-(w2 / 2.0)),
			C(location.y-(l2 / 2.0)),
			z_levels[7]);
  bcube.reset();


  setPoints(0,qbase[q1],qbase[q7],qbase[q3]);
  setPoints(1,qbase[q3],qbase[q7],qbase[q5]);
  
  points[0] = qbase[q1];
  points[1] = qbase[q7];
  points[2] = qbase[q3];
  points[3] = qbase[q5];

  /*
  pd0[0] = 0;
  pd0[1] = 1;
  pd0[2] = 2;

  pd1[0] = 2;
  pd1[1] = 1;
  pd1[2] = 3;
  */

  if (z_levels[0] == 0.0 && z_levels[1] == 0.0 && 
      z_levels[2] == 0.0 && z_levels[3] == 0.0)
    zflg = 1;
  else
    zflg = 0;
}

void Terrain_Shape::fitTextures()
{
  for (int i=0;i<NTPOLYS;i++)
    polys[i].fitTexture(tmap,bcube);
}

void Terrain_Shape::setPoints(int i, R_3DPoint &p1, 
			      R_3DPoint &p2, R_3DPoint &p3)
{
  polys[i].setPoints(p1,p2,p3);
  bcube.set(polys[i].bcube);
}


int Terrain_Shape::getGroundLevel(R_3DPoint &p)
{
  int result = 0;
  if (XYBOUNDS(bcube,p))
    {
      for (int i=0;i<NTPOLYS;i++)
	{
	  if (XYBOUNDS(polys[i].bcube,p) &&
	      polys[i].xybounds(p))
	    {
	      p.z = polys[i].calcZ(p);
	      result = 1;
	      break;
	    }
	}
    }
  return (result);
}

void Terrain_Shape::build_bounding_cube()
{

}

REAL_TYPE Terrain_Shape::draw_prep(Port_3D &port)
{
  C_3DObject_Base::draw_prep(port);
  if (visible_flag)
    {
      polys[0].draw_prep(port);
      polys[1].draw_prep(port);
      visible_flag =
	polys[0].visible_flag ||
	polys[1].visible_flag;
    }
  return (z_order);
}

inline void tPolyDraw(Port_3D &port, TPoly &tp, float dab_min, 
		 TextrMap *tmap, REAL_TYPE bsphere)
{
  R_3DPoint p;
  port.world2port(tp.cp,&p);

  if (!Terrain_Shape::txtr_flag)
    {
      tp.draw(port,Terrain_Shape::color,Terrain_Shape::color_range);
      return;
    }
  if (p.z > dab_min + bsphere)
  {
//	sim_printf("TSTPS = 1\n");
    TSTPS = 1;
  }
  else
    TSTPS = 0;

  tp.draw(port,tmap);
} 

void Terrain_Shape::draw(Port_3D &port)
{
  R2D_TYPE zb;
  zb = zbias; 
  int old_t;
  if (zflg)
    zbias = R2D_TYPE(0L);
  old_t = TSTPS;
  if (polys[0].visible_flag)
    tPolyDraw(port,polys[0],dab_min,tmap,bounding_sphere);
  if (polys[1].visible_flag)
    tPolyDraw(port,polys[1],dab_min,tmap,bounding_sphere);
  zbias = zb;
  TSTPS = old_t;
  drawn_flag = 1;
}




