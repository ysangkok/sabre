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
/**************************************************************
 * viewobj.h                                                  *
 **************************************************************/
#ifndef VIEWOBJ_H
#define VIEWOBJ_H


#define projectile_color 70
#include <stdlib.h>
#include "obj_3d.h"
#include "zview.h"

class FlightLight;
class Projectile;

class Projectile_View : public C_3DObject_Base
{
public:
  //	FlightLight *flt;
  Projectile *projectile;
  int color;
  float v_len;
  R_2DPoint s0,s1;
  Z_Viewer zview;

  virtual class_types class_type()
    { return Projectile_View_t; }
  virtual void draw(Port_3D &);
  virtual void translate(R_3DPoint &)
    {}
  virtual REAL_TYPE draw_prep(Port_3D &);
  Projectile_View(Projectile *p, int col = projectile_color)
    : projectile(p),
    color(col)
    { v_len = 0.5; }
};

class Bitmap_View : public C_3DObject_Base
{
public:
  int sc_x,sc_y;
  TextrMap *tmap;
  R_3DPoint loc;
  float world_len;
  float world_width;
  TR_2DPoint spoints[16];
  int npoints;

  virtual class_types class_type()
    { return Bitmap_View_t; }
  void draw(Port_3D &);
  void translate(R_3DPoint &)
    {}
  int buildScreenPoints(Port_3D &);
  virtual void setPortPoints(Port_3D &port, R_3DPoint *ppoly);
  virtual void setTexturePoints(TxtPoint *tp);
  virtual void port2world(Port_3D &port,
			  R_3DPoint *ppoly,
			  R_3DPoint *wpoly);
  REAL_TYPE draw_prep(Port_3D &);

  Bitmap_View(TextrMap *tmp, R_3DPoint *p,
	      float wl, float ww)
    : tmap(tmp),
    loc(*p),
    world_len(wl),
    world_width(ww)
    {}

  void setMap(TextrMap *tmp)
    {
      tmap = tmp;
    }
  void setPosition(const R_3DPoint &p)
    {
      loc = p;
    }
  const R_3DPoint &getPosition()
    {
      return (loc);
    }
};

class Skew_View : public Bitmap_View
{
public:
  float t,l,b,r;
  Skew_View(TextrMap *tmp, R_3DPoint *p,
	    float wl, float ww)
    :Bitmap_View(tmp,p,wl,ww)
    {
      t = l = b = r = 1.0;
    }
  virtual void setPortPoints(Port_3D &port, R_3DPoint *ppoly);
  void setSkew(float tt, float ll, float bb, float rr)
    {
      t = tt; l = ll; b = bb; r = rr;
    }
  void getSkew(float &tt, float &ll, float &rr, float &bb)
    {
      tt = t; ll = l; bb = b; rr = r;
    }
};

class Debris_View : public C_3DObject_Base
{
public:
  FlightLight *flt;
  int sc_x,sc_y;
  int color;
  C_Oriented_Shape debris_shape;
  R_3DPoint points[4];
  R_3DPoint points1[4];
  R_2DPoint spoints[16];
  int npoints;
  C_PolyInfo p_info[2];
  C_ShapeInfo info;

  virtual class_types class_type()
    { return Debris_View_t; }
  virtual void draw(Port_3D &);
  virtual void translate(R_3DPoint &)
    {}
  virtual REAL_TYPE draw_prep(Port_3D &);
  Debris_View(FlightLight *fl, float v_len, float v_width);
  virtual ~Debris_View();
};







#endif
