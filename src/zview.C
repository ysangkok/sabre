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
/***************************************************
 *           Sabre Fighter Plane Simulator         *
 * Version: 0.1                                    *
 * File   : zview.C                                *
 * Date   : March, 1997                            *
 * Author : Dan Hammer                             *
 * zviewer class                                   *
 * This class performs an object-level depth sort  *
 * rendering. Not really needed when using a       *
 * a z-buffer.                                     *
 * The sort is preconfigured. Basically, you define*
 * which shapes are to be drawn in which order     *
 * based on the orientation of the viewer.         *
 *                                                 *
 * 08/10/97                                        *
 * Most of the Z_Node_Manager functionality is     *
 * irrelevant at this time as I am using a Z-buffer*
 * 03/09/98                                        * 
 * Removed classes and code made irrelevant by     *
 * z-buffer                                        *
 ***************************************************/
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <string.h>
#include <limits.h>
#include <values.h>
#include "defs.h"
#include "grafix.h"
#include "vga_13.h"
#include "vmath.h"
#include "port_3d.h"
#include "cpoly.h"
#include "obj_3d.h"
#include "group_3d.h"
#include "copoly.h"
#include "simerr.h"
#include "simfile.h"
#include "zview.h"
#include "convpoly.h"
#include "rendpoly.h"
#include "rndrpoly.h"
#include "colorspc.h"

typedef REAL_TYPE C;

extern float world_scale;

Z_Node_Manager **Z_Node_Manager::g_zmanagers = NULL;
int Z_Node_Manager::nzmanagers = 0;

Z_Node_Manager *Z_Node_Manager::getZNode(const char *id)
{
  Z_Node_Manager *result = NULL;
  if (g_zmanagers != NULL)
    {
      for (int i=0;i<nzmanagers;i++)
	{
	  if (g_zmanagers[i] != NULL)
	    {
	      if (!strcmp(id,g_zmanagers[i]->id))
		{
		  result = g_zmanagers[i];
		  break;
		}
	    }
	}
    }
  return (result);
}

Z_Node_Manager::Z_Node_Manager()
{
  n_shapes = 0;
  n_reference = 0;
  cur_shapes = NULL;
  cur_port = NULL;
  s_params = NULL;
  shapes = NULL;
  reference_shape = NULL;
  scaler = 1.0;
  id = NULL;
}

Z_Node_Manager::~Z_Node_Manager()
{
  if (s_params != NULL)
    delete [] s_params;
  if (shape_info != NULL)
    delete [] shape_info;
  if (reference_info != NULL)
    delete [] reference_info;
  if (shapes != NULL)
    delete [] shapes;
  if (reference_shape != NULL)
    delete [] reference_shape;
  if (id != NULL)
    delete id;
}

void Z_Node_Manager::setId(const char *new_id)
{
  if (id != NULL)
    delete id;
  id = strdup(new_id);
}

static const char *zzpath;
int Z_Node_Manager::read_file(const char *path)
{
  int result = 0;
  std::ifstream infile;
  if (open_is(infile,path))
    {
      zzpath = path;
      result = 1;
      infile >> *this;
      infile.close();
    }
  return result;
}
extern int shape_cnt;

void Z_Node_Manager::read(std::istream &is)
{
  char c;
  READ_TOKI('{',is,c)
    is >> scaler;
  unsigned int sn_reference;
  is >> sn_reference;
  n_reference = sn_reference;
  reference_info = new C_ShapeInfo[static_cast<unsigned int>(n_reference)];
  MYCHECK(reference_info != NULL);
  shape_cnt = 0;
  for (int i=0;i<n_reference;i++)
    {
      is >> reference_info[i];
      reference_info[i] *= scaler * world_scale;
    }
  unsigned int sn_shapes;
  is >> sn_shapes;
  n_shapes = sn_shapes;
  shape_info = new C_ShapeInfo[static_cast<unsigned int>(n_shapes)];
  MYCHECK(shape_info != NULL);
  s_params = new shape_params[static_cast<unsigned int>(n_shapes)];
  MYCHECK(s_params != NULL);
  for (int i=0;i<n_shapes;i++)
    {
      is >> c;
      while (is && (c != '{'))
	is >> c;
      if (!is)
	error_jump("!!Unexpected end-of-file!!");
      is >> shape_info[i];
      shape_info[i] *= scaler * world_scale;
      is >> s_params[i].flags;
      s_params[i].p_params = new poly_params[shape_info[i].npolys];
      s_params[i].n_params = shape_info[i].npolys;
      MYCHECK(s_params[i].p_params != NULL);
      for (int j=0;j<shape_info[i].npolys;j++)
	is >> s_params[i].p_params[j];
      is >> c;
      file_context = "ZView";
      while (is && (c != '}'))
	is >> c;
      if (!is)
	error_jump("%s: shape %d: ??Unexpected end-of-file??",zzpath,i);
    }

  READ_TOK('}',is,c);
  buildShapes();

  reference_shape = new C_Oriented_Shape[static_cast<unsigned int>(n_reference)];
  MYCHECK(reference_shape != NULL);
  for (int i=0;i<n_reference;i++)
    reference_shape[i].create(&(reference_info[i]));
}

std::istream & operator >>(std::istream &is, Z_Node_Manager &zm)
{
  zm.read(is);
  return(is);
}

void Z_Node_Manager::add(C_ShapeInfo &new_info, shape_params &new_param)
{
  C_ShapeInfo *new_infos;
  shape_params *new_params;
  new_infos = new C_ShapeInfo[static_cast<unsigned int>(n_shapes + 1)];
  MYCHECK(new_infos != NULL);
  new_params = new shape_params[static_cast<unsigned int>(n_shapes + 1)];
  MYCHECK(new_params != NULL);
  for (int i=0;i<n_shapes;i++)
    {
      new_infos[i] = shape_info[i];
      new_params[i] = s_params[i];
    }
  n_shapes++;
  new_infos[n_shapes - 1] = new_info;
  new_params[n_shapes - 1] = new_param;

  if (shape_info)
    delete [] shape_info;
  if (s_params)
    delete [] s_params;
  shape_info = new_infos;
  s_params = new_params;

  buildShapes();
}

void Z_Node_Manager::buildShapes()
{
  if (shapes)
    delete [] shapes;
  shapes = new C_Oriented_Shape[static_cast<unsigned int>(n_shapes)];
  MYCHECK(shapes != NULL);
  for (int i=0;i<n_shapes;i++)
    {
      shapes[i].create(&(shape_info[i]));
      shapes[i].set_params(&(s_params[i]));
      shapes[i].visible = 1;
    }
}

void Z_Node_Manager::deleteShape(int which)
{
  C_ShapeInfo *new_infos;
  shape_params *new_params;
  if (which < 0 || which >= n_shapes)
    return;
  new_infos = new C_ShapeInfo[static_cast<unsigned int>(n_shapes - 1)];
  MYCHECK(new_infos != NULL);
  new_params = new shape_params[static_cast<unsigned int>(n_shapes - 1)];
  MYCHECK(new_params != NULL);
  for (int i=0,ix=0;i<n_shapes;i++)
    {
      if (i != which)
	{
	  new_infos[ix] = shape_info[i];
	  new_params[ix] = s_params[i];
	  ix++;
	}
    }
  n_shapes--;
  if (shape_info)
    delete [] shape_info;
  if (s_params)
    delete [] s_params;
  shape_info = new_infos;
  s_params = new_params;


  buildShapes();
}

void Z_Node_Manager::create()
{
  int i;
  if (shapes != NULL)
    delete [] shapes;
  shapes = new C_Oriented_Shape[static_cast<unsigned int>(n_shapes)];
  MYCHECK(shapes != NULL);
  if (reference_shape != NULL)
    delete [] reference_shape;
  reference_shape = new C_Oriented_Shape[static_cast<unsigned int>(n_reference)];
  MYCHECK(reference_shape != NULL);
  for (i=0;i<n_shapes;i++)
    {
      shapes[i].create(&(shape_info[i]));
      shapes[i].set_params(&(s_params[i]));
      shapes[i].visible = 1;
    }
  for (i=0;i<n_reference;i++)
    reference_shape[i].create(&(reference_info[i]));

}

int Z_Node_Manager::write_file(const char *path)
{
  int result = 0;
  std::ofstream ofile;
  if (open_os(ofile,path))
    {
      result = 1;
      ofile << *this;
      ofile.close();
    }
  return result;
}

void Z_Node_Manager::write(std::ostream &os)
{
  int i;
  os << "{\n";
  os << scaler << "\n";
  os << static_cast<unsigned int>(n_reference) << "\n";

  for (i=0;i<n_reference;i++)
    {
      C_ShapeInfo tmp;
      tmp = reference_info[i];
      tmp *= C(1.0 / (scaler * world_scale));
      os << tmp;
    }
  os << static_cast<unsigned int>(n_shapes) << '\n';
  for (i=0;i<n_shapes;i++)
    {
      C_ShapeInfo tmp;
      os << "{\n";
      tmp = shape_info[i];
      tmp *= C(1.0 / (scaler * world_scale));
      os << tmp;
      os << s_params[i].flags << '\n';
      for (int j=0;j<shape_info[i].npolys;j++)
	os << s_params[i].p_params[j] << '\n';
      os << "}\n";
    }
  /*
    for (i = 0; i < node_count; i++)
    os << "Z" << nodes[i] << "\n";
    */
  os << "}\n";
}

void Z_Node_Manager::write_params(std::ostream &os, int i)
{
  if (i >= 0 && i < n_shapes)
    {
      os << s_params[i].flags << '\n';
      for (int j=0;j<shape_info[i].npolys;j++)
	os << s_params[i].p_params[j] << '\n';
    }
}

std::ostream & operator <<(std::ostream &os, Z_Node_Manager &zm)
{
  zm.write(os);
  return os;
}

void Z_Viewer::draw(Port_3D &port)
{
  MYCHECK(z_manager != NULL);
  MYCHECK(z_manager->n_shapes == nshapes);
  int tstps;

  tstps = TSTPS;
  TSTPS = 320;
  if (da_level == da_minimal)
    draw_minimal(port);
  else if (da_level == da_partial)
    draw_partial(port);
  else
    {
      Vector vc = world_light_source;
      vc.Normalize();
      for (int i=0;i<nshapes;i++)
	{
	  if (shapes[i].visible)
	    {
	      shapes[i].set_world_points(*reference_port);
	      shapes[i].set_visible(port);
	      shapes[i].set_poly_icolor(vc);
	    }
	  shapes[i].drawn_flag = 0;
	}
      
      if (draw_shadow)
	{
	  Vector v = world_light_source + Vector(reference_port->look_from);
	  for (int i=0;i<nshapes;i++)
	    {
	      if (shapes[i].visible)
		shapes[i].render_shadow(port,v);
	    }
	}

      for (int i=nshapes-1;i>=0;i--)
	if (shapes[i].visible)
	  shapes[i].draw(port,reference_port->look_from);
    }
  
  TSTPS = tstps;
  drawn_flag = 1;
  for (int i=0;i<nshapes;i++)
    shapes[i].visible = 1;

}

void Z_Viewer::draw_minimal(Port_3D &port)
{
  int color = shapes[0].o_polys[0].color;
  int x,y;
  R_3DPoint p;
  port.world2port(reference_port->look_from,&p);
  port.port2screen(p,&x,&y);
  if (!port.over_flow && is_visible(port.screen,x,y))
    {
      if (zbuff && zbff_flag)
	if (!setzbuff(x,y,p.z))
	  return;

      putpixel(x,y,color);
    }
}


void Z_Viewer::draw_partial(Port_3D &port)
{
  int flg = C_Poly::txtrflag;
  C_Poly::txtrflag = 0;
  Vector vc = world_light_source;
  vc.Normalize();
  
  for (int i=0;i<nreference;i++)
    reference_shape[i].set_world_points(*reference_port);

  for (int i=0;i<nshapes;i++)
    {
      if (shapes[i].flags & MAJOR_SHAPE)
	{
	  if (shapes[i].visible)
	    {
	      shapes[i].set_world_points(*reference_port);
	      shapes[i].set_visible(port);
	      shapes[i].set_poly_icolor(vc);
	    }
	  shapes[i].drawn_flag = 0;
	}
      else
	shapes[i].visible = 0;
    }

  if (draw_shadow)
    {
      Vector v = world_light_source + Vector(reference_port->look_from);
      for (int i=0;i<nshapes;i++)
	{
	  if (shapes[i].visible && shapes[i].flags & MAJOR_SHAPE)
	    shapes[i].render_shadow(port,v);
	}
    }

  for (int i=0;i<nshapes;i++)
    if ((shapes[i].flags & MAJOR_SHAPE) && shapes[i].visible)
      shapes[i].draw(port,reference_port->look_from);
  
  C_Poly::txtrflag = flg;
}


REAL_TYPE Z_Viewer::draw_prep(Port_3D &port)
{
  MYCHECK(reference_port != NULL);
  R_3DPoint port_local;
  drawn_flag = 0;
  first_flag = 0;
  port.world2port(reference_port->look_from,&port_local);
  z_order = port_local.z;
  if (z_order + bounding_sphere <= port.get_z_min())
    visible_flag = 0;
  else if (isVisible(port,*reference_port,bcube))
    {
      calc_screen_bounds(port,NULL);
      if (max_sc < min_sw)
	visible_flag = 0;
      else
	visible_flag = 1;
    }
  return z_order;
}

void Z_Viewer::init(Z_Node_Manager *zmd)
{
  MYCHECK(zmd != NULL);
  z_manager = zmd;
  MYCHECK(z_manager->n_shapes > 0);
  MYCHECK(z_manager->n_reference > 0);
  nshapes = z_manager->n_shapes;
  nreference = z_manager->n_reference;
  draw_shadow = 0;

  /*
    shapes = new C_Oriented_Shape[nshapes];
    MYCHECK(shapes != NULL);
    reference_shape = new C_Oriented_Shape[nreference];
    MYCHECK(reference_shape != NULL);
    for (int i=0;i<nshapes;i++)
    {
    shapes[i].create(&(z_manager->shape_info[i]));
    shapes[i].set_params(&(z_manager->s_params[i]));
    }
    for (i=0;i<nreference;i++)
    reference_shape[i].create(&(z_manager->reference_info[i]));
    */
 
  shapes = z_manager->shapes;
  reference_shape = z_manager->reference_shape;
  reference_port = NULL;
  build_bounding_cube();
  min_partial = 1.0;
  min_full = 16.0;
  min_sw = 0.5;
  visible_flag = 1;
  da_level = da_minimal;
}

Z_Viewer::Z_Viewer(Z_Node_Manager *zm)
{
  init(zm);
}

void Z_Viewer::setManager(Z_Node_Manager *zm)
{
  init(zm);
}

/*
Z_Viewer::~Z_Viewer()
{
    //if (shapes != NULL)
    //delete [] shapes;
    //if (reference_shape != NULL)
    //delete [] reference_shape;
}
*/

void Z_Viewer::build_bounding_cube()
{
  bcube.flg = 0;
  for (int i=0;i<nshapes;i++)
    {
      bounding_cube &bc = z_manager->shape_info[i].bcube;
      if (fabs(bc.min_y - bc.max_y) < world_scale)
	bc.max_y = bc.min_y + (world_scale * 4);
      if (fabs(bc.min_x - bc.max_x) < world_scale)
	bc.max_x = bc.min_x + (world_scale * 4);
      if (fabs(bc.min_z - bc.max_z) < world_scale)
	bc.max_z = bc.min_z + (world_scale * 4);
      bcube.set(bc);
    }
  REAL_TYPE x,y,z;
  x = abs_max(bcube.min_x,bcube.max_x);
  y = abs_max(bcube.min_y,bcube.max_y);
  z = abs_max(bcube.min_z,bcube.max_z);
  bounding_sphere = C(sqrt((x*x) + (y*y) + (z*z)));
}

void Z_Viewer::calc_screen_bounds(Port_3D &port, R_3DPoint *)
{
  R_3DPoint pp;

  port.world2port(reference_port->look_from,&pp);
  if (pp.z <= 0)
    {
      da_level = da_full;
      max_sc = min_full;
    }
  else
    {
      C_3DObject_Base::calc_screen_bounds(port,&pp);
      if (max_sc >= min_full)
	da_level = da_full;
      else if (max_sc >= min_partial)
	da_level = da_partial;
      else
	da_level = da_minimal;
    }
}

void Z_Viewer::write(std::ostream &os)
{
  os << "{\n";
  os << "XXXXXX\n";
  os << nshapes << '\n';

  for (int i=0;i<nshapes;i++)
    {
      os << "{\n";
      shapes[i].create_points(*reference_port);
      os << shapes[i] << '\n';
      shapes[i].delete_points();
      //      z_manager->write_params(os,i);
      os << z_manager->s_params[i];
      os << "}\n";
    }

  os << "}\n";
}

std::ostream &operator <<(std::ostream &os, Z_Viewer &zv)
{
  zv.write(os);
  return os;
}

