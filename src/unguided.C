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
 * File   : unguided.C                           *
 * Date   : March, 1997                          *
 * Author : Dan Hammer                           *
 * Things that go boom                           *
 *************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <limits.h>
#include <values.h>
#include "defs.h"
#include "pc_keys.h"
#include "sim.h"
#include "vga_13.h"
#include "vmath.h"
#include "port_3d.h"
#include "cpoly.h"
#include "obj_3d.h"
#include "copoly.h"
#include "flight.h"
#include "fltzview.h"
#include "fltlite.h"
#include "colorspc.h"
#include "simfile.h"
#include "viewobj.h"
#include "rtkey.h"
#include "simsnd.h"
#include "weapons.h"
#include "txtrmap.h"
#include "grndlevl.h"
#include "simmath.h"
#include "unguided.h"

extern float vtable [][3];
extern int vsize;
static int debris_len = 6, debris_width = 6;
static float dustup_size;
static float dustup_time = 3.0;
//static int local_flag = 0;
//static long memlimit = 2000;

#define GRAY_COLORS 100

Unguided_Manager *Unguided_Manager::the_umanager = NULL;
Target          **Unguided_Manager::targets = NULL;
int               Unguided_Manager::ntargets = 0;
int               Unguided_Manager::inited = 0;

float BlastMap::bm_sizex = 1.0;
float BlastMap::bm_sizey = 1.0;
float BlastMap::bm_dur = 1.0;
const char *BlastMap::bm_map = "fire";
const char *BlastMap::bm_initmap = "fire";
float BlastMap::bm_initdur = 1.0;

int   Smokey::sm_n_smokes = 6;
float Smokey::sm_init_size = 10.0;
float Smokey::sm_d_size = 5.0;
float Smokey::sm_d_len = 5.0;
float Smokey::sm_kink = 48.0;
float Smokey::sm_max_time = 60.0;
int   Smokey::sm_has_flame = 1;
float Smokey::sm_flame_time_max = 15.0;
int   Smokey::sm_nflames = 3;
const char  *Smokey::sm_smokemap_id = "smoke";
const char  *Smokey::sm_firemap_id = "fire";

inline void RANDPOINT(R_3DPoint &p, float rval)
{
	float kk;
	kk = frand(rval);
	if (RANDOM(2) == 1)
		kk = -kk;
	p.x += kk;
	kk = frand(rval);
	if (RANDOM(2) == 1)
		kk = -kk;
	p.y += kk;
	kk = frand(rval);
	if (RANDOM(2) == 1)
		kk = -kk;
	p.z += kk;
}

/**********************************************
 * BlastMap Methods                           *
 **********************************************/
void BlastMap::init()
{
  initflag = 0;
  setDuration(bm_dur);
  map = map_man->get_map_ptr(bm_map);
  MYCHECK(map != NULL);
  if (bm_initmap != NULL)
    {
      initmap = map_man->get_map_ptr(bm_initmap);
      if (initmap != NULL)
	{
	  initflag = 1;
	  initdur = bm_initdur;
	  view = new Bitmap_View(initmap,&position,bm_sizex,bm_sizey);
	}
      else
	view = new Bitmap_View(map,&position,bm_sizex,bm_sizey);
    }
  else
    view = new Bitmap_View(map,&position,bm_sizex,bm_sizey);
  if (view)
    {
      ((Bitmap_View *)view)->loc = position;
      TMoveable::init();
      active = 1;
    }
  else
    active = 0;
}

void BlastMap::update()
{
  TMoveable::update();
  if (!active)
    return;
  if (initflag && elapsed_time >= initdur)
    {
      initflag = 0;
      if (view)
	((Bitmap_View *)view)->setMap(map);
    }
}

void BlastMap::add_draw_list(DrawList & dl, Port_3D & port)
{
  Moveable::add_draw_list(dl,port);
}

void BlastMap::de_activate()
{
  if (view)
    delete view;
  view = NULL;
  TMoveable::de_activate();
}

/*********************************************
 * DustUp Methods                            *
 *********************************************/
void DustUp::init()
{
  if (view == NULL)
    {
      TextrMap *dustmap = map_man->get_map_ptr("smoke");
      MYCHECK(dustmap != NULL);
      view = new Bitmap_View(dustmap,&position,
			     dustup_size,dustup_size);
    }
  if (view)
    {
      ((Bitmap_View *)view)->loc = position;
      max_time = dustup_time;
      TMoveable::init();
    }
  else
    active = 0;
}

void DustUp::de_activate()
{
  if (view)
    delete view;
  view = NULL;
  TMoveable::de_activate();
}

void Projectile::init()
{
  elapsed_time = 0.0;
  hit = NULL;
  rad_hits = 0;
  max_time = specs->time_limit;
  if (view != NULL)
    delete view;
  view = NULL;
  MYCHECK(flt == NULL);
  if (flt != NULL)
    {
      flt->pause();
      delete flt;
    }
  flt = new FlightLight(specs);
  if (flt)
    {
      active = 1;
      create_view();
      if (view == NULL)
	is_tracer = 0;
    }
  else
    active = 0;
}

void Projectile::create_view()
{
  if (view != NULL)
    delete view;
  if (wspecs)
    {
      if ((wspecs->wep_type == gun_t ||
	  wspecs->wep_type == cannon_t) &&
	!is_tracer)
	return;
    }
  view = new Projectile_View(this,view_color);
  if (view)
    ((Projectile_View *)view)->v_len = view_len;
}

void Projectile::activate(Launch_Params &lp)
{
  is_tracer = lp.flags & LP_TRACER;
  specs = &lp.specs->flt_specs;
  t_position = lp.target_position;
  hitScaler = lp.hitScaler;
  wspecs = lp.specs;
  if (lp.specs->wep_type == gun_t ||
      lp.specs->wep_type == cannon_t)
    view_color = ((Gun_Specs *)lp.specs)->tracer_color;
  else
    view_color = 15;
  fuse_type = lp.fuse_type;
  fuse_data = lp.fuse_data;

  init();
  if (!active)
    return;
  launcher = lp.launcher;
  damage = lp.specs->damage_factor;
  
  if (lp.flags & LP_PORT)
    {
      flt->activate(lp.port,
		    &lp.specs->flt_specs,
		    lp.departure_point,
		    lp.departure_vector,
		    & lp.launcher_velocity
		);
    }
  else
    {
      flt->set_specs(specs);
      flt->activate(lp.departure_point,
		    lp.departure_vector);
    }
  position = l_position = flt->state.flight_port.look_from;
  s_position = t_position;
  flt->start();
  if (wspecs->wep_type == rocket_t)
    {
      smk = new Rocket_Trail((Rocket_Specs *)wspecs);
      MYCHECK(smk != NULL);
      smk->activate(flt);
    }
}


Projectile::~Projectile()
{
  if (flt)
    delete flt;
  if (smk)
    delete smk;
}

void Projectile::activate(R_3DPoint &p, Vector &dir)
{
  init();
  if (!active)
    return;
  position = l_position = s_position = p;
  launcher = NULL;
  wspecs = NULL;
  flt->activate(p,dir);
  flt->start();
}

void Projectile::update()
{
  if (!active)
    return;
  if (!flt->state.on_ground)
    {
      flt->update();
      l_position = position;
      position = flt->state.flight_port.look_from;
      if (flt->state.on_ground)
	{
	  R_3DPoint ip;
	  if (SimMath::sectLinePlane(ip,l_position,
				     position,
				     ggNormal,
				     ggplane_constant) 
	      == SimMath::NORMSECT)
	    {
	      position = ip;
	    }
	}
    }
  elapsed_time += flt->l_time;
  if (smk != NULL)
    smk->update();
  if (elapsed_time >= max_time)
    de_activate();
}

int Projectile::check_for_hit()
{
float hitscl = hit_scaler;

	if (!active || hit)
		return 0;
Target *trg;

   hit_scaler = hitScaler;
	hit = NULL;
	for (int i = 0; i< Unguided_Manager::ntargets; i++)
	{
		trg = Unguided_Manager::targets[i];
		if (trg == launcher)
			continue;
		if (trg->ouch(l_position,position,damage,launcher))
		{
			hit = trg;
			hit->i_hits++;
			if (launcher)
			{
				launcher->youHit(trg);
				launcher->o_hits++;
			}
			hit_scaler = hitscl;
			return(1);
		}
	}
	hit_scaler = hitscl;
	return 0;
}

int Projectile::check_radius_hits()
{
  if (Unguided_Manager::ntargets <= 0)
    return(0);

  int result = 0;
  Target *trg;
  float rad;
  rad_hits = 0;
  rad = wspecs->getDamageRadius();
  if (rad > 0.0)
    {
      for (int i =0; i< Unguided_Manager::ntargets; i++)
	{
	  trg = Unguided_Manager::targets[i];
	  if (trg == hit)
	    continue;
	  if (trg->ouch(position,rad,damage,launcher))
	    {
	      if (rad_hits < MAX_RADIUS_HITS)
		radius_hits[rad_hits++] = trg;
	      if (launcher)
		launcher->youHit(trg);
	      result++;
	    }
	}
    }
  return (result);
}

void Projectile::de_activate()
{
  if (!active)
    return;
  if (flt)
    {
      flt->pause();
      delete flt;
    }
  if (view)
    delete view;
  if (smk)
    {
      smk->de_activate();
      delete smk;
    }
  flt = NULL;
  view = NULL;
  smk = NULL;
  active = 0;
  wspecs = NULL;
}

void Projectile::add_draw_list(DrawList &dl, Port_3D &port)
{
  Moveable::add_draw_list(dl,port);
  if (smk != NULL)
    {
      smk->add_draw_list(dl,port);
    }
}

void Debris::init()
{
  is_tracer = 1;
  if (local_specs)
    delete local_specs;
  local_specs = new FlightLight_Specs(*global_specs);
  if (local_specs == NULL)
    active = 0;
  else
    {
      specs = local_specs;
      Projectile::init();
      if (active && flt)
	flt->do_rotations = 1;
    }
}

void Debris::create_view()
{
  float len = ((float)(RANDOM(debris_width)+1)) * world_scale;
  float wid = ((float)(RANDOM(debris_len)+1)) * world_scale;
  view = new Debris_View(flt,len,wid);
  //	view = NULL;
  if (view != NULL)
    {
      int col = RANDOM(13) + GRAY_COLORS;
      ((Debris_View *)view)->color = col;
    }
  else
    active = 0;
}

void Debris::update()
{
  if (!active)
    return;
  elapsed_time += time_frame;
  if (elapsed_time >= max_time)
    de_activate();
  else
    {
      flt->update();
      l_position = position;
      position = flt->state.flight_port.look_from;
    }
}

void Debris::de_activate()
{
  if (local_specs)
    delete local_specs;
  local_specs = NULL;
  Projectile::de_activate();
}

/*******************************************************
 * Smokey methods                                      *
 *******************************************************/
void Smokey::initSmokey()
{
  sm_init_size *= world_scale;
  sm_d_size *= world_scale;
  sm_d_len *= 5.0;
  sm_kink *= world_scale;
}

Smokey::Smokey()
  :TMoveable(1.0)
{
  for (int i=0;i<MAX_VSMOKES;i++)
    views[i] = NULL;
  setLocalParams();
  flame_time = 0.0;
}

void Smokey::setLocalParams()
{
  n_smokes       = sm_n_smokes;
  init_size      = sm_init_size;
  d_size         = sm_d_size;
  d_len          = sm_d_len;
  kink           = sm_kink;
  max_time       = sm_max_time;
  has_flame      = sm_has_flame;
  flame_time_max = sm_flame_time_max;
  smokemap_id    = sm_smokemap_id;
  firemap_id     = sm_firemap_id;
  nflames        = sm_nflames;
}

Smokey::~Smokey()
{
  for (int i=0;i<MAX_VSMOKES;i++)
    if (views[i] != NULL)
      {
	delete views[i];
	views[i] = NULL;
      }
}

void Smokey::init()
{
  smokemap = map_man->get_map_ptr(smokemap_id);
  if (has_flame)
    {
      firemap = map_man->get_map_ptr(firemap_id);
      flame_time = flame_time_max;
      flame_flag = 1;
    }
  else
    flame_flag = 0;
  cur_position = position;
  cur_size = init_size;
  v = Vector(0,0,d_len);
  next_view = 0;
  nview = 0;
  TMoveable::init();
  active = 1;
}

void Smokey::activate(R_3DPoint &p)
{
  setLocalParams();
  TMoveable::activate(p);
}

void Smokey::de_activate()
{
  for (int i=0;i<MAX_VSMOKES;i++)
    if (views[i] != NULL)
      {
	delete views[i];
	views[i] = NULL;
      }
  TMoveable::de_activate();
}

void Smokey::update()
{
  if (!active)
    return;

  if (flame_flag)
    {
      flame_time -= time_frame;
      if (flame_time <= 0.0)
	flame_flag = 0;
    }
  
  if (next_view < n_smokes)
    {
      if (kink > 0.0)
	RANDPOINT(cur_position,kink);
      views[next_view] = new Bitmap_View(smokemap,&cur_position,
					   cur_size,cur_size);
      MYCHECK(views[next_view] != NULL);
      next_view++;
      cur_size += d_size;
      cur_position += v;
    }
  else
    {
      cur_size = init_size;
      cur_position = TMoveable::position;
      RANDPOINT(cur_position, C(5.0 * world_scale));
      next_view = 0;
      nview = 1;
    }

  TMoveable::update();
}

void Smokey::add_draw_list(DrawList &dl, Port_3D &port)
{
  if (!active)
    return;
  int n;
  
  if (nview)
    n = n_smokes;
  else
    n = next_view;
  for (int i=0;i<n;i++)
    {
      if (views[i] != NULL)
	{
	  if (flame_flag && i < nflames)
	    views[i]->setMap(firemap);
	  else
	    views[i]->setMap(smokemap);
	  views[i]->draw_prep(port);
	  if (views[i]->visible_flag)
	    dl.add_object(views[i]);
	}
    }
}

/*********************************************************
 * Smoke_Trail methods                                   *
 *********************************************************/
Smoke_Trail::Smoke_Trail()
{
  flight = NULL;
  next_view = 0;
}

void Smoke_Trail::init()
{
  Smokey::init();
}

void Smoke_Trail::de_activate()
{
  for (int i=0;i<MAX_VSMOKES;i++)
    if (views[i] != NULL)
      {
	delete views[i];
	views[i] = NULL;
      }
  TMoveable::de_activate();
}

void Smoke_Trail::activate(Flight &flt)
{
  setLocalParams();
  TMoveable::activate(flt.state.flight_port.look_from);

  v = flt.state.velocity.direction.to_vector();
  v *= -1;
  v.Normalize();
  v *= d_len;
  flight = &flt;
  next_view = 0;
}

void Rocket_Trail::activate(FlightLight *flt)
{
  fltlte = flt;
  max_time = Smokey::sm_max_time;
  init_size = rspcs->flume_initl_size;
  d_size = rspcs->flume_delta_size;
  d_len = Smokey::sm_d_len;
  v = fltlte->state.velocity.direction.to_vector();
  v *= -1;
  v.Normalize();
  v *= d_len;
  TMoveable::activate(fltlte->state.flight_port.look_from);
  flame_flag = 1;
  flame_time = flame_time_max;
  next_view = 0;
}

void Smoke_Trail::add_draw_list(DrawList &dl, Port_3D &port)
{
  if (active)
    {
      if (flame_flag)
	{
	  flame_time -= time_frame;
	  if (flame_time <= 0.0)
	    flame_flag = 0;
	}
      for (int i=0;i<next_view;i++)
	{
	  if (views[i] != NULL)
	    {
	      if (flame_flag && i > next_view - 6)
		views[i]->setMap(firemap);
	      else
		views[i]->setMap(smokemap);
	      views[i]->draw_prep(port);
	      if (views[i]->visible_flag)
		{
		  dl.add_object(views[i]);
		}
	    }
	}
    }
}

void Smoke_Trail::update()
{
  if (!active)
    return;
  R_3DPoint cur_p = flight->state.flight_port.look_from;
  Vector v = Vector(cur_p - position);
  if (v.Magnitude() > 80.0 * world_scale)
    {
      if (next_view < MAX_VSMOKES)
	{
	  views[next_view] = new Bitmap_View(smokemap,&cur_p,
					     cur_size,cur_size);
	  MYCHECK(views[next_view] != NULL);
	  for (int i=0;i<next_view-1;i++)
	    {
	      views[i]->world_len += d_size;
	      views[i]->world_width += d_size;
	    }
	  // cur_size += d_size;
	  next_view++;
	  position = cur_p;
	}
      else if (!flight->state.crashed)
	{
	  position = flight->state.flight_port.look_from;
	  for (int i=0;i < MAX_VSMOKES-1;i++)
	views[i]->setPosition(views[i+1]->getPosition());
	  views[MAX_VSMOKES-1]->setPosition(position);
	}
    }
  TMoveable::update();
}

void Rocket_Trail::update()
{
  if (!active)
    return;
  if (fltlte == NULL)
    return;
  R_3DPoint cur_p;

  fltlte->state.flight_port.port2world(rspcs->flume_origin,&cur_p);
  position = cur_p;
  if (next_view < MAX_VSMOKES)
    {
      views[next_view] = new Bitmap_View(smokemap,&cur_p,
					 init_size,init_size);
      MYCHECK(views[next_view] != NULL);
      for (int i=0;i<next_view-1;i++)
	{
	  views[i]->world_len += d_size;
	  views[i]->world_width += d_size;
	}
      next_view++;
    }
  else
    {
      for (int i=0;i < MAX_VSMOKES-1;i++)
	views[i]->setPosition(views[i+1]->getPosition());
      views[MAX_VSMOKES-1]->setPosition(position);
    }
  TMoveable::update();
}


/********************************************************
 * Unguided_Manager methods                             *
 ********************************************************/
void Unguided_Manager::initUnguided()
{
  Smokey::initSmokey();
  inited = 1;
}

Unguided_Manager::Unguided_Manager()
  : m_rounds(MAX_ROUNDS,(Moveable **) rounds),
    m_flak(MAX_FLAK,(Moveable **) flak),
    m_blasts(MAX_BLASTS,(Moveable **) blasts),
    m_dustups(MAX_DUSTUPS,(Moveable **) dustups),
    m_debris(MAX_DEBRIS,(Moveable **) debris),
    m_smokes(MAX_SMOKES,(Moveable **) smokeys),
    m_strails(MAX_STRAILS,(Moveable **) smoke_trails)
{
  int i;
  nrounds = MAX_ROUNDS;
  active_count = 0;
  specs.thrust = 0.0;
  specs.lift_factor = 0.0;
  specs.weight = 0.25;
  specs.init_speed = 50.0;
  specs.time_limit = 5.0;
  specs.drag_factor = C(0.0000004);
  specs.mass = specs.weight / C(32.0);

  for (i=0;i<nrounds;i++)
    {
      rounds[i] = new Projectile(&specs);
      rounds[i]->view_len = C(6.0) * world_scale;
      MYCHECK(rounds[i] != NULL);
    }

  nflak = MAX_FLAK;
  active_flak = 0;
  flak_specs.thrust = 0.0;
  flak_specs.lift_factor = 0.0;
  flak_specs.weight = 0.0;
  flak_specs.init_speed = 500.0;
  flak_specs.time_limit = 10.0;
  flak_specs.drag_factor = 0.0;
  flak_specs.mass = specs.weight / C(32.0);
  for (i=0;i<nflak;i++)
    {
      flak[i] = new Projectile(&flak_specs);
      flak[i]->view_len = C(100.0) * world_scale;
      flak[i]->view_color = 11;
      MYCHECK(flak[i] != NULL);
    }

  nblasts = MAX_BLASTS;
  for (i=0;i<nblasts;i++)
    {
      blasts[i] = new BlastMap();
      MYCHECK(blasts[i] != NULL);
    }

  ndustups = MAX_DUSTUPS;
  for (i=0;i<ndustups;i++)
    {
      dustups[i] = new DustUp(C(2.2));
      MYCHECK(dustups[i] != NULL);
    }
  elapsed_time = 0.0;
  rtime = 0.0;

  debris_specs = flak_specs;
  for (i=0;i<MAX_DEBRIS;i++)
    {
      debris[i] = new Debris(&debris_specs);
      MYCHECK(debris[i] != NULL);
    }

  for (i=0;i<MAX_SMOKES;i++)
    {
      smokeys[i] = new Smokey();
      MYCHECK(smokeys[i] != NULL);
    }

  for (i=0;i<MAX_STRAILS;i++)
    {
      smoke_trails[i] = new Smoke_Trail();
      MYCHECK(smoke_trails[i] != NULL);
    }
}

Unguided_Manager::~Unguided_Manager()
{
  int i;
  for (i=0;i<MAX_ROUNDS;i++)
    delete rounds[i];
  for (i=0;i<nflak;i++)
    delete flak[i];
  for (i=0;i<nblasts;i++)
    delete blasts[i];
  for (i=0;i<ndustups;i++)
    delete dustups[i];
  for (i=0;i<m_debris.n;i++)
    delete debris[i];
  for (i=0;i<m_smokes.n;i++)
    delete smokeys[i];
  for (i=0;i<m_strails.n;i++)
    delete smoke_trails[i];
}

int Unguided_Manager::endProjectilePath(Projectile *p)
{
	R_3DPoint pp;
	Weapon_Specs *wps;

	int result = 0;
	if (p->has_damage_radius())
	if (p->check_radius_hits())
	handle_radius_hits(p);
	wps = p->getWepSpecs();
	if (wps && wps->hasBlastInfo())
	{
		char wep_blast_sound_id[64];
		int vol;
		pp = p->position;
		pp.z += wps->getBlastMapSize() / 2.0 ;
		new_blast(pp,wps->getBlastMapSize(),
		wps->getBlastMapSize(),
		wps->getBlastDur(),
		wps->getSmokeMap(),
		wps->getBlastMap(),
		wps->getBlastDur() / C(4.0));
		sprintf(wep_blast_sound_id,"%sexpl",
				  wps->wep_name);
		vol = sound_calc_distant_vol(pp,3000.0);
		sound_on(wep_blast_sound_id,NORM,vol);
		result = 1;
	}
	else
	{
		dustup_size = C(32.0) * world_scale;
		dustup_time = 1.0;
		pp = p->position;
		pp.z += 16.0 * world_scale;
		new_dustup(pp);
	}
	p->de_activate();
	return(result);
}

int Unguided_Manager::update()
{
	Projectile *p;
	if (m_rounds.active_count)
	{
		m_rounds.active_count = 0;
		for (int i=0;i<m_rounds.n;i++)
		{
			p = rounds[i];
			if (p->active)
			{
				if (p->flt->state.on_ground)
					endProjectilePath(p);
				else if (p->fuse_type == f_altitude &&	p->position.z >= p->fuse_data)
					endProjectilePath(p);
				else
				{
					p->update();
					m_rounds.active_count++;
				}
			}
		}
	}
	m_flak.update();
	m_blasts.update();
	m_dustups.update();
	m_debris.update();
	m_smokes.update();
	m_strails.update();
	return active_count;
}

int Unguided_Manager::new_smoke(R_3DPoint &p,
		int n_smokes,
		float init_size,
		float d_size,
		float d_len,
		float kink,
		float max_time,
		int has_flame,
		float flame_time_max,
		int n_flames,
		const char *smokemap_id,
		const char *firemap_id)
{
  Smokey::sm_n_smokes = n_smokes;
  Smokey::sm_init_size = init_size;
  Smokey::sm_d_size = d_size;
  Smokey::sm_d_len  = d_len;
  Smokey::sm_kink = kink;
  Smokey::sm_max_time = max_time;
  Smokey::sm_has_flame = has_flame;
  Smokey::sm_flame_time_max = flame_time_max;
  Smokey::sm_smokemap_id = smokemap_id;
  Smokey::sm_firemap_id = firemap_id;
  Smokey::sm_nflames = n_flames;
  return (m_smokes.activate(p));
}

int Unguided_Manager::new_flak(R_3DPoint &p, Vector &v)
{
  return (m_flak.activate(p,v));
}

int Unguided_Manager::new_round(Launch_Params &lp)
{
  return (m_rounds.activate(lp));
}

int Unguided_Manager::new_blast(R_3DPoint &loc, float xdim, float ydim,
				float dur, const char *bmap, const char *initmap,
				float initdur)
{
  BlastMap::bm_sizex = xdim;
  BlastMap::bm_sizey = ydim;
  BlastMap::bm_dur = dur;
  BlastMap::bm_map = bmap;
  BlastMap::bm_initmap = initmap;
  BlastMap::bm_initdur = initdur;
  return (m_blasts.activate(loc));
}

int Unguided_Manager::new_dustup(R_3DPoint &p)
{
  return (m_dustups.activate(p));
}

int Unguided_Manager::new_smoke_trail(Flight &flt)
{
  return (m_strails.activate(flt));
}

void Unguided_Manager::add_draw_list(DrawList &dl, Port_3D &port)
{
  m_rounds.add_draw_list(dl,port);
  m_flak.add_draw_list(dl,port);
  m_blasts.add_draw_list(dl,port);
  m_dustups.add_draw_list(dl,port);
  m_debris.add_draw_list(dl,port);
  m_smokes.add_draw_list(dl,port);
  m_strails.add_draw_list(dl,port);
}

void Unguided_Manager::pause()
{
  m_rounds.pause();
  m_flak.pause();
  m_blasts.pause();
  m_dustups.pause();
  m_debris.pause();
  m_smokes.pause();
  m_strails.pause();
}

void Unguided_Manager::start()
{
  m_rounds.start();
  m_flak.start();
  m_blasts.start();
  m_dustups.start();
  m_debris.start();
  m_smokes.start();
  m_strails.start();
}

int Unguided_Manager::check_hits()
{
  if (ntargets <= 0)
    return(0);
  Projectile *p;
  int hit_count = 0;
  if (!m_rounds.active_count)
    return 0;
  for (int i=0;i<m_rounds.n;i++)
    {
      p = rounds[i];
      if (p->active)
	{
	  if (p->check_for_hit())
	    {
	      Target *hit = p->hit;
	      handle_hit(hit,p);
	      hit_count++;
	      endProjectilePath(p);
	    }
	}
    }
  return hit_count;
}

void Unguided_Manager::handle_radius_hits(Projectile *p)
{
  int i;
  Target *hit;

  for (i=0;i<p->rad_hits;i++)
    {
      hit = p->radius_hits[i];
      if (hit)
	{
	  handle_hit(hit,p);
	}
    }
}

void Unguided_Manager::boom(R_3DPoint &origin, int )
{
  float blast_size;
  int nn;
  
  create_debris(origin,RANDOM(20) + 10, RANDOM(20) + 10,
		RANDOM(2) + 2, NULL);
  for (int j=0;j<32;j++)
    {
      int zz = RANDOM(vsize);
      Vector v(vtable[zz][0],vtable[zz][1],vtable[zz][2]);
      v.Z = 1.0;
      v.Normalize();
      v *= C(2000.0) * world_scale;
      new_flak(origin,v);
    }
  
  blast_size = C(160.0) * world_scale;
  new_blast(origin,blast_size,blast_size,1.0,"fire","fire",C(0.1));
  nn = RANDOM(3) + 7;
  new_smoke(origin,
	    nn,
	    blast_size * 2.0f / nn,
	    blast_size * 2.0f / nn,
	    blast_size / 2.0f,
	    C((frand(32.0) + 10.0) * world_scale),
	    160.0,
	    1,
	    20.0,
	    RANDOM(3) + 1,
	    "smoke",
	    "fire");
}

void Unguided_Manager::handle_hit(Target *hit, Projectile *p)
{
	R_3DPoint hit_point;
	Vector    v;
	float     blast_size;
	int       nn;

	int ctype = hit->getType();

	switch (ctype)
	{
	case TARGET_BASE_T:
	case C_3DOBJECT_T:
	case GROUND_UNIT_T:
		hit_point = p->l_position;
		create_debris(hit_point,RANDOM(20) + 10, RANDOM(20) + 10,
		RANDOM(2) + 1, NULL);
		dustup_size = C(12.0 * world_scale);
		dustup_time = (float) RANDOM(6) + 3;
		new_dustup(hit_point);

		if (hit->isHistory())
		{
			hit_point = *hit->get_hit_point();
			for (int j=0;j<15;j++)
			{
				int zz = RANDOM(vsize) - 1;
				if (zz < 0)
					zz = 0;
				v = Vector(vtable[zz][0],vtable[zz][1],vtable[zz][2]);
				new_flak(hit_point,v);
			}

			blast_size = ((float)hit->max_damage)  * 100.0f;
			if (blast_size > 200)
				blast_size = 200;
			blast_size *= world_scale;
			R_3DPoint pp = hit_point;
			pp.z += blast_size / 2.0;
			new_blast(pp,blast_size,blast_size,1.0,"fire","fire",1.0);
			pp = hit_point;
			pp.z += 10 * world_scale;
			nn = RANDOM(5) + 5;
			new_smoke(pp,
						nn,
						blast_size / nn,
						blast_size / nn,
						blast_size / 2.0f,
						C((frand(32.0) + 10.0) * world_scale),
						160.0,
						1,
						20.0,
						RANDOM(2) + 1,
						"smoke",
						"fire");
			int vol = sound_calc_distant_vol(hit_point,1000.0);
			if (vol > 0)
				sound_on(SNDGLOBAL_EXPLOSION_MEDIUM,NORM,vol);
		}

	break;

	case FLIGHT_ZVIEW_T:
		{
			Flight_ZViewer *fvwr = (Flight_ZViewer *) hit;
			dustup_size = 15.0f * world_scale;

			hit_point = *(fvwr->get_hit_point());
			Vector_Q *vq = fvwr->get_velocity();

			if (RANDOM(3) == 1)
			{
				dustup_time = RANDOM(6) + 3;
				new_dustup(hit_point);
			}
			if (RANDOM(2) == 1)
			{
				create_debris(hit_point,RANDOM(5) + 3, RANDOM(5) + 3, 
				RANDOM(2) + 1,
				vq);
			}

			if (fvwr->isHistory())
			{
				blast_size = 50.0f * world_scale;
				new_blast(hit_point,blast_size,blast_size,3.0f,"fire","fire",1.0f);
				Smokey::sm_max_time = 60.0f;
				Smokey::sm_init_size = 12.0f * world_scale;
				Smokey::sm_d_size =   1.8f * world_scale;
				Smokey::sm_d_len = 42.0f * world_scale;
				new_smoke_trail(*(fvwr->flt));
				int vol = sound_calc_distant_vol(hit_point,1000.0f);
				if (vol > 0)
					sound_on(SNDGLOBAL_EXPLOSION_MEDIUM,NORM,vol);
			}
			break;
		}
	}
}

void Unguided_Manager::create_debris(R_3DPoint &p, int , int ,
				     int n, Vector_Q *vq)
{
  /*
  debris_len = l;
  debris_width = w;
  */
  Vector v;

  while (n--)
    {
      int x = RANDOM(vsize);
      debris_specs.time_limit = RANDOM(10) + 3;
      debris_specs.weight = RANDOM(120) + 30;
      debris_specs.mass = debris_specs.weight / 32.0f;
      debris_specs.init_speed = RANDOM(100) + 80;
      debris_specs.drag_factor = RANDOM(10) * 0.00003f;
      debris_specs.init_pitch_rate = RANDOM(15) * 0.3f + 0.4f;
      debris_specs.init_yaw_rate = RANDOM(15) * 0.3f + 0.4f;
      debris_specs.init_roll_rate =  RANDOM(15) * 0.3f + 0.4f;
      if (vq != NULL)
	{
	  v = Vector(vtable[x][0],vtable[x][1],
			      vtable[x][2]);
	  v.Normalize();
	  v *= (((float)RANDOM(15)) + 1.0f) * world_scale;
	  v = v + vq->direction.to_vector();
	  debris_specs.init_speed = vq->magnitude;
	  m_debris.activate(p,v);
	}
      else
	{
	  v = Vector(vtable[x][0],vtable[x][1],
				   vtable[x][2]);
	  v.Normalize();
	  v *= 100.0f * world_scale;
	  m_debris.activate(p,v);
	}
    }
}


