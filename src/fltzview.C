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
 *           SABRE Fighter Plane Simulator              *
 * Version: 0.1                                  *
 * File   : fltzview.C                           *
 * Date   : March, 1997                          *
 * Author : Dan Hammer                           *
 *************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>
#include <fstream.h>
#include <math.h>
#include <limits.h>
#include <values.h>
#include "defs.h"
#include "pc_keys.h"
#include "vga_13.h"
#include "vmath.h"
#include "port_3d.h"
#include "cpoly.h"
#include "obj_3d.h"
#include "copoly.h"
#include "flight.h"
#include "font8x8.h"
#include "weapons.h"
#include "fltzview.h"
#include "sim.h"

extern Font8x8 *the_font;
#define R_TOP 0
#define R_RIGHT 1
#define R_BACK 2
const float shadow_limit = 1500.0;

Flight_ZViewer::Flight_ZViewer(Z_Node_Manager *zv)
  :Z_Viewer(zv)
{
  flt = NULL;
  wil = NULL;
  hurt = 0;
  hitme = 0;
  cpk_flg = 0;
  ground_hits = ground_kills = 0;
  air_hits = air_kills = 0;
  bagged = 0;
}

Flight_ZViewer::~Flight_ZViewer()
{
  if (wil != NULL)
    delete wil;
}

void Flight_ZViewer::set_flight(Flight *f)
{
  flt = f;
  reference_port = &(flt->state.flight_port);
  max_damage = flt->specs->max_damage;
}

extern int rz_which_line;

REAL_TYPE Flight_ZViewer::draw_prep(Port_3D &port)
{
  REAL_TYPE result;
  reference_port = &(flt->state.flight_port);
  result = Z_Viewer::draw_prep(port);
  if (!cpk_flg && visible_flag && wil != NULL && wil->hasExterns())
    wil->draw_prep(&port,&(flt->state.flight_port));
  return (result);
}

void Flight_ZViewer::draw(Port_3D &port)
{
  float ht;
  reference_port = &(flt->state.flight_port);
  int dr = draw_shadow;
  int rz = rz_which_line;
  rz_which_line = 0;
  if (dr)
    {
      ht = reference_port->look_from.z - flt->state.ground_height;
      if (ht * world_scale <= shadow_limit * world_scale)
	draw_shadow = 1;
      else
	draw_shadow = 0;
    }
  if (cpk_flg)
    shapes[cpit].visible = 0;

  /* 
     If there's landing gear shapes, see if they
     should show
     */

  if (z_manager->n_shapes >= lgear3 + 1)
    {
      if (flt->controls.landing_gear)
	{
	  shapes[lgear1].visible = 1;
	  shapes[lgear2].visible = 1;
	  shapes[lgear3].visible = 1;
	}
      else
	{
	  shapes[lgear1].visible = 0;
	  shapes[lgear2].visible = 0;
	  shapes[lgear3].visible = 0;
	}
    }
  Z_Viewer::draw(port);
  if (!cpk_flg && wil != NULL && wil->hasExterns())
    wil->draw(&port);
  draw_shadow = dr;
  rz_which_line = rz;
}

/*****************************************************************
 * Return true if we detect a hit                                *
 *****************************************************************/
int Flight_ZViewer::ouch(const R_3DPoint &p1, const R_3DPoint &p2, int damage, Target *trg)
{

  if (hurt >= max_damage)
    return (0);
  if (damage < threshold_damage)
    return (0);
  int result = 0;
  if (reference_port)
    {
      R_3DPoint r1,r2;
      reference_port->world2port(p1,&r1);
      reference_port->world2port(p2,&r2);
      // It would be nice to simply check if the position of
      // the projectile is in bounds, but ...
      // We have to deal with the fact that during the time frame,
      // the projectile may have travelled a distance greater than
      // the length of our plane ... in other words, it can easily
      // 'skip over' us. So we check for the intersection between
      // the bounding_cube and the line between the start and end of
      // the projectile's movement. This however favors a hit, as we
      // are not taking into account our movement during this time.

      result = calc_hit(r1,r2,bcube);
      if (result)
	{
	  if ( (result = calc_hit(r1,r2,z_manager->shape_info[fuselage].bcube)) != 0 )
	    hit_shape = fuselage;
	  else if ((result = calc_hit(r1,r2,z_manager->shape_info[tail].bcube)) != 0 )
	    hit_shape = tail;
	  else if ((result = calc_hit(r1,r2,z_manager->shape_info[left_wing].bcube)) != 0)
	    hit_shape = left_wing;
	  else if ((result = calc_hit(r1,r2,z_manager->shape_info[right_wing].bcube)) != 0 )
	    hit_shape = right_wing;
	  else if ((result = calc_hit(r1,r2,z_manager->shape_info[left_hstab].bcube)) != 0 )
	    hit_shape = left_hstab;
	  else if ((result = calc_hit(r1,r2,z_manager->shape_info[right_hstab].bcube)) != 0)
	    hit_shape = right_hstab;
	  else if ((result = calc_hit(r1,r2,z_manager->shape_info[cpit].bcube)) != 0)
	    hit_shape = cpit;
	  else if (hit_scaler > 1.0)
	    {
	      result = 1;
	      hit_shape = RANDOM(right_wing+1);
	    }
	  if (result)
	    {
	      calc_damage(hit_shape,damage);
	      who_got_me = trg;
	      hitme = 1;
	    }
	}
    }

  if (hurt >= max_damage)
    {
      bagged = 1;
      who_got_me = trg;
    }
  return result;
}

int Flight_ZViewer::ouch(const R_3DPoint &w0, float radius, int damage, Target *tg)
{
  if (isHistory() || damage < threshold_damage)
    return (0);
  R_3DPoint p0;
  int dmg;
  int result = 0;
  reference_port->world2port(w0,&p0);
  if (calc_radial_damage(p0, radius, damage, bcube, dmg))
    {
      if (dmg > threshold_damage)
	{
	  result = 1;
	  hit_shape = 0;
	  hurt += damage;
	  calc_damage(hit_shape,dmg);
	  hitme = 1;
	  who_got_me = tg;
	  if (isHistory())
	    bagged = 1;
	}
    }
  return (result);
}

void Flight_ZViewer::youHit(Target *tg)
{
  switch (tg->getType())
    {
    case TARGET_BASE_T:
    case C_3DOBJECT_T:
    case GROUND_UNIT_T:
      ground_hits++;
      if (tg->isHistory())
	ground_kills++;
      break;

    case FLIGHT_ZVIEW_T:
      air_hits++;
      if (tg->isHistory())
	air_kills++;
      break;
    }
}

// See if the flight is in landing parameters
// if it is on the ground. Return true if plane has
// 'landed' ... for now, that means on the ground
int Flight_ZViewer::landing_report(Landing_Report &lr)
{
	int result = 0;
	if (flt->state.landing_recorded)
	{
		if (lr.landing_attempted)
		{
			/* If we've already recorded a landing,
			* but we're airborne again, reset
			* some flags so we can record one again
			*/
			if (!flt->state.on_ground)
			{
				flt->state.landing_recorded = 0;
				lr.landing_attempted = 0;
			}
			return 0;
		}

		lr.landing_attempted = 1;
		result = 1;
		// Ooops -- forgot landing gear!
		if (!flt->controls.landing_gear)
		{
			lr.gear_up = 1;
			lr.score -= (int) (50 + 10 * flt->state.landing_velocity);
		}
		// Did we dig a hole?
		if (flt->state.landing_z <= -flt->specs->l_z)
		{
			lr.l_z = 1;
			lr.l_z_value = flt->state.landing_z;
			lr.score -= (int) (10 * (fabs((flt->state.landing_z - -flt->specs->l_z)) / 10.0));
		}
		// Too fast?
		if (flt->state.landing_velocity > flt->specs->lspeed)
		{
			lr.v = 1;
			lr.v_value = flt->state.landing_velocity;
			lr.score -= (int) ((flt->state.landing_velocity - flt->specs->lspeed) / 10);
		}

		// bad pitch?
		if (flt->state.landing_pitch < _PI2 - 0.8 ||
		flt->state.landing_pitch > _PI2 + 0.8)
		{
			lr.phi = 1;
			lr.phi_value = flt->state.landing_pitch;
			lr.score -= (int) (fabs(_PI2 - flt->state.landing_pitch) * 20);
		}

		// bad roll?
		if (flt->state.landing_roll > 0.4 && flt->state.landing_roll < _2PI - 0.4)
		{
			lr.roll = 1;
			lr.roll_value = flt->state.landing_roll;
			lr.score -= (int) (fabs(_PI - flt->state.landing_roll) * 20);
		}

		if (lr.score < 0)
		{
			calc_damage(fuselage,-lr.score);
			if (hurt > max_damage)
				flt->state.crashed = 1;
		}
	}
	return result;
}

void Flight_ZViewer::calc_damage(int hit_shape, int damage)
{
  int choice;
  flt->mods.battle_damage += damage;
  hurt = flt->mods.battle_damage;

  if (flt->mods.battle_damage >= max_damage)
    {
      flt->mods.spin_out = frand(63.0) + 15.0;
      if (RANDOM(2)) 
	flt->mods.spin_out *= -1.0;

      flt->mods.yaw_out = frand(5.0) + 2.0;
      if (RANDOM(2)) 
	flt->mods.yaw_out *= -1.0;

      flt->mods.pitch_out = frand(6.0) + 3.0;
      if (RANDOM(2)) 
	flt->mods.pitch_out *= -1.0;
     
      flt->mods.engine_e = 0;
      flt->mods.elevators_e = 0;
      flt->mods.ailerons_e = 0;
      flt->mods.speed_brakes_e = 0;
      flt->mods.rudder_e = 0;
    }
  else if (flt->mods.battle_damage >= max_damage / 4)
    {
      flt->mods.engine_e = (max_damage - flt->mods.battle_damage) / 100.0;
      float dmg = ((float)damage) / ((float)max_damage);
      switch (hit_shape)
	{
	case fuselage:
	case tail:
	case left_hstab:
	case right_hstab:
	case cpit:
	  choice = RANDOM(4);
	  switch (choice)
	    {
	    case 0:
	      flt->mods.elevators_e -= dmg;
	      break;
	    case 1:
	      flt->mods.h_stab_e -= dmg;
	      break;
	    case 2:
	      flt->mods.speed_brakes_e -= dmg;
	      break;
	    case 3:
	      flt->mods.v_stab_e -= dmg;
	      break;
	    }
	  break;

	case left_wing:
	case right_wing:
	  if (flt->mods.spin_out == 0.0 && flt->mods.battle_damage >= max_damage * 0.75)
	    {
	      flt->mods.spin_out = frand(15.0) + 4.0;
	      if (hit_shape == right_wing)
		flt->mods.spin_out = -flt->mods.spin_out;
	    }
	  if (RANDOM(2))
	    {
	      flt->mods.ailerons_e -= dmg;
	      if (flt->mods.ailerons_e < 0.1)
		flt->mods.ailerons_e = 0.1;
	    }
	  else
	    {
	      flt->mods.wing_e -= dmg;
	    }
	}
    }
}


R_3DPoint *Flight_ZViewer::get_hit_point()
{
  C_ShapeInfo &si = z_manager->shape_info[hit_shape];
  C_PolyInfo &pi = si.polyinfos[RANDOM(si.npolys)];
  R_3DPoint &hp = pi.lpoints[RANDOM(pi.npoints)];
  reference_port->port2world(hp,&hitpoint);
  return (&hitpoint);
}

R_3DPoint *Flight_ZViewer::get_position()
{
  return (&flt->state.flight_port.look_from);
}

Vector_Q *Flight_ZViewer::get_velocity()
{
  if (flt)
    return (& flt->state.velocity);
  else
    return NULL;
}
