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
 * File   : viewobj.C                            *
 * Date   : March, 1997                          *
 * Author : Dan Hammer                           *
 *************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>
#include <fstream.h>
#include <math.h>
#include "defs.h"
#include "vga_13.h"
#include "vmath.h"
#include "port_3d.h"
#include "cpoly.h"
#include "obj_3d.h"
#include "copoly.h"
#include "rndzpoly.h"
#include "flight.h"
#include "fltlite.h"
#include "colorspc.h"
#include "bits.h"
#include "moveable.h"
#include "unguided.h"
#include "sim.h"
#include "viewobj.h"

#define GRAY_COLORS 108

REAL_TYPE Projectile_View::draw_prep(Port_3D &port)
{
  R_3DPoint w1;
  R_3DPoint p0,p1;
  Z_Node_Manager *zm = NULL;
  
  drawn_flag = 0;
  visible_flag = 0;

  if (projectile->wspecs)
    zm = projectile->wspecs->getZNodeMgr();

  if (zm == NULL)
    {
      Vector v;
      v = projectile->flt->state.velocity.direction.to_vector();
      v *= v_len;
      w1 = projectile->position + R_3DPoint(v);
      
      port.world2port(projectile->position,&p0);
      port.world2port(w1,&p1);
      if (zline_clip(p0,p1,zbuff_min))
	{
	  port.port2screen(p0,&s0);
	  port.port2screen(p1,&s1);
	  if (ch_zclip(s0,s1,&port.screen))
	    visible_flag = 1;
	}
      if (visible_flag)
	z_order = p0.z;
    }
  else
    {
      zview.setManager(zm);
      zview.setReferencePort(&projectile->flt->state.flight_port);
      zview.draw_prep(port);
      z_order = zview.z_order;
      visible_flag = zview.visible_flag;
    }
  return z_order;
}

void Projectile_View::draw(Port_3D &port)
{
  if (projectile->wspecs && projectile->wspecs->getZNodeMgr())
    zview.draw(port);
  else
    rendzline(s0,s1,color);
  drawn_flag = 1;
}


REAL_TYPE Bitmap_View::draw_prep(Port_3D &port)
{
  drawn_flag = 0;
  visible_flag = 1;
  R_3DPoint port_local;
  port.world2port(loc,&port_local);
  z_order = port_local.z;
  REAL_TYPE zmin;
  if (zbuff && zbff_flag)
    zmin = 1.0;
  else
    zmin = port.get_z_min();
  if (z_order + world_len <= zmin)
    visible_flag = 0;
  else if (port_local.z >= 0)
    {
      port.port2screen(port_local,&sc_x,&sc_y);
      //      if (!port.over_flow && port.IsVisible(sc_x,sc_y))
	{
	  REAL_TYPE x2,y2;
	  R_3DPoint p2 = port_local;
	  p2.x += world_width;
	  p2.y += world_len;
	  port.port2screen(p2,&x2,&y2);
	  if (fabs( ((float)sc_x) - x2 ) < 1.0)
	    visible_flag = 0;
	  else
	    visible_flag = 1;
	}
	//      else
	//visible_flag = 0;
    }

  if (visible_flag)
    visible_flag = buildScreenPoints(port);
  return z_order;
}

int Bitmap_View::buildScreenPoints(Port_3D &port)
{
  R_3DPoint ppoly[4];
  R_3DPoint wpoly[4];
  TxtPoint  tp[4];
  setPortPoints(port,ppoly);
  setTexturePoints(tp);
  port2world(port,ppoly,wpoly);
  npoints = tr_rendpoly(wpoly,tp,4,port,-1,tmap,spoints);
  return(npoints);
}

void Bitmap_View::setPortPoints(Port_3D &port, R_3DPoint *ppoly)
{
  R_3DPoint p;
  REAL_TYPE wl = world_len / 2;
  REAL_TYPE ww = world_width / 2;
  port.world2port(loc,&p);
  ppoly[0] = R_3DPoint(p.x - ww, p.y + wl, p.z);
  ppoly[1] = R_3DPoint(p.x + ww, p.y + wl, p.z);
  ppoly[2] = R_3DPoint(p.x + ww, p.y - wl, p.z);
  ppoly[3] = R_3DPoint(p.x - ww, p.y - wl, p.z);
}

void Bitmap_View::setTexturePoints(TxtPoint *tp)
{
  tp[3] = TxtPoint(0,0,0);
  tp[2] = TxtPoint(tmap->map_w - 1,0,0);
  tp[1] = TxtPoint(tmap->map_w - 1,tmap->map_h - 1,0);
  tp[0] = TxtPoint(0,tmap->map_h - 1,0);
}

void Bitmap_View::port2world(Port_3D &port,
			     R_3DPoint *ppoly,
			     R_3DPoint *wpoly)
{
  port.port2world(ppoly[0],&wpoly[0]);
  port.port2world(ppoly[1],&wpoly[1]);
  port.port2world(ppoly[2],&wpoly[2]);
  port.port2world(ppoly[3],&wpoly[3]);
}

void Bitmap_View::draw(Port_3D &)
{
  if (npoints > 0)
    {
      int oldt = TSTPS;
      TSTPS = SCREEN_WIDTH;
      tr_fill_convpoly(spoints,npoints,tmap,-1);
      TSTPS = oldt;
    }
  drawn_flag = 1;
}

void Skew_View::setPortPoints(Port_3D &port, R_3DPoint *ppoly)
{
  R_3DPoint p;
  REAL_TYPE wl = world_len / 2;
  REAL_TYPE ww = world_width / 2;
  port.world2port(loc,&p);
  ppoly[0] = R_3DPoint(p.x - ww * l, p.y + wl * t, p.z);
  ppoly[1] = R_3DPoint(p.x + ww * r, p.y + wl * t, p.z);
  ppoly[2] = R_3DPoint(p.x + ww * r, p.y - wl * b, p.z);
  ppoly[3] = R_3DPoint(p.x - ww * l, p.y - wl * b, p.z);
}

REAL_TYPE Debris_View::draw_prep(Port_3D &port)
{
  drawn_flag = 0;
  R_3DPoint port_local;
  port.world2port(flt->state.flight_port.look_from,&port_local);
  z_order = port_local.z;
  if (z_order > port.get_z_max())
    visible_flag = 0;
  if (visible_flag)
    {
      debris_shape.set_world_points(flt->state.flight_port);

      C_Oriented_Poly *ply;
      debris_shape.set_visible(port);
      if (debris_shape.o_polys[0].visible)
	ply = &debris_shape.o_polys[0];
      else
	ply = &debris_shape.o_polys[1];
      npoints = ply->getScreenPoints(port,
				     flt->state.flight_port,
				     spoints);
      color = ply->color;
      visible_flag = (npoints > 0);
    }
  return z_order;
}


Debris_View::~Debris_View()
{

}

void Debris_View::draw(Port_3D &)
{
  r_fill_convpoly(spoints,npoints,color);
  drawn_flag = 1;
}

Debris_View::Debris_View(FlightLight *fl, float l, float w)
  : flt(fl)
{

  poly_params p_params[2];
  shape_params s_params(2,0,p_params);

  points[0] = R_3DPoint(w,0,l);
  points[1] = R_3DPoint(-w,0,l);
  points[2] = R_3DPoint(-w,0,-l);
  points[3] = R_3DPoint(w,0,-l);

  points1[0] = R_3DPoint(w,0,l);
  points1[1] = R_3DPoint(w,0,-l);
  points1[2] = R_3DPoint(-w,0,-l);
  points1[3] = R_3DPoint(-w,0,l);

  p_info[0] = C_PolyInfo(4,points);
  p_info[1] = C_PolyInfo(4,points1);
  info = C_ShapeInfo(2,p_info);

  p_params[0] = poly_params(RANDOM(13) + GRAY_COLORS,0,0,0);
  p_params[1] = poly_params(RANDOM(13) + GRAY_COLORS,0,0,0);
  if (debris_shape.create(&info))
    {
      debris_shape.set_params(&s_params);
      s_params.p_params = NULL;
    }
  else
    flags = -1;
}




