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
 *           SABRE Fighter Plane Simulator       *
 * Version: 0.1                                  *
 * File   : weapons.C                            *
 * Date   : March, 1997                          *
 *          October, 1997                        *
 * Author : Dan Hammer                           *
 *************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <limits.h>
#include <values.h>
#include "defs.h"
#include "sim.h"
#include "simfilex.h"
#include "vmath.h"
#include "port_3d.h"
#include "cpoly.h"
#include "copoly.h"
#include "colorspc.h"
#include "flight.h"
#include "fltlite.h"
#include "zview.h"
#include "unguided.h"
#include "weapons.h"

Weapons_Manager *Weapons_Manager::the_weapons_manager = NULL;

/***************************************************
 * Weapon_Specs methods                            *
 ***************************************************/
void Weapon_Specs::read(std::istream &is)
{
  is >> wep_name;
  is >> flt_specs;
  is >> min_range;
  is >> max_range;
  is >> damage_factor;
}

void Weapon_Specs::write(std::ostream &os)
{
  os << wep_name << '\n';
  os << flt_specs << '\n';
  os << min_range << ' ' << max_range << ' ' << damage_factor << '\n';
}

/***************************************************
 * Gun_Specs methods                               *
 ***************************************************/
void Gun_Specs::read(std::istream &is)
{
  int clr,dummy;
  char c =  ' ';
  READ_TOK('(',is,c);
  Weapon_Specs::read(is);
  is >> rounds_per_second;
  READ_TOKI('(',is,c);
  is >> clr >> dummy >> dummy >> dummy;
  READ_TOK(')',is,c);
  tracer_color = clr;
  is >> tracer_freq;
  is >> max_rounds >> c;
  READ_TOK(')',is,c);
  rounds_per_second = C(1.0 / (rounds_per_second / 60));
}

void Gun_Specs::write(std::ostream &os)
{
  color_spec cs(tracer_color);
  os << '(' << '\n';
  Weapon_Specs::write(os);
  os << rounds_per_second << ' ' << cs << '\n';
  os << tracer_freq << ' ' << max_rounds << '\n';
  os << ')' << '\n';
}

/***************************************************
 * Cannon_Specs methods                            *
 ***************************************************/
void Cannon_Specs::read(std::istream &is)
{
  char c;
  int clr;
  int dummy;
  READ_TOKI('(',is,c);
  Weapon_Specs::read(is);
  is >> rounds_per_second;
  READ_TOKI('(',is,c);
  is >> clr >> dummy >> dummy >> dummy;
  READ_TOK(')',is,c);
  tracer_color = clr;
  is >> tracer_freq;
  is >> max_rounds;

  is >> blast_radius;
  is >> blast_map;
  is >> smoke_map;
  is >> blast_map_size;
  is >> blast_dur;

  READ_TOKI(')',is,c);

  blast_radius *= world_scale;
  blast_map_size *= world_scale;
  rounds_per_second = C(1.0 / (rounds_per_second / 60));
}

void Cannon_Specs::write(std::ostream &os)
{
  color_spec cs(tracer_color);
  os << '(' << '\n';
  Weapon_Specs::write(os);
  os << rounds_per_second << ' ' << cs << '\n';
  os << tracer_freq << ' ' << max_rounds << '\n';

  os << blast_radius / world_scale << '\n';
  os << blast_map << '\n';
  os << smoke_map << '\n';
  os << blast_map_size / world_scale << '\n';
  os << blast_dur << '\n';
  os << ')' << '\n';
}

/***************************************************
 * Bomb_Specs methods                              *
 ***************************************************/
Bomb_Specs::~Bomb_Specs()
{
  if (zm != NULL)
    delete zm;
}

void Bomb_Specs::createZManager(char *path)
{
  if (zm != NULL)
    delete zm;
  zm = new Z_Node_Manager();
  MYCHECK(zm != NULL);
  zm->read_file(path);
}

void Bomb_Specs::read(std::istream &is)
{
  char c;
  READ_TOKI('(',is,c);
  Weapon_Specs::read(is);
  is >> shape_file;
  is >> blast_radius;
  is >> blast_map;
  is >> smoke_map;
  is >> blast_map_size;
  is >> blast_dur;
  READ_TOKI(')',is,c);
  createZManager(shape_file);
  blast_radius *= world_scale;
  blast_map_size *= world_scale;
}

void Bomb_Specs::write(std::ostream &os)
{
  os << '(' << '\n';
  Weapon_Specs::write(os);
  os << shape_file << '\n';
  os << blast_radius / world_scale << '\n';
  os << blast_map << '\n';
  os << smoke_map << '\n';
  os << blast_map_size / world_scale << '\n';
  os << blast_dur << '\n';
  os << ')' << '\n';
}

/***************************************************
 * Rocket_Specs methods                            *
 ***************************************************/
void Rocket_Specs::read(std::istream &is)
{
  char c;
  READ_TOKI('(',is,c);
  Weapon_Specs::read(is);
  is >> shape_file;
  is >> blast_radius;
  is >> blast_map;
  is >> smoke_map;
  is >> blast_map_size;
  is >> blast_dur;
  is >> flume_origin;
  is >> flume_initl_size;
  is >> flume_delta_size;
  is >> flume_flame_map;
  is >> flume_smoke_map;
  READ_TOKI(')',is,c);
  createZManager(shape_file);
  blast_radius *= world_scale;
  blast_map_size *= world_scale;
  flume_origin *= zm->scaler * world_scale;
  flume_initl_size *= world_scale;
  flume_delta_size *= world_scale;
}

void Rocket_Specs::write(std::ostream &os)
{
  os << '(' << '\n';
  Weapon_Specs::write(os);
  os << shape_file << '\n';
  os << blast_radius / world_scale << '\n';
  os << blast_map << '\n';
  os << smoke_map << '\n';
  os << blast_map_size / world_scale << '\n';
  os << blast_dur << '\n';
  os << flume_origin << ' ' << flume_initl_size << ' ' 
     << flume_delta_size << '\n';
  os << flume_flame_map << '\n';
  os << flume_smoke_map << '\n';
  os << ')' << '\n';
}

/*****************************************************
 * Weapon methods                                    *
 *****************************************************/

Weapon::Weapon()
{
  strcpy(id,"NONE");
  positions = NULL;
  vectors = NULL;
  host_flight = NULL;
  n = 0;
  idx = 0;
  w_specs = NULL;
  wep_i = NULL;
}

Weapon::~Weapon()
{
  if (positions)
    delete [] positions;
  if (vectors)
    delete [] vectors;
}

Weapon::Weapon(Weapon_Specs *ws)
{
  w_specs = ws;
  strcpy(id,"NONE");
  positions = NULL;
  vectors = NULL;
  host_flight = NULL;
  n = 0;
  idx = 0;
  wep_i = NULL;
}

void Weapon::read(std::istream &is)
{
  char c;
  READ_TOKI('{',is,c);
  is >> id;
  is >> aim_position;
  aim_position *= world_scale;
  is >> aim_vector;
  is >> n;
  if (n > 0)
    {
      positions = new R_3DPoint[n];
      MYCHECK(positions != NULL);
      vectors = new Vector[n];
      MYCHECK(vectors != NULL);
      for (int i=0;i<n;i++)
	{
	  is >> positions[i] >> vectors[i];
	  positions[i] *= world_scale;
	}
    }
  READ_TOK('}',is,c);
}

void Weapon::write(std::ostream &)
{

}

void Weapon::get_launch_params(Launch_Params &lp)
{
	lp.specs = w_specs;
	lp.launcher = NULL;
	lp.departure_point = positions[idx];
	lp.departure_vector = vectors[idx];
	lp.fuse_type = f_impact;
	lp.fuse_data = 0.0;
	lp.launcher_velocity = host_flight->state.velocity;
	lp.flight = host_flight;
	lp.port = host_flight->state.flight_port;
	lp.hitScaler = wep_i->hitScaler;

	if (wep_i->flags & WP_SETVIEW)
	{
		lp.port.set_view(lp.port.look_from,wep_i->target_position);
		lp.target_position = wep_i->target_position;
		Vector v = Vector(lp.target_position - lp.port.look_from);
		v.Normalize();
		lp.launcher_velocity.direction = DVector(v);
	}
	else
		lp.target_position = impact_point;
	lp.flags = LP_PORT;
}

R_3DPoint Weapon::predict_path (Flight &hf, float flt_time)
{
  float x,y,z;
  host_flight = &hf;
  // Use FlightLite object to get initial parameters
  FlightLight flt(w_specs->getFltSpecs());
  flt.activate(host_flight->state.flight_port,
	       w_specs->getFltSpecs(),
	       aim_position,
	       aim_vector,
	       &host_flight->state.velocity
	       );
  DVector hv = to_vector(flt.state.velocity);
  float v_vel = hv.Z;

  hv.Z = 0.0;
  hv *= flt_time;
  hv *= world_scale;
  // Get final x,y coords
  x = hv.X + flt.state.flight_port.look_from.x;
  y = hv.Y + flt.state.flight_port.look_from.y;

  // Find vertical distance travelled
  // s = v0t + 1/2at2
  float sy = (v_vel * flt_time ) +
    ( -g / 2.0f) * (flt_time * flt_time);
  // scale it
  sy *= world_scale;
  // get vertical position
  z = flt.state.flight_port.look_from.z + sy;
  impact_point = R_3DPoint(x,y,z);
  return (R_3DPoint(x,y,z));
}

REAL_TYPE Weapon::calc_htime(Flight &hf, R_3DPoint &p)
{
  R_KEY_BEGIN(402)
    host_flight = &hf;
  float result;
  float init_speed = w_specs->flt_specs.init_speed;
  Vector v = Vector(p - host_flight->state.flight_port.look_from);
  v.Z = 0.0;
  float d = (float) sqrt(v.X * v.X + v.Y * v.Y);
  d /= world_scale;
  result = d / init_speed;
  R_KEY_END
    return result;
}

REAL_TYPE Weapon::calc_hvtime(Flight &host_flit, R_3DPoint &p)
{
  REAL_TYPE hdist;
  REAL_TYPE htime;
  Vector v = Vector(p - host_flit.state.flight_port.look_from);
  hdist = C(sqrt(v.X * v.X + v.Y * v.Y) / world_scale);

  FlightLight flt(&w_specs->flt_specs);
  flt.activate(host_flit.state.flight_port,
	       &w_specs->flt_specs,
	       aim_position,
	       aim_vector,
	       &host_flit.state.velocity
	       );

  DVector hv = to_vector(flt.state.velocity);

  htime = C(hdist / sqrt(hv.X * hv.X + hv.Y * hv.Y));
  return (htime);
}

/*****************************************************
 * Guns methods                                      *
 *****************************************************/
std::istream & operator >>(std::istream &is, Guns &gp)
{
  ((Weapon &)gp).read(is);
  return is;
}

unsigned int Guns::getMaxRounds()
{
  if (gun_specs)
    return (gun_specs->max_rounds);
  else
    return (1000);
}


int Guns::update(Flight &hf, Unguided_Manager *um, Weapon_Instance *wi, 
		 Target *launcher)
{
  int result = 0;
  host_flight = &hf;
  Launch_Params lp;
  wep_i = wi;

  if (wep_i->elapsed_time >= gun_specs->rounds_per_second)
    {
      for (idx=0;idx<n;idx++)
	{
	  get_launch_params(lp);
	  lp.launcher = launcher;
	  if ( ((wep_i->rounds_remaining / n) % gun_specs->tracer_freq) == 0)
	    lp.flags |= LP_TRACER;
	  if (um->new_round(lp))
	    {
	      result++;
	      wep_i->rounds_remaining--;
	    }
	}
    }
  return result;
}

/********************************************************
 * Bombs methods                                        *
 ********************************************************/
std::istream & operator >>(std::istream &is, Bombs &b)
{
  ((Weapon &)b).read(is);
  return is;
}

int Bombs::update(Flight &hf, Unguided_Manager *um, Weapon_Instance *wi, 
		  Target *launcher)
{
  int result = 0;
  Launch_Params lp;

  host_flight = &hf;
  wep_i = wi;
  for (int i=0;i<wep_i->rounds_per_launch;i++)
    {
      if (wi->launch_idx >= n)
	break;
      idx = wep_i->launch_idx;
      get_launch_params(lp);
      lp.flags |= LP_TRACER;
      lp.launcher = launcher;
      if (um->new_round(lp))
	{
	  result++;
	  wep_i->launch_idx++;
	  wep_i->rounds_remaining--;
	}
    }
  return result;
}

/********************************************************
 * Rockets methods                                        *
 ********************************************************/
std::istream & operator >>(std::istream &is, Rockets &r)
{
  ((Weapon &)r).read(is);
  return is;
}

int Rockets::update(Flight &hf, Unguided_Manager *um, 
		    Weapon_Instance *wi, 
		    Target *launcher)
{
  int result = 0;
  host_flight = &hf;
  Launch_Params lp;
  wep_i = wi;
  for (int i=0;i<wep_i->rounds_per_launch;i++)
    {
      if (wep_i->launch_idx >= n)
	break;
      idx = wep_i->launch_idx;
      get_launch_params(lp);
      lp.launcher = launcher;
      lp.flags |= LP_TRACER;

      if (um->new_round(lp))
	{
	  result++;
	  wep_i->launch_idx++;
	  wep_i->rounds_remaining--;
	}
    }
  return result;
}

/********************************************************
 * Missiles methods                                     *
 ********************************************************/
std::istream & operator >>(std::istream &is, Missiles &r)
{
  ((Weapon &)r).read(is);
  return is;
}

int Missiles::update(Flight &hf, Unguided_Manager *um, 
		     Weapon_Instance *wi, 
		     Target *launcher)
{
  int result = 0;
  host_flight = &hf;
  Launch_Params lp;
  wep_i = wi;
  for (int i=0;i<wep_i->rounds_per_launch;i++)
    {
      if (wep_i->launch_idx >= n)
	break;
      idx = wep_i->launch_idx;
      get_launch_params(lp);
      lp.launcher = launcher;
      lp.flags |= LP_TRACER;
      if (um->new_round(lp))
	{
	  result++;
	  wep_i->launch_idx++;
	  wep_i->rounds_remaining--;
	}
    }
  return result;
}

/********************************************************
 * FuelTanks methods                                     *
 ********************************************************/
std::istream & operator >>(std::istream &is, FuelTanks &r)
{
  ((Weapon &)r).read(is);
  return is;
}

int FuelTanks::update(Flight &hf, Unguided_Manager *um, 
		     Weapon_Instance *wi, 
		     Target *launcher)
{
  int result = 0;
  host_flight = &hf;
  Launch_Params lp;
  wep_i = wi;
  for (int i=0;i<wep_i->rounds_per_launch;i++)
    {
      if (wep_i->launch_idx >= n)
	break;
      idx = wep_i->launch_idx;
      get_launch_params(lp);
      lp.launcher = launcher;
      lp.flags |= LP_TRACER;
      if (um->new_round(lp))
	{
	  result++;
	  wep_i->launch_idx++;
	  wep_i->rounds_remaining--;
	}
    }
  return result;
}

/********************************************************
 * Weapon_Instance methods                              *
 ********************************************************/
Weapon_Instance::~Weapon_Instance()
{
  if (viewers != NULL)
    delete [] viewers;
  if (ports != NULL)
    delete [] ports;
}

void Weapon_Instance::set_weapon(Weapon *wp)
{
  viewers = NULL;
  ports = NULL;
  weapon = wp;
  n = weapon->n;
  if (viewers != NULL)
    delete viewers;
  rounds_remaining = weapon->getMaxRounds();
  Z_Node_Manager *zm = weapon->w_specs->getZNodeMgr();
  if (zm != NULL)
    {
      viewers = new Z_Viewer[n];
      MYCHECK(viewers != NULL);
      ports = new Port_3D[n];
      MYCHECK(ports != NULL);
      for (int i=0;i<n;i++)
	{
	  viewers[i].setManager(zm);
	  viewers[i].setReferencePort(&ports[i]);
	}
    }
}

int Weapon_Instance::update(Flight &host_flight, 
			    Unguided_Manager *um, 
			    Target *launcher)
{
  int result = 0;
  if (rounds_remaining <= 0)
    return 0;
  elapsed_time += host_flight.l_time;
  if (host_flight.controls.bang_bang)
    {
      result = weapon->update(host_flight,um,this,launcher);
      if (result)
	elapsed_time = 0.0;
      host_flight.controls.bang_bang = 0;
    }
  if (result)
    flags = 0;
  return result;
}

REAL_TYPE Weapon_Instance::draw_prep(Port_3D *port, Port_3D *ref_port)
{
  REAL_TYPE result = 0.0;
  R_3DPoint w0,w1;
  R_3DPoint p1;
  Vector v0;
  if (viewers)
    {
      for (int i=launch_idx;i<n;i++)
	{
	  v0 = weapon->vectors[i];
	  v0.Normalize();
	  p1 = weapon->positions[i] + R_3DPoint(v0);
	  ref_port->port2world(weapon->positions[i],&w0);
	  ref_port->port2world(p1,&w1);
	  ports[i] = *ref_port;
	  ports[i].set_view(w0,w1);
	  result = viewers[i].draw_prep(*port);
	}
    }
  return (result);
}

void Weapon_Instance::draw(Port_3D *port)
{
  if (viewers)
    {
      for (int i=launch_idx;i<n;i++)
	viewers[i].draw(*port);
    }
}

bool Weapon_Instance::isVisible()
{
  if (viewers)
    {
      for (int i=launch_idx;i<n;i++)
	if (viewers[i].visible_flag)
	  return(1);
    }
  return (0);
}

/********************************************************
 * Weapon_List methods                                  *
 ********************************************************/
Weapon_List::~Weapon_List()
{
  if (weapons != NULL)
    {
      if (del_flag)
	{
	  for (int i=0;i<n;i++)
	    if (weapons[i])
	      delete weapons[i];
	}
    }
  delete [] weapons;
}

Weapon_List::Weapon_List(int nweapons, int df)
{
  del_flag = df;
  create_list(nweapons);
  idx = 0;
}

int Weapon_List::create_list(int sz)
{
  n = sz;
  weapons = new Weapon *[n];
  MYCHECK(weapons != NULL);
  for (int i=0;i<n;i++)
    weapons[i] = NULL;
  return (weapons != NULL);
}

int Weapon_List::add_weapon(Weapon *wp)
{
  if (idx < n)
    {
      weapons[idx++] = wp;
      return 1;
    }
  else
    return 0;
}

Weapon *Weapon_List::get_weapon(int i)
{
  if (i >= 0 && i < n)
    return weapons[i];
  else
    return NULL;
}

Weapons_Manager::Weapons_Manager()
{
  wep_specs = NULL;
  master_list = NULL;
  lists = NULL;
}

Weapons_Manager::~Weapons_Manager()
{
  unsigned int i;
  if (wep_specs)
    {
      for (i=0;i<nspecs;i++)
	delete wep_specs[i];
      delete wep_specs;
    }
  if (master_list)
    delete master_list;
  if (lists)
    delete [] lists;
}

Weapon_Instance *Weapons_Manager::build_instance_list(unsigned int n, unsigned int *cnt, char *id)
{
  Weapon_Instance *result = NULL;
  Weapon_List *wl;
  if (id != NULL)
    wl = get_list(id);
  else
    wl = get_list(n);
  if (wl)
    {
      result = new Weapon_Instance[wl->n];
      MYCHECK(result != NULL);
      for (unsigned int i=0;i<wl->n;i++)
	{
	  MYCHECK(wl->weapons[i] != NULL);
	  result[i].set_weapon(wl->weapons[i]);
	}
      *cnt = wl->n;
    }
  else if (id != NULL)
    printf("Failed to find weapons list %s\n",id);
  return result;
}

Weapon_Instance_List *Weapons_Manager::build_instance_list(unsigned int n)
{
  unsigned int cnt;
  Weapon_Instance *wi = build_instance_list(n,&cnt);
  return (new Weapon_Instance_List(wi,cnt));
}

Weapon_List *Weapons_Manager::get_list(char *id)
{
  Weapon_List *result = NULL;
  for (int i=0;i<nlists;i++)
    if (!strcmp(id,lists[i].id))
      {
	result = &lists[i];
	break;
      }
  return(result);
}

Weapon_List *Weapons_Manager::get_list(int n)
{
  Weapon_List *result = NULL;
  if (n >= 0 && n < nlists)
    result = &lists[n];
  return result;
}

void Weapons_Manager::read_file(const char *path)
{
  std::ifstream infile;
  if (open_is(infile,path))
    infile >> *this;
  infile.close();
}

#define NWPTYPES 6
static simfileX::dict wptypes[NWPTYPES] =
{
  { "gun_type", gun_t },
  { "rocket_type", rocket_t },
  { "bomb_type", bomb_t },
  { "missile_type", missile_t },
  { "fueltank_type",fueltank_t },
  { "cannon_type",cannon_t }
};

void Weapons_Manager::read(std::istream &is)
{
  int i,j,ndx,ndx2;
  int wep_type,sz;
  char buff[64];
  Weapon_Specs *wsp;

  char c =  ' ';

  READ_TOKI('{',is,c);
  // first come the specs
  is >> nspecs;
  MYCHECK(nspecs > 0 && nspecs < 50);
  wep_specs = new Weapon_Specs *[nspecs];
  MYCHECK(wep_specs != NULL);

  READ_TOKI('{',is,c);
  for (i=0;i<nspecs;i++)
    {
      simfileX::readdictinput(is,buff,sizeof(buff),wep_type,wptypes,NWPTYPES);
      MYCHECK(wep_type >= ((int)gun_t) && wep_type <= ((int)cannon_t));
 
      switch (wep_type)
	{
	case bomb_t:
	  wep_specs[i] = new Bomb_Specs();
	  is >> *((Bomb_Specs *)wep_specs[i]);
	  wep_specs[i]->wep_type = bomb_t;
	  break;

	case cannon_t:
	  wep_specs[i] = new Cannon_Specs();
	  is >> *((Cannon_Specs *)wep_specs[i]);
	  wep_specs[i]->wep_type = cannon_t;
	  break;

	case rocket_t:
	  wep_specs[i] = new Rocket_Specs();
	  is >> *((Rocket_Specs *)wep_specs[i]);
	  wep_specs[i]->wep_type = rocket_t;
	  break;

	case missile_t:
	  wep_specs[i] = new Missile_Specs();
	  is >> *((Bomb_Specs *)wep_specs[i]);
	  wep_specs[i]->wep_type = missile_t;
	  break;

	case gun_t:
	  wep_specs[i] = new Gun_Specs();
	  is >> *((Gun_Specs *)wep_specs[i]);
	  wep_specs[i]->wep_type = gun_t;
	  break;

	case fueltank_t:
	  wep_specs[i] = new FuelTank_Specs();
	  is >> *((Bomb_Specs *)wep_specs[i]);
	  wep_specs[i]->wep_type = fueltank_t;
	  break;
	}
    }
  READ_TOK('}',is,c);

  // Build master list
  is >> nweps;
  MYCHECK(nweps > 0);
  master_list = new Weapon_List(nweps,1);
  MYCHECK(master_list != NULL);

  READ_TOKI('{',is,c);
  for (i=0;i<nweps;i++)
    {
      if (simfileX::readinput(is,buff,sizeof(buff),ndx2) == 
	  simfileX::STR_INPUT)
	wsp = getSpecs(buff);
      else
	{
	  MYCHECK(ndx2 >= 0 && ndx2 < nspecs);
	  wsp = wep_specs[ndx2];
	}
      MYCHECK(wsp != NULL);
      switch (wsp->wep_type)
	{

	case gun_t:
	case cannon_t:
	  {
	    Guns *gg = new Guns((Gun_Specs *) wsp);
	    MYCHECK(gg != NULL);
	    is >> *gg;
	    master_list->add_weapon(gg);
	    break;
	  }
	
	case bomb_t:
	  {
	    Bombs *bb = new Bombs((Bomb_Specs *) wsp);
	    MYCHECK(bb != NULL);
	    is >> *bb;
	    master_list->add_weapon(bb);
	    break;
	  }
	
	case rocket_t:
	  {
	    Rockets *rr = new Rockets((Rocket_Specs *)wsp);
	    MYCHECK(rr != NULL);
	    is >> *rr;
	    master_list->add_weapon(rr);
	    break;
	  }

	case missile_t:
	  {
	    Missiles *mm = new Missiles((Missile_Specs *)wsp);
	    MYCHECK(mm != NULL);
	    is >> *mm;
	    master_list->add_weapon(mm);
	    break;
	  }

	case fueltank_t:
	  {
	    FuelTanks *ft = new FuelTanks((FuelTank_Specs *)wsp);
	    MYCHECK(ft != NULL);
	    is >> *ft;
	    master_list->add_weapon(ft);
	    break;
	  }
	}
    }
  READ_TOK('}',is,c);

  // Build lists of weapons
  is >> nlists;
  MYCHECK(nlists > 0);
  lists = new Weapon_List[nlists];
  MYCHECK(lists != NULL);

  READ_TOKI('{',is,c);
  for (i=0;i<nlists;i++)
    {
      is >> sz;
      is >> lists[i].id;
      lists[i].create_list(sz);
      READ_TOKI('[',is,c);
      for (j=0;j<sz;j++)
	{
	  Weapon *wpp;
	  if (simfileX::readinput(is,buff,sizeof(buff),ndx) == 
	      simfileX::STR_INPUT)
	    wpp = getWeapon(buff);
	  else
	    {
	      MYCHECK(ndx >= 0 && ndx < nweps );
	      wpp = master_list->get_weapon(ndx);
	    }
	  MYCHECK(wpp != NULL);
	  lists[i].add_weapon(wpp);
	}
      READ_TOKI(']',is,c);
    }
  READ_TOK('}',is,c);

  READ_TOKI('}',is,c);
}

std::istream & operator >>(std::istream &is, Weapons_Manager &wp)
{
  wp.read(is);
  return is;
}

Weapon_Specs *Weapons_Manager::getSpecs(char *spec_name)
{
  Weapon_Specs *result = NULL;
  for (int i=0;i<nspecs;i++)
    if (!strcmp(wep_specs[i]->wep_name,spec_name))
      {
	result = wep_specs[i];
	break;
      }

  return (result);
}

Weapon *Weapons_Manager::getWeapon(char *wep_id)
{
  Weapon *result = NULL;
  Weapon *wp = NULL;
  for (int i=0;i<nweps;i++)
    {
      wp = master_list->get_weapon(i);
      if (wp && !strcmp(wp->id,wep_id))
	{
	  result = wp;
	  break;
	}
    }
  return(result);
}

/***************************************************
 * Weapon_Instance_List methods                    *
 ***************************************************/
Weapon_Instance_List::Weapon_Instance_List(Weapon_Instance *wi, unsigned int n)
    :n_weaps(n),
     sel_wpn(0),
     weapons(wi)
{
  has_externs = 1;
  for (unsigned int i=0;i<n_weaps;i++)
    {
      if (weapons[i].hasExterns())
	{
	  has_externs = 1;
	  break;
	}
    }
}

void Weapon_Instance_List::copy(const Weapon_Instance_List &wpl)
{
  if (weapons)
    delete [] weapons;
  n_weaps = wpl.n_weaps;
  sel_wpn = wpl.sel_wpn;
  weapons = new Weapon_Instance[n_weaps];
  for (int i=0;i<n_weaps;i++)
    weapons[i] = wpl.weapons[i];
  has_externs = wpl.has_externs;
}

void Weapon_Instance_List::draw_prep(Port_3D *port, Port_3D *ref_port)
{
  for (int i=0;i<n_weaps;i++)
    weapons[i].draw_prep(port,ref_port);
}

bool Weapon_Instance_List::isVisible()
{
  for (int i=0;i<n_weaps;i++)
    if (weapons[i].isVisible())
      return(1);
  return(0);
}

void Weapon_Instance_List::draw(Port_3D *port)
{
  for (int i=0;i<n_weaps;i++)
    weapons[i].draw(port);
}

Weapon_Specs::~Weapon_Specs() = default;
Rocket_Specs::~Rocket_Specs() = default;
Cannon_Specs::~Cannon_Specs() = default;
Missile_Specs::~Missile_Specs() = default;
Gun_Specs::~Gun_Specs() = default;
FuelTank_Specs::~FuelTank_Specs() = default;
