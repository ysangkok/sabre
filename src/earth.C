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
 * File   : earth.C                              *
 * Date   : March, 1997                          *
 * Author : Dan Hammer                           *
 * background rendering of earth & sky           *
 *************************************************/
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <limits.h>
#include <values.h>
//#define fmin(a,b) (a) < (b) ? (a) : (b)
//#define fmax(a,b) (a) > (b) ? (a) : (b)
#include "vmath.h"
#include "grafix.h"
#include "vga_13.h"
#include "pen.h"
#include "port_3d.h"
#include "clip.h"
#include "cpoly.h"
#include "obj_3d.h"
#include "group_3d.h"
#include "txtrmap.h"
#include "rendpoly.h"
#include "rndrpoly.h"
#include "rotate.h"
#include "traveler.h"
#include "simfile.h"
#include "earth.h"
#include "grndlevl.h"

extern int frame_switch;
extern REAL_TYPE world_scale;

Vector    ggNormal;
REAL_TYPE ggplane_constant;
TPoly     *ggtpoly;

Earth_Watch2 *the_earth = NULL;

Earth_Watch::Earth_Watch(int)
  :g_polyinfos(NULL),
   groups(NULL),
   polys(NULL)
{
  unit_size = (REAL_TYPE) 1.3;
  x_width = y_width = 5;
  terrain_on = 1;
}

Earth_Watch::~Earth_Watch()
{
  if (g_polyinfos)
    delete [] g_polyinfos;
  if (groups)
    delete [] groups;
  if (polys)
    {
      for (int i=0;i<n3dpolys;i++)
	if (polys[i])
	  delete polys[i];
      delete [] polys;
    }
}

C_3DPoly **resizePolyPtr(C_3DPoly **polys, int old_size, int amt)
{
  int size = old_size + amt;
  C_3DPoly **result;

  result = new C_3DPoly *[size];
  MYCHECK(result != NULL);
  
  for (int i=0;i<old_size;i++)
    result[i] = polys[i];

  delete [] polys;
  
  return (result);
}

void Earth_Watch::read(std::istream &is)
{
  char c;
  int i,j,idx;
  int n;
  R_3DPoint loc;
  REAL_TYPE dab_min;
  READ_TOKI('{',is,c)
  is >> unit_size;
  is >> dab_min;
  is >> wh_distance;
  is >> local_color_1 >> local_color_2;

  is >> ground_color >> sky_color;
  is >> sky_grade_color >> ground_grade_color;
  is >> grade_count;

  unit_size *= world_scale;
  wh_distance *= world_scale;
  dab_min *= world_scale;
  C_3DObject_Base::dab_min = dab_min;

  is >> scale;
  is >> npolys;
  g_polyinfos = new C_PolyInfo[npolys];
  MYCHECK(g_polyinfos != NULL);
  for (i=0;i<npolys;i++)
    is >> g_polyinfos[i];
  calc_texture_bounds();


  is >> ngroups;
  groups = new C_3DObject_Group[ngroups];
  MYCHECK(groups != NULL);

  n3dpolys = 0;
  polys = NULL;
  idx = 0;
  for (i=0;i<ngroups;i++)
    {
      READ_TOKI('{',is,c)
	is >> loc;
      is >> n;
      if (polys == NULL)
	{
	  n3dpolys = n;
	  polys = new C_3DPoly *[n];
	  MYCHECK(polys != NULL);
	}
      else
	{
	  polys = resizePolyPtr(polys, n3dpolys, n);
	  n3dpolys += n;
	}
	    
      for (j=0;j<n;j++)
	{
	  C_PolyInfo *an_info;
	  int info_idx;
	  R_3DPoint a_location;
	  poly_params a_param;
	  READ_TOKI('[',is,c)
	    is >> info_idx;
	  MYCHECK(info_idx >= 0 && info_idx < npolys);
	  an_info = & g_polyinfos[info_idx];
	  is >> a_location;
	  a_location += loc;
	  a_location *= (scale * world_scale);
	  is >> a_param;
	  
	  C_3DPoly *cp = new C_3DPoly(an_info, a_location, & a_param, 
				      scale * world_scale);
	  MYCHECK(cp != NULL);
	  polys[idx++] = cp;
	  groups[i].addObject(cp);
	  READ_TOKI(']',is,c)
	}
      READ_TOK('}',is,c)
    }
  READ_TOKI('}',is,c)
}

std::istream &operator >>(std::istream &is, Earth_Watch &ew)
{
  ew.read(is);
  return is;
}


/********************************************************
 * draw it all                                          *
 ********************************************************/
void Earth_Watch::draw(Port_3D &port, int)
{
  drawBackDrop(port);
  render_ground(port);
}

/******************************************************
 * draw sky and graded horizon                        *
 *****************************************************/
void Earth_Watch::drawBackDrop(Port_3D &port)
{
  draw_horizon(port,1);
  if (grade_count && !frame_switch)
    draw_horizon_grade(port);
}

/*********************************************************
 * Do a Genesis sort of thing -- divide up the world     *
 * into a basic earth/sky duality.                       *
 *********************************************************/
const REAL_TYPE phi_fudge = (REAL_TYPE) 0.03;

void Earth_Watch::draw_horizon(Port_3D &port, int)
{
  R_3DPoint wpoly[4];

  // This little nasty deals with phi values betwixt 2pi & pi
  // during which the view is inverted
  negative_phi= 0;
  // And this little nasty deals with roll values betwixt
  // pi/2 and 3pi/2, in which the view is also inverted
  negative_roll = 0;

  // Reset flags
  all_sky = all_ground = 0;


  if ((port.slook_from.phi < _2PI) &&
      (port.slook_from.phi > _PI))
    negative_phi = 1;


  if ((port.roll > _PI2) &&
      (port.roll < _PI34))
    negative_roll = 1;

  // Check for values of phi near 0,2pi & pi -- these are
  // situtations in which we are either pointing straight up or down
  if ((port.slook_from.phi < phi_fudge) ||
      (port.slook_from.phi > (_2PI - phi_fudge)))
    {
      // phi near 0 -- we're looking straight down
      fill_rect(port.screen,ground_color);
      all_ground = 1;
      return;
    }
  else if ((port.slook_from.phi > (_PI - phi_fudge)) &&
	   (port.slook_from.phi < (_PI + phi_fudge)))
    {
      fill_rect(port.screen,sky_color);
      all_sky = 1;
      return;
    }

  fill_rect(port.screen,sky_color);

  int top_edge = 0 ; // port.screen.topLeft.y;
  int bottom_edge = SCREEN_HEIGHT - 1 ; // port.screen.botRight.y;
  Point t1,t2;
  Port_3D tport;
  tport = port;
  tport.look_from.z = 0.0;
  tport.look_at.z = 0.0;

  tport.get_view_horizon(wh_distance,&wh_left,&wh_center,&wh_right);

  R_3DPoint tmp1,tmp2;

  tmp1 = wh_left;
  tmp1.z -= 10000;
  tmp2 = wh_right;
  tmp2.z -= 10000;

  tport.transform(wh_left,&h_x1,&h_y1);
  tport.transform(wh_right,&h_x2,&h_y2);
  tport.transform(wh_center,&h_x,&h_y);

  t1 = Point(h_x1,h_y1);
  t2 = Point(h_x2,h_y2);

  if (negative_phi)
    {
      if (negative_roll)
	{
	  if (t1.y <= top_edge && t2.y <= top_edge)
	    {
	      fill_rect(port.screen,ground_color);
	      all_ground = 1;
	      return;
	    }
	  if (t1.y >= bottom_edge && t2.y >= bottom_edge)
	    {
	      all_sky = 1;
	      return;
	    }
	}
      else
	{
	  if (t1.y >= bottom_edge && t2.y >= bottom_edge)
	    {
	      fill_rect(port.screen,ground_color);
	      all_ground = 1;
	      return;
	    }
	  if (t1.y < top_edge && t2.y < top_edge)
	    {
	      all_sky = 1;
	      return;
	    }
	}
    }
  else
    {
      if (negative_roll)
	{
	  if (t1.y >= bottom_edge && t2.y >= bottom_edge)
	    {
	      fill_rect(port.screen,ground_color);
	      all_ground = 1;
	      return;
	    }
	  if (t1.y <= top_edge && t2.y <= top_edge)
	    {
	      all_sky = 1;
	      return;
	    }
	}
      else
	{
	  if (t1.y <= top_edge && t2.y <= top_edge)
	    {
	      fill_rect(port.screen,ground_color);
	      all_ground = 1;
	      return;
	    }
	  if (t1.y >= bottom_edge && t2.y >= bottom_edge)
	    {
	      all_sky = 1;
	      return;
	    }
	}
    }

  
  wpoly[0] = wh_left;
  wpoly[1] = wh_right;
  wpoly[2] = tmp2;
  wpoly[3] = tmp1;

  rendpoly(wpoly,4,ground_color,tport,1);
  if (frame_switch)
    {
      frame_switch = 0;
      rendpoly(wpoly,4,ground_color,tport,1);
      frame_switch = 1;
    }

}


/*********************************************************
 * Depict the sky fading as it nears the horizon         *
 *********************************************************/
void Earth_Watch::draw_horizon_grade(Port_3D &port)
{

  R_3DPoint s[4];
  R_3DPoint g[4];
  float grade_depth = 3.0;

  int gcol = ground_grade_color + grade_count - 1;
  int scol = sky_grade_color + grade_count - 1;

  Port_3D tport;
  tport = port;
  tport.look_from.z = 0.0;
  tport.look_at.z = 0.0;

  s[0] = s[3] = g[0] = g[3] = wh_left;
  s[1] = s[2] = g[1] = g[2] = wh_right;

  s[2].z += grade_depth;
  s[3].z += grade_depth;
  g[2].z -= grade_depth;
  g[3].z -= grade_depth;

  for (int n=0;n<grade_count;n++)
    {
      rendpoly(s,4,scol--,tport,1);
      rendpoly(g,4,gcol--,tport,1);
      s[0].z = s[1].z = s[2].z;
      s[2].z += grade_depth;
      s[3].z += grade_depth;
      g[0].z = g[1].z = g[2].z;
      g[2].z -= grade_depth;
      g[3].z -= grade_depth;
      grade_depth += 0.5;
    }
}

/**********************************************************
 * setup texture coordinants                              *
 **********************************************************/
void Earth_Watch::calc_texture_bounds()
{
  REAL_TYPE t_w = 0.0, t_h = 0.0;
  int i;
  if (map_man != NULL)
    {
      TextrMap &tmap = map_man->get_map(local_color_1);
      bool before = &tmap != &nullmap;
      if (before)
	{
	  t_w = tmap.map_w;
	  t_h = tmap.map_h;
	  tpoints1[1].u = tmap.map_w;
	  tpoints1[2].u = tmap.map_w;
	  tpoints1[2].v = tmap.map_h;
	  tpoints1[3].v = tmap.map_h;
	  TextrMap &tmap2 = map_man->get_map(local_color_2);
	  if (&tmap2 != &nullmap)
	    {
	      tpoints2[1].u = tmap2.map_w;
	      tpoints2[2].u = tmap2.map_w;
	      tpoints2[2].v = tmap2.map_h;
	      tpoints2[3].v = tmap2.map_h;
	    }
	}
      if (before) {
	  if (before != (&tmap != &nullmap)) abort();
	  for (i=0;i<npolys;i++)
	    calc_texture_bounds(g_polyinfos[i],t_w,t_h);
      }
    }
}

/****************************************************************
 * Fit the poly's texture coordinants for a map of width tw,    *
 * height th. Assumption is that the poly is parallel to the    *
 * z axis                                                       *
 ****************************************************************/
void Earth_Watch::calc_texture_bounds(C_PolyInfo &ply, REAL_TYPE tw, 
				      REAL_TYPE th)
{
  REAL_TYPE xspan,yspan;
  REAL_TYPE min_x,min_y;
  REAL_TYPE dx,dy;

  tw -= 1.0;
  th -= 1.0;
  if (ply.tpoints == NULL)
    {
      ply.tpoints = new TxtPoint[ply.npoints];
      MYCHECK(ply.tpoints != NULL);
    }
  xspan = ply.bcube.max_x - ply.bcube.min_x;
  yspan = ply.bcube.max_y - ply.bcube.min_y;
  min_x = ply.bcube.min_x;
  min_y = ply.bcube.min_y;
  for (int i=0;i<ply.npoints;i++)
    {
      R_3DPoint &p = ply.lpoints[i];
      dx = (REAL_TYPE) fabs((p.x - min_x) / xspan);
      dy = (REAL_TYPE) fabs((p.y - min_y) / yspan);
      
      ply.tpoints[i].u = tw * dx;
      ply.tpoints[i].v = th * dy;
      //      cerr << ply.lpoints[i] << ply.tpoints[i] << '\n';
    }
}

void Earth_Watch::t_render_ground(Port_3D &port)
{
  int i;
  DrawList dlist;
  for (i=0;i<ngroups;i++)
    groups[i].addDrawList(dlist,port);
  dlist.draw(port);
}

void Earth_Watch::render_ground(Port_3D &port)
{
  Terrain_Shape::txtr_flag = terrain_on;
  t_render_ground(port);
}

void Earth_Watch2::read(std::istream &is)
{
  char c;
  int i,j;
  int rsrv;
  char terrain_path[100];
  std::ifstream ts;
  float dab_min;

  R_3DPoint loc;
  REAL_TYPE zlevels[8];
  REAL_TYPE percents[4];
  REAL_TYPE width,length;
  TextrMap  *tmap;
  READ_TOKI('{',is,c)
  is >> unit_size;
  is >> dab_min;
  is >> wh_distance;

  is >> local_color_1 >> local_color_2;

  is >> ground_color >> sky_color;
  is >> sky_grade_color >> ground_grade_color;
  is >> grade_count >> grade_count2;

  //  unit_size *= world_scale;
  wh_distance *= world_scale;
  dab_min *= world_scale;
  C_3DObject_Base::dab_min = dab_min;

  is >> ngroups;
  groups = new C_3DObject_Group[ngroups];
  MYCHECK(groups != NULL);
  is >> scale;

  is >> terrain_path;
  
  if (open_is(ts,terrain_path))
    {
      ts >> ntshapes;
      tshapes = new Terrain_Shape[ntshapes];
      MYCHECK(tshapes != NULL);
      
      rsrv = map_man->getReserved();

      Terrain_Shape::color = local_color_1;
      Terrain_Shape::color_range = local_color_2;
      
      for (i=0;i<ntshapes;i++)
	{
	  int grp,tmp;
	  
	  READ_TOKI('[',ts,c);
	  ts >> loc;
	  loc *= world_scale * scale;
	  ts >> grp;
	  MYCHECK(grp >= 0 && grp < ngroups);
	  ts >> tmp;
	  tmp += rsrv;
	  tmap = map_man->get_map_ptr(tmp);
	  MYCHECK(tmap != NULL);
	  ts >> width >> length;
	  width *= world_scale * scale;
	  length *= world_scale * scale;
	  
	  for (j=0;j<8;j++)
	    {
	      ts >> zlevels[j];
	      zlevels[j] *= world_scale * unit_size;
	    }
	  tshapes[i].setup(loc,width,length,
			   zlevels,
			   percents,
			   tmap);
	  tshapes[i].dab_min = dab_min;
	  groups[grp].addObject(&tshapes[i]);
	}
      ts.close();
    }
  READ_TOKI('}',is,c)
}

//static int gggresult = 0;

REAL_TYPE Earth_Watch2::getGroundLevel(R_3DPoint &tp)
{
  R_3DPoint tmp;
  REAL_TYPE result = 0.0;

  ggplane_constant = 0.0;
  ggNormal = Vector(0,0,1);
  ggtpoly = NULL;
  tmp = tp;
  for (int i=0;i<ngroups;i++)
    {
      if (
	  ( groups[i].getNObjects() > 0 )
	  &&  (XYBOUNDS(groups[i].bounds,tp))
          )
	{
	  for (int j = 0;j<groups[i].getNObjects();j++)
	    {
	      Terrain_Shape *ts = (Terrain_Shape *) groups[i].getObject(j);
	      if (ts && ts->getGroundLevel(tmp))
		{
		  result = tmp.z;
		  break;
		}
	    }
	}
    }
  if (result < 0.0)
    result = 0.0;
  return(result);
}

REAL_TYPE getGroundLevel(R_3DPoint &p)
{
  if (the_earth != NULL)
    return (the_earth->getGroundLevel(p));
  else
    return (0.0);
}



