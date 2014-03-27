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
 * File   : terrain.h                            *
 * Date   : June, 1997                           *
 * Author : Dan Hammer                           *
  *************************************************/
#ifndef __terrain_h
#define __terrain_h

#include "cpoly.h"
#include "obj_3d.h"
#include "txtrmap.h"

#define NTP_POINTS 3
#define NTP_MAXPTS 12
#define NT_POLYS 2

class LineEquat
{
public:
  enum { le, ge };
  int       xslope_flag;
  REAL_TYPE m;
  REAL_TYPE b;
  REAL_TYPE xx;
  int       oprtr;
  LineEquat()
    {
      m = 0.0;
      b = 0.0;
      xx = 0.0;
    }
  void set(R_3DPoint p0, R_3DPoint p1, R_3DPoint p2);
  int  bounded(R_3DPoint &);
};


class TPoly
{
public:
  R_3DPoint     points[NTP_POINTS];
  TxtPoint      tpoints[NTP_POINTS];
  TR_3DPoint    port_points[NTP_MAXPTS];
  int           np;
  TR_2DPoint	 screen_points_nc[NTP_MAXPTS];
  TR_2DPoint    screen_points[NTP_MAXPTS];
#ifdef USES_DDRAW
  TRF_2DPoint	 screen_points_ncf[NTP_MAXPTS];
  TRF_2DPoint   screen_pointsf[NTP_MAXPTS];
#endif
  int           sp;
  LineEquat     eqs[NTP_POINTS];
  REAL_TYPE     plane_constant;
  Vector        surface_normal;
  REAL_TYPE     intensity;
  bounding_cube bcube;
  int           visible_flag;
  R_3DPoint     cp;

  TPoly()
    :plane_constant(0.0),
     intensity(1.0),
     visible_flag(0)
    {}

  TPoly(const R_3DPoint &a_p0, const R_3DPoint &a_p1, 
	const R_3DPoint &a_p2)
    {
      setPoints(a_p0,a_p1,a_p2);
    }

  void          setPoints(const R_3DPoint &, const R_3DPoint &, 
			  const R_3DPoint &);
  void          computePlaneConstant();
  void          fitTexture(TextrMap *, bounding_cube &bcube);
  int           calcVisible(Port_3D &);
  REAL_TYPE     calcIntensity(Vector &light);
  REAL_TYPE     calcZ(R_3DPoint &);
  void          draw(Port_3D &port, TextrMap *tmap);
  void          draw(Port_3D &port, int base_color, int color_range);
  void          hilite(Port_3D &port, int color);
  int           draw_prep(Port_3D &port);
  int           getPortPoints(Port_3D &port);
  int           getScreenPoints(Port_3D &port);

  R_3DPoint &P0()
    {
      return(points[0]);
    }
  R_3DPoint &P1()
    {
      return(points[1]);
    }
  R_3DPoint &P2()
    {
      return(points[2]);
    }
  TxtPoint &T0()
    {
      return(tpoints[0]);
    }
  TxtPoint &T1()
    {
      return(tpoints[1]);
    }
  TxtPoint &T2()
    {
      return(tpoints[2]);
    }
  int xybounds(R_3DPoint &);
};

class Terrain_Shape : public C_3DObject_Base
{
public:
  TPoly     polys[NT_POLYS];
  REAL_TYPE z_levels[8];
  REAL_TYPE percents[8];
  REAL_TYPE width,length;
  TextrMap *tmap;
  REAL_TYPE dab_min;
  int      zflg;
  enum { q1,q2,q3,q4,q5,q6,q7,q8};
  enum { p1,p2,p3,p4 };
  static int txtr_flag; 
  static int color, color_range;

  R_3DPoint points[4];

  Terrain_Shape(REAL_TYPE a_dab_min = 100)
    :tmap(NULL),
     dab_min(a_dab_min)
  {}

  void setup(R_3DPoint & a_location,
	      REAL_TYPE a_width, REAL_TYPE a_length,
	      REAL_TYPE *a_z_levels,
	      REAL_TYPE *a_percents,
	      TextrMap *tmap);

  virtual class_types class_type()
  { return (C_3DPoly_t); }
  virtual void draw(Port_3D &);
  void build_bounding_cube();
  void setPoints(int i, R_3DPoint &p1, R_3DPoint &p2, 
		 R_3DPoint &p3);
  void createPolys();
  void fitTextures();
  virtual REAL_TYPE draw_prep(Port_3D &);
  int getGroundLevel(R_3DPoint &p);
};

inline int XYBOUNDS(bounding_cube &bcube, R_3DPoint &p)
{ 
  return (
	  (p.y >= bcube.min_y) && (p.y <= bcube.max_y) &&
	  (p.x >= bcube.min_x) && (p.x <= bcube.max_x)
	  ) ;
}


#endif
