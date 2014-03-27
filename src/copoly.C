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
 * File   : copoly.C                             *
 * Date   : March, 1997                          *
 * Author : Dan Hammer                           *
 *************************************************/
#include <stdio.h>
#include <values.h>
#include <math.h>
#include <iostream.h>
#include "simerr.h"
#include "vga_13.h"
#include "pen.h"
#include "convpoly.h"
#include "vmath.h"
#include "clip.h"
#include "cpoly.h"
#include "copoly.h"

extern int frame_switch;

/***************************************************
 * An 'oriented' poly class. These are polys and   *
 * shapes which can be rotated and translated.     *
 * They make use of a "reference port" which prov- *
 * ides the rotation and translation matrix        *
 ***************************************************/

REAL_TYPE C_Oriented_Poly::co_scaler = REAL_TYPE(1.0);

C_Oriented_Poly::C_Oriented_Poly()
{

}

int C_Oriented_Poly::create(C_PolyInfo *pi)
{
  //	return (C_Poly::create(pi));
  p_info = pi;
  color = 0;
  wpoints = NULL;
  return (1);
}

C_Oriented_Poly::~C_Oriented_Poly()
{

}

/***************************************************
 * create the vertices by using a "reference port" *
 * to obtain the rotations and translations        *
 ***************************************************/
void C_Oriented_Poly::create_points(Port_3D &ref_port)
{
  int npoints = p_info->npoints;
  if (wpoints != NULL)
    delete [] wpoints;
  wpoints = new R_3DPoint[npoints];
  MYCHECK(wpoints != NULL);
  for (int i =0;i<npoints;i++)
    {
      R_3DPoint p = p_info->lpoints[i];
      p *= co_scaler;
      ref_port.port2world(p,&wpoints[i]);
    }
}

void C_Oriented_Poly::render_shadow(Port_3D &theport, int z_clip,
				    REAL_TYPE z_value,
				    Vector &light_source,
				    int shadow_color,
				    Port_3D &ref_port)
{
  if (!(flags & SHADOW))
    return;
  if (wpoints == NULL)
    create_points(ref_port);
  C_Poly::render_shadow(theport,z_clip,z_value,light_source,shadow_color);
}

void C_Oriented_Poly::draw(Port_3D &port, Port_3D &ref_port, int zclip)
{
  if (wpoints == NULL)
    create_points(ref_port);
  C_Poly::draw(port,zclip);
  if (wpoints != NULL)
    {
      delete [] wpoints;
      wpoints = NULL;
    }
}

int C_Oriented_Poly::getScreenPoints(Port_3D &port, Port_3D &ref_port, 
				     R_2DPoint *spoints)
{
  int result = 0;
  if (wpoints == NULL)
    create_points(ref_port);
  result = C_Poly::getScreenPoints(port,spoints);
  if (wpoints != NULL)
    {
      delete [] wpoints;
      wpoints = NULL;
    }
  return (result);
}

int C_Oriented_Poly::getScreenPoints(Port_3D &port, Port_3D &ref_port, 
				     TR_2DPoint *spoints)
{
  int result = 0;
  if (wpoints == NULL)
    create_points(ref_port);
  result = C_Poly::getScreenPoints(port,spoints);
  if (wpoints != NULL)
    {
      delete [] wpoints;
      wpoints = NULL;
    }
  return (result);
}

// A misnamed function -- this really sets the surface
// normal and the plane constant for the given
// reference port
void C_Oriented_Poly::set_world_points(Port_3D &ref_port)
{
  visible = 1;
  R_3DPoint w0,w1,w2;
  R_3DPoint p0,p1,p2;
  p0 = p_info->lpoints[0];
  p1 = p_info->lpoints[1];
  p2 = p_info->lpoints[2];
  p0 *= co_scaler;
  p1 *= co_scaler;
  p2 *= co_scaler;
  ref_port.port2world(p0,&w0);
  ref_port.port2world(p1,&w1);
  ref_port.port2world(p2,&w2);

  Vector side1,side2;
  side1 = Vector(R_3DPoint(w1 - w0));
  side2 = Vector(R_3DPoint(w2 - w1));
  surface_normal = side1.Cross(side2);
  surface_normal.Normalize();
  plane_constant = -surface_normal.Dot(Vector(w0));
}

void C_Oriented_Poly::set_poly_icolor(Vector &light)
{
  if (visible && (flags & I_SHADE))
    {
      float intensity = O_Intensity(light,surface_normal);
      if (intensity > 1.0 )
	intensity = 1.0;
      {
	int col = base_color + (int) (intensity * ((float) color_range) );
	/*
	if (col <= base_color + color_range &&
	    col >= base_color)
	    */
	  color = col;
      }
    }
}

void C_Oriented_Poly::write(ostream &os)
{
  int tflag = 0;
  char cc = '0';

  if (wpoints == NULL)
    {
      os << "[ 0 0 ]";
      return;
    }
  if (p_info->tpoints != NULL)
    {
      tflag = 1;
      cc = '1';
    }
  os << '[' << ' ' << p_info->npoints  << ' ' << cc << ' ';
  for (int i=p_info->npoints-1;i>=0;i--)
    {
      if (!tflag)
	{
	  R_3DPoint &p = wpoints[i];
	  os << p << ' ';
	}
      else
	{
	  R_3DPoint &p = wpoints[i];
	  TxtPoint &tp = p_info->tpoints[i];
	  os << '<' << p.x << ' ' << p.y << ' ' << p.z << ' ' << tp.u << ' ' << tp.v << '>';
	}
    }
  os << ']';
}

ostream & operator <<(ostream &os, C_Oriented_Poly &cp)
{
  cp.write(os);
  return os;
}

ostream &operator <<(ostream &os, C_Oriented_Shape &cs)
{
  os << '{' << '\n';
  os << cs.npolys << '\n';
  for (int i=0;i<cs.npolys;i++)
    os << cs.o_polys[i] << '\n';
  os << '}' << '\n';
  return os;
}


C_Oriented_Shape::C_Oriented_Shape()
  :o_polys(NULL),
   ref_port(NULL)
{
  visible = 1;
}


int C_Oriented_Shape::create(C_ShapeInfo *ptr)
{
  int result = 1;
  info_ptr = ptr;
  npolys = info_ptr->npolys;
  o_polys = new C_Oriented_Poly[npolys];
  if (o_polys != NULL)
    {
      for (int i=0;i<npolys;i++)
	{
	  if (!o_polys[i].create(&info_ptr->polyinfos[i]))
	    {
	      result = 0;
	      npolys = 0;
	      delete [] o_polys;
	      o_polys = NULL;
	      break;
	    }
	}
    }
  else
    {
      result = 0;
      npolys = 0;
    }
  return (result);
}

C_Oriented_Shape::~C_Oriented_Shape()
{
  if (o_polys)
    delete [] o_polys;
}

istream &operator >>(istream &is, C_Oriented_Shape &cs)
{
  for (int i=0;i<cs.npolys;i++)
    is >> cs.o_polys[i];
  return is;
}

void C_Oriented_Shape::set_params(shape_params *prms)
{
  flags = prms->flags;
  for (int i=0;i<npolys;i++)
    o_polys[i].set_params(&prms->p_params[i]);
}

void C_Oriented_Shape::set_world_points(Port_3D &rf_port)
{
  ref_port = &rf_port;
  maxlen = 0;
  int i;
  visible = 1;
  for (i = 0; i <npolys;i++)
    {
      /*
      C_Oriented_Poly &co = o_polys[i];
      co.visible = 1;
      co.set_world_points(port);
      R_3DPoint w0,w1,w2;
      port.port2world(co.p_info->lpoints[0],&w0);
      port.port2world(co.p_info->lpoints[1],&w1);
      port.port2world(co.p_info->lpoints[2],&w2);
      Vector side1,side2;
      side1 = Vector(R_3DPoint(w1 - w0));
      side2 = Vector(R_3DPoint(w2 - w1));
      co.surface_normal = side1.Cross(side2);
      co.surface_normal.Normalize();
      co.plane_constant = -co.surface_normal.Dot(Vector(w0));
      */
      o_polys[i].set_world_points(rf_port);
    }
}

void C_Oriented_Shape::set_visible(Port_3D &theport)
{
  int i;

  if (frame_switch)
    {
      visible = 1;
      for (i=0;i<npolys;i++)
	o_polys[i].visible = 1;
      return;
    }
  visible = 0;
  for (i = 0; i<npolys;i++)
    {
      if (O_CheckPlaneEquation(Vector(theport.look_from),
			       o_polys[i].surface_normal,
			       o_polys[i].plane_constant))
	{
	  o_polys[i].visible = 1;
	  visible = 1;
	}
      else
	o_polys[i].visible = 0;
    }

}

void C_Oriented_Shape::set_poly_icolor(Vector &light)
{
  for (int i=0;i<npolys;i++)
    {
      // do it "inline"
      C_Oriented_Poly &co = o_polys[i];
      if (co.visible && (co.flags & I_SHADE))
	{
	  float intensity = O_Intensity(light,co.surface_normal);
	  if (intensity > 1.0 )
	    intensity = 1.0;
	  {
	    int col = co.base_color + (int) (intensity * ((float) co.color_range - 1) );
	    if (col <= co.base_color + co.color_range &&
		col >= co.base_color)
	      co.color = col;
	  }
	}
      //		if (o_polys[i].visible)
      //			o_polys[i].set_poly_icolor(light);
    }
}

void C_Oriented_Shape::render_shadow(Port_3D &port, Vector &light)
{
  MYCHECK(ref_port != NULL);
  for (int i=0;i<npolys;i++)
    {
      if (O_CheckPlaneEquation(light,o_polys[i].surface_normal,
			       o_polys[i].plane_constant))
	o_polys[i].render_shadow(port,(int) (flags & Z_CLIPME),0,
				 light,0,*ref_port);
    }
}

void C_Oriented_Shape::draw(Port_3D &port, R_3DPoint &)
{
  int i;
  if (!visible)
    return;
  MYCHECK(ref_port != NULL);
  for (i = 0; i <npolys;i++)
    {
      if (o_polys[i].visible)
	o_polys[i].draw(port,*ref_port,1);
      else if (o_polys[i].wpoints != NULL)
	{
	  delete [] o_polys[i].wpoints;
	  o_polys[i].wpoints = NULL;
	}
    }
}

void C_Oriented_Shape::create_points(Port_3D &ref_port)
{
  for (int i=0;i<npolys;i++)
    o_polys[i].create_points(ref_port);
}

void C_Oriented_Shape::delete_points()
{
  for (int i=0;i<npolys;i++)
    {
      if (o_polys[i].wpoints != NULL)
	delete [] o_polys[i].wpoints;
      o_polys[i].wpoints = NULL;
    }
}

void C_Oriented_Shape::set_poly_color(int n, int color)
{
  if (n >= 0 && n < npolys)
    o_polys[n].color = color;
}
