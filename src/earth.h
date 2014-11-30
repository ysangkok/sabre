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
 * File   : earth.h                              *
 * Date   : March, 1997                          *
 * Author : Dan Hammer                           *
 *************************************************/
#ifndef __earth_h
#define __earth_h

#include "terrain.h"

class Earth_Watch
{
public:
  R_3DPoint wh_center, wh_left, wh_right;
  int h_x,h_y,h_x1,h_y1,h_x2,h_y2;
  float cos_ang, sin_ang;
  int negative_phi,negative_roll;
  REAL_TYPE unit_size;
  REAL_TYPE x_width,y_width;
  REAL_TYPE wh_distance;
  int local_max;
  int all_sky;
  int all_ground;
  TxtPoint tpoints1[4];
  TxtPoint tpoints2[4];
  C_PolyInfo *g_polyinfos;
  C_3DObject_Group *groups;
  C_3DPoly **polys;
  unsigned int ngroups;
  unsigned int n3dpolys;
  unsigned int npolys;
  float scale;
 
  int ground_color,sky_color;
  int local_color_1,local_color_2;
  int sky_grade_color,ground_grade_color;
  int grade_count;
  int grade_count2;
  int ground_grade_flag;
  int terrain_on;

  Earth_Watch()
    :g_polyinfos(NULL),
     groups(NULL),
     polys(NULL)
    { }
  Earth_Watch(int);
  virtual ~Earth_Watch();
  void calc_texture_bounds();
  void calc_texture_bounds(C_PolyInfo &, REAL_TYPE, REAL_TYPE);
  void draw(Port_3D &, int grade = 1);
  void drawBackDrop(Port_3D &);
  void draw_horizon(Port_3D &, int grade = 1);
  void draw_horizon_grade(Port_3D &);
  void render_ground(Port_3D &);
  void t_render_ground(Port_3D &);
  friend std::istream &operator >>(std::istream &is, Earth_Watch &);
  virtual void read(std::istream &is);
};

class Earth_Watch2 : public Earth_Watch
{
public:
  Terrain_Shape *tshapes;
  int            ntshapes;
  virtual void read(std::istream &is);
  REAL_TYPE getGroundLevel(R_3DPoint &p);
};

C_3DPoly **resizePolyPtr(C_3DPoly **polys, unsigned int old_size, int amt);

extern Earth_Watch2 *the_earth;
extern REAL_TYPE getGroundLevel(R_3DPoint &p);
#endif


