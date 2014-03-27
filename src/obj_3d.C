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
 * File   : obj_3d.C                             *
 * Date   : March, 1997                          *
 * Author : Dan Hammer                           *
 * Higher-level gathering of shapes and          *
 * polygons into objects and groups              *
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
#include "cpoly.h"
#include "obj_3d.h"
#include "group_3d.h"
#include "earth.h"
#include "kbdhit.h"

C_ShapeInfo *shape_infos = NULL;
extern Vector world_light_source;
extern REAL_TYPE world_scale;
extern int frame_switch;
extern char fcontext_buff[];
extern int shape_cnt;
extern int poly_cnt;
extern int point_cnt;

extern void blit_buff();

static const REAL_TYPE z_min = -99999999.0;
static const REAL_TYPE z_max =  99999999.0;

REAL_TYPE C_3DObject_Base::dab_min = 1000.0;

inline void ZMIN(REAL_TYPE x, REAL_TYPE y, REAL_TYPE z, 
		 Port_3D &port, REAL_TYPE &min_z)
{
  R_3DPoint w;
  w = R_3DPoint(x,y,z);
  REAL_TYPE d = distance_squared(port.look_from,w);
  if (d < min_z)
    min_z =  d;
}

/***************************************************************
 * C_3DObject_Base Methods                                     *
 ***************************************************************/

C_3DObject_Base::~C_3DObject_Base()
{

}

void C_3DObject_Base::calc_bounding_sphere()
{
  Vector v(bcube.max_x - bcube.min_x,
	   bcube.max_y - bcube.min_y,
	   bcube.max_z - bcube.min_z);
  bounding_sphere = v.Magnitude();
}

void C_3DObject_Base::calc_screen_bounds(Port_3D &port, R_3DPoint *p)
{
  REAL_TYPE scx1,scx2,scy1,scy2;
  R_3DPoint p1;

  max_sc = 0.0;
  p1 = *p;
  p1.x += bounding_sphere;
  port.port2screen(*p,&scx1,&scy1);
  port.port2screen(p1,&scx2,&scy2);
  max_sc = fabs(scx2 - scx1);
  /*
     if (frame_switch)
    {
    p1 = *p;
    p1.y += bounding_sphere;
    port.port2screen(p1,&scx3,&scy3);
    b_linedraw((int)scx1,(int)scy1,(int)scx2,(int)scy2,8,&port.screen);
    b_linedraw((int)scx1,(int)scy1,(int)scx3,(int)scy3,8,&port.screen);
    }
    */
 }

inline void SSET(Port_3D &port, REAL_TYPE x, REAL_TYPE y, REAL_TYPE z, bounding_cube &cube)
{
  R_3DPoint p;
  R_2DPoint s;
  p.x = x;
  p.y = y;
  p.z = z;
  port.port2screen(p,&s);
  cube.set(s);
}

REAL_TYPE C_3DObject_Base::draw_prep(Port_3D &port)
{
  REAL_TYPE min_z;
  R_3DPoint port_local;
  port.world2port(location,&port_local);
  z_order = port_local.z;
  visible_flag = 1;
  drawn_flag = 0;
  if (zbff_flag)
    min_z = 1.0;
  else
    min_z = port.get_z_min();
  if (z_order + bounding_sphere < min_z)
    visible_flag = 0;
  else 
    {
      if (port_local.z >= 0.0 )
	{
	  calc_screen_bounds(port,&port_local);
	  if (max_sc <= 1.0)
	    visible_flag = 0;
	}
    }
  return (z_order);
}

/***************************************************************
 * C_3DPoly Methods                                            *
 ***************************************************************/

C_3DPoly::C_3DPoly(C_PolyInfo *an_info, R_3DPoint & a_location,
		   poly_params *a_param, REAL_TYPE a_scale)
  : info(an_info),
    location(a_location),
    scale(a_scale),
    param(a_param)

{
  create_poly(poly);
  build_bounding_cube();
  calc_bounding_sphere();
}

void C_3DPoly::create_poly(C_Poly &poly)
{
  poly.set_scale(scale);
  poly.set_params(param);
  poly.create(info);
  poly.set_world_points(location);
}

void C_3DPoly::build_bounding_cube()
{
  bcube.reset();
  for (int i=0;i<info->npoints;i++)
    bcube.set(poly.wpoints[i]);
}

void C_3DPoly::translate(R_3DPoint &newloc)
{
  location = newloc;
  poly.set_world_points(newloc);
  build_bounding_cube();
}

REAL_TYPE C_3DPoly::draw_prep(Port_3D &port)
{
  REAL_TYPE min_z;
  R_3DPoint port_local;
  port.world2port(location,&port_local);
  z_order = port_local.z;
  visible_flag = 1;
  drawn_flag = 0;
  if (zbff_flag)
    min_z = 1.0;
  else
    min_z = port.get_z_min();
  if (z_order + bounding_sphere < min_z)
    visible_flag = 0;
  else 
    {
      if (port_local.z >= 0.0 )
	{
	  calc_screen_bounds(port,&port_local);
	  if (max_sc <= 1.0)
	    visible_flag = 0;
	}
    }
  return (z_order);
}

void C_3DPoly::draw(Port_3D &port)
{
  if (visible_flag)
    {
      //      poly.color = -1;
      poly.draw(port);
    }
  drawn_flag = 1;
}

/***************************************************************
 * C_3DObject_List Methods                                     *
 ***************************************************************/

int C_3DObject_List::add_object(C_3DObject_Base *obj)
{
  if (n_objects < MAX_OBJECTS - 1)
    {
      objects[++object_idx] = obj;
      n_objects++;
      return 1;
    }
  else
    return 0;
}

/***************************************************************
 * DrawList Methods                                            *
 ***************************************************************/

/*
   Straight non-sorted draw 
*/
void DrawList::drawz(Port_3D &port)
{
  for (int i=0;i<n_objects;i++)
    if (objects[i]->visible_flag)
      {
	objects[i]->draw(port);
	objects[i]->drawn_flag = 1;
      }
}

/* 
   draw the closest objects first
*/
void DrawList::draw(Port_3D &port)
{
  REAL_TYPE high_z;
  REAL_TYPE low_z;

  int i;
  C_3DObject_Base *cdraw;
  while (1)
    {
      cdraw = NULL;
      high_z = z_min;
      low_z = z_max;
      for (i=0;i<n_objects;i++)
	{
	  if ((objects[i]->visible_flag != 0) && (objects[i]->drawn_flag == 0))
	    {
	      if (objects[i]->z_order <= low_z)
		{
		  low_z = objects[i]->z_order;
		  cdraw = objects[i];
		}
	    }
	}
      if (cdraw)
	{
	  cdraw->draw(port);
	  cdraw->drawn_flag = 1;
	}
      else
	break;
    }
}


C_3DObjectInfo *C_3DInfoManager::getInfo(int i)
{
  if (i >= 0 && i < ninfos)
    return(&infos[i]);
  else
    return(NULL);
}

C_3DObjectInfo *C_3DInfoManager::getInfo(char *id)
{
  C_3DObjectInfo *result = NULL;
  for (int i=0;i<ninfos;i++)
    {
      if (!strcmp(id,infos[i].id))
	{
	  result = &infos[i];
	  break;
	}
    }
  return (result);
}

void C_3DInfoManager::readFile(char *path)
{
  std::ifstream is;
  if (this->path)
    delete this->path;
  this->path = strdup(path);
  if (open_is(is,path))
    read(is);
}

void C_3DInfoManager::writeFile(char *path)
{
  std::ofstream os;
  MYCHECK(path != NULL);
  if (this->path == NULL)
    this->path = strdup(path);
  else if (strcmp(this->path,path))
    {
      delete this->path;
      this->path = strdup(path);
    }
  if (open_os(os,path))
    write(os);
}

void C_3DInfoManager::read(std::istream &is)
{
  char c;
  char buff[256];

  READ_TOKI('{',is,c)
    is >> ninfos;
  infos = new C_3DObjectInfo[ninfos];
  MYCHECK(infos != NULL);

  for (int i=0;i<ninfos;i++)
    {
      is >> buff;
      if (*buff != '@')
	infos[i].readFile(buff);
      else
	is >> infos[i] ;
    }

  READ_TOKI('}',is,c)
}

void C_3DInfoManager::write(std::ostream &os)
{
  os << "{\n" << ninfos << "\n";
  for (int i=0;i<ninfos;i++)
    {
      os << infos[i].path << "\n";
      infos[i].writeFile(infos[i].path);
    }
  os << "}\n";
}


/***************************************************************
 * C_3DObject2 Methods                                         *
 ***************************************************************/

void C_3DObject2::read(std::istream &is)
{
  char c;
  int i;

  MYCHECK(im != NULL);

  READ_TOKI('{',is,c)
    is >> obj_id;
  ci = im->getInfo(obj_id);
  if (ci == NULL)
    {
      std::cout << "C_3DObject2: object id " << obj_id << "not found!\n";
      READ_TOKI('}',is,c);
      return;
    }
    is >> world_origin;
  world_origin *= world_scale;
  if (world_origin.z == 0.0 && the_earth)
    world_origin.z = the_earth->getGroundLevel(world_origin);
  
  is >> flags;
  if (flags & HIT_ME)
    is >> max_damage;
  is >> scale ;
  scale *= world_scale;
  w_origin = world_origin;

  nshapes = ci->nshapes;
  shapes = new C_Shape[nshapes];
  MYCHECK(shapes != NULL);
  bcubes = new bounding_cube[nshapes];
  MYCHECK(bcubes != NULL);

  for (i=0;i<nshapes;i++)
    {
      shapes[i].create(&ci->shapes[i]);
      shapes[i].set_scale(scale);
      shapes[i].set_world_location(w_origin);
      shapes[i].set_params(&ci->dflt_params[i]);
    }


  READ_TOK('}',is,c)

    Vector lt = world_light_source;
  lt.Normalize();
  for (i=0;i<nshapes;i++)
    {
      shapes[i].set_poly_icolor(lt);
      if (shapes[i].flags & SHADOW)
	{
	  Vector sh_l = world_light_source + Vector(w_origin);
	  shapes[i].set_shadow(w_origin.z,sh_l);
	}
    }
  build_bounding_cube();
  calc_bounding_sphere();
}

void C_3DObject2::build_bounding_cube()
{
  bcube.reset();
  for (int i=0;i<nshapes;i++)
    {
      shapes[i].getBounds(bcubes[i]);
      bcube.set(bcubes[i]);
    }
}

void C_3DObject2::translate(R_3DPoint &r)
{
  w_origin = r + world_origin;
  for (int i=0;i<nshapes;i++)
    {
      shapes[i].set_world_location(w_origin);
      if (shapes[i].flags & SHADOW)
	{
	  Vector sh_l = world_light_source + Vector(w_origin);
	  shapes[i].set_shadow(w_origin.z,sh_l);
	}
    }
  build_bounding_cube();
}

REAL_TYPE C_3DObject2::draw_prep(Port_3D &port)
{
  drawn_flag = 0;
  R_3DPoint port_local;

  if (!visible)
    {
      visible_flag = 0;
      return -1;
    }
  port.world2port(w_origin,&port_local);
  z_order = port_local.z;

  visible_flag = isVisible(port,bcube);
  if (visible_flag && port_local.z >= 0)
    {
      calc_screen_bounds(port,&port_local);
      if (max_sc <= 1.0)
	visible_flag = 0;
    }

  if (visible_flag)
    {
      R_3DPoint p = R_3DPoint(bcube.min_x,bcube.min_y,bcube.min_z);
      z_order = distance_squared(port.look_from,p);
      ZMIN(bcube.min_x,bcube.min_y,bcube.max_z,port,z_order);
      ZMIN(bcube.min_x,bcube.max_y,bcube.min_z,port,z_order);
      ZMIN(bcube.min_x,bcube.max_y,bcube.max_z,port,z_order);
      ZMIN(bcube.max_x,bcube.min_y,bcube.min_z,port,z_order);
      ZMIN(bcube.max_x,bcube.min_y,bcube.max_z,port,z_order);
      ZMIN(bcube.max_x,bcube.max_y,bcube.min_z,port,z_order);
      ZMIN(bcube.max_x,bcube.max_y,bcube.max_z,port,z_order);
      z_order = sqrt(z_order);
    }
  return z_order;
}


void C_3DObject2::draw(Port_3D &port)
{
  int i;
  int t;
  int tflg;

  if (!shapes)
    return;
  for (i=0;i<nshapes;i++)
    shapes[i].set_visible(port);
  t = TSTPS;

  if (z_order > C_3DObject_Base::dab_min + bounding_sphere)
    TSTPS = 1;
  else
    TSTPS = 0;

  tflg = C_Poly::txtrflag;
  C_Poly::txtrflag = txtr_flag;

  for (i=0;i<nshapes;i++)
    {
      if (shapes[i].visible)
	{
	  if (shapes[i].flags & SHADOW)
	    shapes[i].draw_shadow(port);
	  shapes[i].draw(port,w_origin);
	}
    }

  C_Poly::txtrflag = tflg;
  TSTPS = t;
  drawn_flag  = 1;
}

int C_3DObject2::ouch(const R_3DPoint &p1, const R_3DPoint &p2, int damage, Target *tg)
{
  if (!visible || isHistory() || damage < threshold_damage)
    return 0;
  int result = 0;
  if (inbounds(p1,p2,bcube))
    {
      for (int i=0;i<nshapes;i++)
	{
	  if (inbounds(p1,p2,bcubes[i]))
	    {
	      result = 1;
	      who_got_me = tg;
	      hurt += damage;
	      hit_shape = i;
	      if (isHistory())
		visible = 0;
	      break;
	    }
	}
    }
  return result;
}

int C_3DObject2::ouch(const R_3DPoint &p, int damage, Target *tg)
{
  if (isHistory() || damage < threshold_damage)
    return 0;
  int result = 0;
  if (inbounds(p,bcube))
    {
      for (int i=0;i<nshapes;i++)
	{
	  if (inbounds(p,bcubes[i]))
	    {
	      result = 1;
	      hurt += damage;
	      hit_shape = i;
	      who_got_me = tg;
	      if (isHistory())
		visible = 0;
	      break;
	    }
	}
    }
  return result;
}

int C_3DObject2::ouch(const R_3DPoint &p, float radius, int damage, Target *tg)
{
  if (!visible || isHistory() || (damage < threshold_damage))
    return 0;

  int dmg;
  int result = 0;
  if (calc_radial_damage(p,radius,damage,bcube,dmg))
    {
      if (dmg > threshold_damage)
	{
	  result = 1;
	  hurt += dmg;
	  who_got_me = tg;
	  if (isHistory())
	    visible = 0;
	  hit_shape = RANDOM(nshapes);
	}
    }
  return (result);
}

R_3DPoint *C_3DObject2::get_hit_point()
{
  if (hit_shape >= 0 && hit_shape < nshapes)
    {
      C_Shape & cs = shapes[hit_shape];
      C_Poly & cp = cs.polys[RANDOM(cs.npolys)];
      cp.getWorldPoint(hit_point,RANDOM(cp.p_info->npoints),w_origin);
    }
  else
    hit_point = w_origin;
  return (&hit_point);
}

R_3DPoint *C_3DObject2::get_position()
{
  return (&w_origin);
}

/***************************************************************
 * C_DrawList2 Methods                                         *
 ***************************************************************/

C_DrawList2::~C_DrawList2()
{
  if (objects)
    delete [] objects;
  if (groups)
    delete [] groups;
}

void C_DrawList2::read(std::istream &is)
{
  int i,j,idx,n;
  char c;

  shape_cnt = poly_cnt = 0;
  file_context = "C_DrawList2";
  READ_TOKI('(',is,c)
    is >> n_objects >> n_groups;
  READ_TOK(')',is,c)
   objects = new C_3DObject2[n_objects];
  for (i=0;i<n_objects;i++)
    objects[i].im = im;
  groups = new C_3DObject_Group[n_groups];
  MYCHECK(groups != NULL);
  idx = 0;
  for (i=0;(i<n_groups) && (idx<n_objects);i++)
    {
      READ_TOKI('{',is,c);
      is >> n;
      for (j=0;j<n;j++)
	{
	  is >> objects[idx];
	  groups[i].addObject(&objects[idx]);
	  idx++;
	  if (idx >= n_objects)
	    break;
	}
      READ_TOKI('}',is,c);
    }
}

std::istream &operator >>(std::istream &is, C_DrawList2 &cd)
{
  cd.read(is);
  return is;
}

void C_DrawList2::translate(R_3DPoint &r)
{
  for (int i=0;i<n_objects;i++)
    objects[i].translate(r);
}

void C_DrawList2::get_targets(Target_List &tlist)
{
  for (int i=0;i<n_objects;i++)
    if (objects[i].flags & HIT_ME)
      tlist.add_target(&objects[i]);
}

void C_DrawList2::add_draw_list(DrawList &dlist, Port_3D &port)
{
  for (int i=0;i<n_groups;i++)
    groups[i].addDrawList(dlist,port);
}

void C_DrawList2::setTextrFlag(int flag, char *id, int id_len)
{
  for (int i=0;i<n_objects;i++)
    {
      if (id != NULL)
	{
	  if (!memcmp(objects[i].obj_id,id,id_len))
	    objects[i].txtr_flag = flag;
	}
      else
	objects[i].txtr_flag = flag;
    }
}

void C_DrawList2::setVisibleFlag(int flag, char *id, int id_len)
{
  for (int i=0;i<n_objects;i++)
    {
      if (id != NULL)
	{
	  if (!memcmp(objects[i].obj_id,id,id_len))
	    objects[i].visible = flag;
	}
      else
	objects[i].visible = flag;
    }
}

