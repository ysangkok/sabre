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
 * File   : group_3d.C                           *
 * Date   : March, 1997                          *
 * Author : Dan Hammer                           *
 *************************************************/
#include <stdio.h>
#include <values.h>
#include <math.h>
#include <iostream>
#include "simerr.h"
#include "grafix.h"
#include "vmath.h"
#include "port_3d.h"
#include "cpoly.h"
#include "obj_3d.h"
#include "group_3d.h"

int C_3DObject_Group::min_pixel_width = 8;


inline void SSET(Port_3D &port, R_3DPoint &p, R_2DPoint &s, bounding_cube &cube)
{
  if (p.z < 0.0)
    p.z = -p.z;
  port.port2screen(p,&s);
  cube.set(s);
}

C_3DObject_Group::C_3DObject_Group()
{
  bounds.reset();
}

void C_3DObject_Group::addObject(C_3DObject_Base *obj)
{
  if (C_3DObject_List::add_object(obj))
    bounds.set(obj->bcube);
}

bool C_3DObject_Group::isVisible(Port_3D &port)
{
  visible = ::isVisible(port,bounds);
  return (visible);
}

void C_3DObject_Group::addDrawList(DrawList &dlist, Port_3D &port)
{
  unsigned int i;
  if ((n_objects > 0) && isVisible(port))
    {
      for (i = 0; i<n_objects;i++)
	if (objects[i]->visible)
	  objects[i]->draw_prep(port);
      for (i=0;i<n_objects;i++)
	{
	  if (objects[i]->visible && objects[i]->visible_flag)
	    dlist.add_object(objects[i]);
	}
    }
}

inline int VVTEST(R_3DPoint &p0, R_3DPoint &p1, R_3DPoint &p2, 
		  R_3DPoint &p3, Port_3D &port, REAL_TYPE minz);
void VVSHOW(R_3DPoint &p0, R_3DPoint &p1, R_3DPoint &p2, 
		  R_3DPoint &p3, Port_3D &port, REAL_TYPE minz);
inline int TOPORT(REAL_TYPE x, REAL_TYPE y, REAL_TYPE z, 
		  Port_3D &port, R_3DPoint *p,
		  REAL_TYPE minz);
static int vvvshow = 0;
/*
Try to determine if a bounding_cube is visible by 
seeing if any of the faces formed from it's vertices
would be visible.
*/
bool isVisible(Port_3D &port, bounding_cube &bounds)
{

  float minz;
  R_3DPoint p0,p1,p2,p3,p4,p5,p6,p7;
  // bounding_cube scube;
  int visible;
  int zz;
  visible = 1;

  if (zbff_flag)
    minz = zbuff_min;
  else
    minz = port.z_min;

  /* count of vertices behind us */
  zz = 0;
  zz += TOPORT(bounds.max_x,bounds.max_y,bounds.max_z,port,&p0,minz);
  zz += TOPORT(bounds.max_x,bounds.max_y,bounds.min_z,port,&p1,minz);
  zz += TOPORT(bounds.max_x,bounds.min_y,bounds.min_z,port,&p2,minz);
  zz += TOPORT(bounds.max_x,bounds.min_y,bounds.max_z,port,&p3,minz);

  zz += TOPORT(bounds.min_x,bounds.min_y,bounds.max_z,port,&p4,minz);
  zz += TOPORT(bounds.min_x,bounds.min_y,bounds.min_z,port,&p5,minz);
  zz += TOPORT(bounds.min_x,bounds.max_y,bounds.min_z,port,&p6,minz);
  zz += TOPORT(bounds.min_x,bounds.max_y,bounds.max_z,port,&p7,minz);
  if (zz == 8)
    visible = 0;
  else
    {
      /* If any face is visible, we break */
      if (VVTEST(p0,p1,p2,p3,port,minz))
	if (VVTEST(p3,p2,p5,p4,port,minz))
	  if (VVTEST(p4,p7,p6,p5,port,minz))
	    if (VVTEST(p7,p0,p1,p6,port,minz))
	      if (VVTEST(p0,p3,p4,p7,port,minz))
		if (VVTEST(p1,p2,p5,p6,port,minz))
		  visible = 0;
    }

  if (visible && vvvshow)
    {
      VVSHOW(p0,p1,p2,p3,port,minz);
      VVSHOW(p3,p2,p5,p4,port,minz);
      VVSHOW(p4,p7,p6,p5,port,minz);
      VVSHOW(p7,p0,p1,p6,port,minz);
      VVSHOW(p0,p3,p4,p7,port,minz);
      VVSHOW(p1,p2,p5,p6,port,minz);
    }

  return (visible);
}

/* For bounding cubes in port coords */
bool isVisible(Port_3D &port, Port_3D &ref_port, bounding_cube &bounds)
{

  /* Convert to a world bounding cube */
  /* FIX ME! world cube isn't exactly right but
     works anyway
     */
  bool result;
  bounding_cube wcube;
  R_3DPoint p,w;
  wcube.reset();
  p.x = bounds.max_x;
  p.y = bounds.max_y;
  p.z = bounds.max_z;
  ref_port.port2world(p,&w);
  wcube.set(w);
  p.x = bounds.min_x;
  p.y = bounds.min_y;
  p.z = bounds.min_z;
  ref_port.port2world(p,&w);
  wcube.set(w);
  //  vshow = 1;
  result = isVisible(port,wcube);
  // vshow = 0;
  return (result);
}

/* Return true if face NOT visible */
inline int VVTEST(R_3DPoint &p0, R_3DPoint &p1, R_3DPoint &p2, 
		  R_3DPoint &p3, Port_3D &port, REAL_TYPE minz)
{
  R_3DPoint ppoints[4];
  R_3DPoint rclipped_points[16];
  R_2DPoint screen_points[16];
  R_2DPoint clipped_points[32];
  unsigned int       npoints;
  unsigned int       nxpoints;
  bool result = 1;
  int zclp = 0;
  if (p0.z < minz)
    zclp++;
  if (p1.z < minz)
    zclp++;
  if (p2.z < minz)
    zclp++;
  if (p3.z < minz)
    zclp++;
  /* if at least one vertice in front */
  if (zclp < 4)
    {
      /* need to zclip */
      if (zclp > 0)
	{
	  ppoints[0] = p0;
	  ppoints[1] = p1;
	  ppoints[2] = p2;
	  ppoints[3] = p3;
	  if (zpoly_clip(ppoints,4,rclipped_points,&npoints,minz))
	    {
	      MYCHECK(npoints < 16);
	      for (unsigned int i=0;i<npoints;i++)
		port.port2screen(rclipped_points[i],&screen_points[i]);
	      if (r_poly_clip(screen_points,clipped_points,npoints,
			      &nxpoints,&port.screen))
		result = 0;
	    }
	}
      else
	{
	  port.port2screen(p0,&screen_points[0]);
	  port.port2screen(p1,&screen_points[1]);
	  port.port2screen(p2,&screen_points[2]);
	  port.port2screen(p3,&screen_points[3]);
	  if (r_poly_clip(screen_points,clipped_points,4,&npoints,
			  &port.screen))
	    result = 0;
	}
    }
  return (result);
}

extern void r_frame_convpoly(R_2DPoint *points, unsigned int n);

void VVSHOW(R_3DPoint &p0, R_3DPoint &p1, R_3DPoint &p2, 
		  R_3DPoint &p3, Port_3D &port, REAL_TYPE minz)
{
  R_3DPoint ppoints[4];
  R_3DPoint rclipped_points[16];
  R_2DPoint screen_points[16];
  R_2DPoint clipped_points[32];
  unsigned int       npoints;
  unsigned int       nxpoints;

  int zclp = 0;
  if (p0.z < minz)
    zclp++;
  if (p1.z < minz)
    zclp++;
  if (p2.z < minz)
    zclp++;
  if (p3.z < minz)
    zclp++;
  /* if at least one vertice in front */
  if (zclp < 4)
    {
      /* need to zclip */
      if (zclp > 0)
	{
	  ppoints[0] = p0;
	  ppoints[1] = p1;
	  ppoints[2] = p2;
	  ppoints[3] = p3;
	  if (zpoly_clip(ppoints,4,rclipped_points,&npoints,minz))
	    {
	      MYCHECK(npoints < 16);
	      for (unsigned int i=0;i<npoints;i++)
		port.port2screen(rclipped_points[i],&screen_points[i]);
	      if (r_poly_clip(screen_points,clipped_points,npoints,
			      &nxpoints,&port.screen))
		{
		  nxpoints++;
		  clipped_points[nxpoints - 1] = clipped_points[0];
		  r_frame_convpoly(clipped_points,nxpoints);
		}
	    }
	}
      else
	{
	  port.port2screen(p0,&screen_points[0]);
	  port.port2screen(p1,&screen_points[1]);
	  port.port2screen(p2,&screen_points[2]);
	  port.port2screen(p3,&screen_points[3]);
	  if (r_poly_clip(screen_points,clipped_points,4,&npoints,
			  &port.screen))
	    {
	      npoints++;
	      clipped_points[npoints - 1] = clipped_points[0];
	      r_frame_convpoly(clipped_points,npoints);
	    }
	}
    }

}

inline int TOPORT(REAL_TYPE x, REAL_TYPE y, REAL_TYPE z, 
		  Port_3D &port, R_3DPoint *p,
		  REAL_TYPE minz)
{
  R_3DPoint w;
  w.x = x;
  w.y = y;
  w.z = z;
  port.world2port(w,p);
  if (p->z < minz)
    return (1);
  else
    return (0);
}

/*
  if (VTEST(bounds.min_x,bounds.min_y,bounds.min_z,port,minz,&p0))
    if (VTEST(bounds.min_x,bounds.min_y,bounds.max_z,port,minz,&p1))
      if (VTEST(bounds.min_x,bounds.max_y,bounds.min_z,port,minz,&p2))
	if (VTEST(bounds.min_x,bounds.max_y,bounds.max_z,port,minz,&p3))
	  if (VTEST(bounds.max_x,bounds.min_y,bounds.min_z,port,minz,&p4))
	    if (VTEST(bounds.max_x,bounds.min_y,bounds.max_z,port,minz,&p5))
	      if (VTEST(bounds.max_x,bounds.max_y,bounds.min_z,port,minz,&p6))
		if (VTEST(bounds.max_x,bounds.max_y,bounds.max_z,port,minz,&p7))
		  visible = 0 ;
inline int VTEST(REAL_TYPE x, REAL_TYPE y, REAL_TYPE z, 
		 Port_3D &port, REAL_TYPE min_z, R_3DPoint *p)
{
  R_3DPoint w;
  w.x = x;
  w.y = y;
  w.z = z;
  port.world2port(w,p);
  return (p->z < min_z);
}
*/
