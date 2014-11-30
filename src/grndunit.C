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
 * File   : grndunit.C                           *
 * Date   : Sept, 1997                           *
 * Author : Dan Hammer                           *
 *************************************************/
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <values.h>
#include "defs.h"
#include "sim.h"
#include "simerr.h"
#include "vmath.h"
#include "port_3d.h"
#include "cpoly.h"
#include "obj_3d.h"
#include "copoly.h"
#include "flight.h"
#include "fltlite.h"
#include "weapons.h"
#include "pilot.h"
#include "zview.h"
#include "fltzview.h"
#include "unguided.h"
#include "simfile.h"
#include "simsnd.h"
#include "grndunit.h"

extern REAL_TYPE getGroundLevel(R_3DPoint &p);

/*******************************************************
 * Ground_Unit_Specs methods                           *
 *******************************************************/
Ground_Unit_Specs::~Ground_Unit_Specs()
{
  if (zm != NULL)
    delete zm;
  /*
  if (specs_extra != NULL)
    delete specs_extra;
    */
}

void Ground_Unit_Specs::read(std::istream &is)
{
  char c;
  char specs_id[32];

  file_context = "Ground_Unit_Specs {";

  READ_TOKI('{',is,c)

    is >> type;
  memset(id,' ',sizeof(id));
  is >> id;
  id[sizeof(id) - 1] = '\0';

  memset(shape_file,' ',sizeof(shape_file));
  is >> shape_file;

  is >> update_time;
  is >> acquire_range;
  acquire_range = m2f(acquire_range);
  is >> max_speed;
  max_speed = mph2fps(max_speed);
  is >> avg_speed;
  avg_speed = mph2fps(avg_speed);
  is >> max_damage;
  is >> view_point;
  view_point *= world_scale;
  zm = new Z_Node_Manager();
  MYCHECK(zm != NULL);
  zm->read_file(shape_file);

  switch (type)
    {

    case AAA:
      is >> specs_id;
      specs_extra = 
	Weapons_Manager::the_weapons_manager->getSpecs(specs_id);
      MYCHECK(specs_extra != NULL);
      break;

    }
  file_context = "Ground_Unit_Specs }";
  READ_TOK('}',is,c);
}


/*******************************************************
 * Ground_Unit methods                                 *
 *******************************************************/
void Ground_Unit::init()
{
  Z_Viewer::init(specs->zm);
  setReferencePort(&ref_port);
  max_damage = specs->max_damage;
  setPosition();
}

Ground_Unit::~Ground_Unit()
{

}

void Ground_Unit::copy(const Ground_Unit &gu)
{
  specs = gu.specs;
  ref_port = static_cast<const Port_3D &>(gu.ref_port);
  current_speed = gu.current_speed;
  view = gu.view;
  affiliation = gu.affiliation;
  current_speed = gu.current_speed;
  init();
}

void Ground_Unit::update(Unguided_Manager *, Target_List &)
{
  if (!isHistory())
    updatePosition();
}

void Ground_Unit::updatePosition()
{
  if (current_speed > 0)
    {
      Vector v = ref_port.get_view_normal();
      v *= current_speed * time_frame * world_scale;
      v.Z = 0.0;
      ref_port.delta_look_from(v);
    }
  setPosition();
}

void Ground_Unit::setPosition()
{
  ref_port.look_from.z = getGroundLevel(ref_port.look_from);
  ref_port.look_at.z = getGroundLevel(ref_port.look_at);
  ref_port.set_view(ref_port.look_from,ref_port.look_at);
  Target::position = ref_port.look_from;
}

void Ground_Unit::add_draw_list(DrawList &dl, Port_3D &port)
{
  draw_prep(port);
  if (visible_flag)
    dl.add_object(this);
  draw_shadow = 1;
}

int Ground_Unit::ouch(const R_3DPoint &p1, const R_3DPoint &p2, 
		      int damage, Target *trg)
{

  if (isHistory() || damage < threshold_damage)
    return (0);
  R_3DPoint r1,r2;

  int result = 0;
  ref_port.world2port(p1,&r1);
  ref_port.world2port(p2,&r2);
  result = calc_hit(r1,r2,bcube);
  if (result)
    {
      hit_shape = 0;
      hurt += damage;
      who_got_me = trg;
    }
  return (result);
}

int Ground_Unit::ouch(const R_3DPoint &w0, float radius, 
		      int damage, Target *tg)
{
  if (isHistory() || damage < threshold_damage)
    return (0);
  R_3DPoint p0;
  int dmg;
  int result = 0;
  ref_port.world2port(w0,&p0);
  if (calc_radial_damage(p0, radius, damage, bcube, dmg))
    {
      if (dmg > threshold_damage)
	{
	  result = 1;
	  hit_shape = 0;
	  hurt += dmg;
	  who_got_me = tg;
	}
    }
  return (result);
}

R_3DPoint *Ground_Unit::get_hit_point()
{
  C_ShapeInfo &si = z_manager->shape_info[hit_shape];
  C_PolyInfo &p = si.polyinfos[RANDOM(si.npolys)];
  R_3DPoint &hp = p.lpoints[RANDOM(p.npoints)];
  ref_port.port2world(hp,&hitpoint);
  return (&hitpoint);
}

R_3DPoint *Ground_Unit::get_position()
{
  return (&position);
}

void Ground_Unit::read(std::istream &is)
{
  char c;
  file_context = "Ground_Unit";
  READ_TOKI('{',is,c);
  is >> affiliation;
  is >> view;
  is >> current_speed;
  current_speed = mph2fps(current_speed);
  is >> ref_port;
  ref_port.look_from.z = getGroundLevel(ref_port.look_from);
  ref_port.look_at.z = getGroundLevel(ref_port.look_at);
  ref_port.set_view(ref_port.look_from,ref_port.look_at);
  READ_TOK('}',is, c);
  init();
}

void Ground_Unit::write(std::ostream &os)
{
  os << "{\n";
  os << affiliation << view << '\n';
  os << fps2mph(current_speed) << '\n';
  os << "}\n";
}

std::istream &operator >>(std::istream &is, Ground_Unit &gu)
{
  gu.read(is);
  return(is);
}

std::ostream &operator <<(std::ostream &os, Ground_Unit &gu)
{
  gu.write(os);
  return(os);
}

/*******************************************************
 * Ground_Unit_Manager methods                         *
 *******************************************************/
Ground_Unit_Manager::Ground_Unit_Manager()
  : specs(NULL),
    units(NULL),
    wm()
{
  nspecs = nunits = 0;
}


Ground_Unit_Manager::~Ground_Unit_Manager()
{
  if (specs)
    delete [] specs;
  if (units)
    {
      for (unsigned int i=0;i<nunits;i++)
	if (units[i] != NULL)
	  delete units[i];
      delete [] units;
    }
}

void Ground_Unit_Manager::update(Unguided_Manager *um, Target_List &tl)
{
  for (unsigned int i=0;i<nunits;i++)
    if (units[i])
      units[i]->update(um,tl);
}

void Ground_Unit_Manager::add_draw_list(DrawList &dl, Port_3D &port)
{
  for (unsigned int i=0;i<nunits;i++)
    if (units[i])
      units[i]->add_draw_list(dl,port);
}

std::istream &operator >>(std::istream &is, Ground_Unit_Manager &gum)
{
  gum.read(is);
  return(is);
}

void Ground_Unit_Manager::read_file(const char *path)
{
  std::ifstream is;
  file_context = "Ground_Unit_Manager";
  if (open_is(is,path))
    read(is);
}

void Ground_Unit_Manager::read(std::istream &is)
{
  char c;
  unsigned int gu_idx;
  unsigned int gu_cnt;
  nspecs = read_uint(is);
  nunits = read_uint(is);
  specs = new Ground_Unit_Specs[nspecs];
  MYCHECK(specs != NULL);
  units = new Ground_Unit *[nunits];
  MYCHECK(units != NULL);
  gu_idx = 0;
  for (unsigned int i=0;i<nspecs;i++)
    {
      READ_TOKI('{',is,c);
      gu_cnt = read_uint(is);
      is >> specs[i];
      for (unsigned int j=0;j<gu_cnt;j++)
	{
	  if (gu_idx > nunits - 1)
	    return;
	  switch (specs[i].type)
	    {
	    case AAA:
	      {
		AAA_Unit *tmp = new AAA_Unit(&specs[i]);
		MYCHECK(tmp != NULL);
		is >> *tmp;
		units[gu_idx] = tmp;
	      }
	      break;

	    default:
	      {
		Ground_Unit *tmp = new Ground_Unit(&specs[i]);
		MYCHECK(tmp != NULL);
		is >> *tmp;
		units[gu_idx] = tmp;
	      }
	      break;
	      
	    }

	  gu_idx++; 
	}
      file_context = "Ground_Unit_Manager";
      READ_TOK('}',is,c);
    }  
}

std::ostream &operator >>(std::ostream &os, Ground_Unit_Manager &gum);
std::ostream &operator >>(std::ostream &os, Ground_Unit_Manager &gum)
{
  gum.write(os);
  return(os);
}

void Ground_Unit_Manager::write_file(const char *path)
{
  std::ofstream os;
  if (open_libos(os,path))
    write(os);
}

void Ground_Unit_Manager::write(std::ostream &)
{

}


Ground_Unit_Specs *Ground_Unit_Manager::getSpecs(char *sid)
{
  Ground_Unit_Specs *result = NULL;
  for (unsigned int i=0;i<nspecs;i++)
    {
      if (!strcmp(specs[i].id,sid))
	{
	  result = &specs[i];
	  break;
	}
    }
  return (result);
}

void Ground_Unit_Manager::addTargetList(Target_List &tl)
{
  for (unsigned int i=0;i<nunits;i++)
    if (units[i])
      tl.add_target(units[i]);
}
