/*
    Sabre Fighter Plane Simulator 
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
 *           Sabre Fighter Plane Simulator       *
 * File   : aaaunit.C                            *
 * Date   : Sept, 1997                           *
 * Author : Dan Hammer                           *
 *************************************************/
#include <stdio.h>
#include <iostream.h>
#include <fstream.h>
#include <string.h>
#include <math.h>
#include <limits.h>

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

static REAL_TYPE tg_z = 0.0;

void AAA_Unit::update(Unguided_Manager *um, Target_List &target_list)
{
	if (isHistory())
		return;
	Ground_Unit::update(um,target_list);
	if (target == NULL)
		target = selectTarget(target_list);
	if (target)
		trackTarget(um);
}


void AAA_Unit::trackTarget(Unguided_Manager *um)
{
	elapsed_time += time_frame;
	if (target->isHistory())
	{
		target = NULL;
		return;
	}
	aim_point = position;
	aim_point.z += specs->view_point.y * world_scale;
	target_distance = distance(aim_point,target->position)  / world_scale;
	if (target_distance > gspcs->max_range ||	target_distance < gspcs->min_range)
	{
		target = NULL;
		return;
	}
	if (elapsed_time >= gspcs->rounds_per_second && !(RANDOM(4) == 1))
	{
		Launch_Params lp;
		R_3DPoint pp;
		calcLead(pp,aim_point);
		lp.target_position = target->position;
		lp.specs = gspcs;
		lp.departure_point = aim_point;
		lp.flags = LP_TRACER;
		Vector v = Vector(pp - aim_point);
		v.Normalize();
		lp.departure_vector = v;
		lp.launcher = this;
		lp.flight = NULL;

		if (gspcs->getType() == cannon_t)
		{
			lp.fuse_type = f_altitude;
			lp.fuse_data = tg_z;
		}
		else
		{
			lp.fuse_type = f_impact;
			lp.fuse_data = 0.0;
		}
		um->new_round(lp);
		elapsed_time = 0.0;
	}
	else if (RANDOM(200) == 1)
		elapsed_time = 0.0;
}

void AAA_Unit::calcLead(R_3DPoint &lp, R_3DPoint &ap)
{
float t,hd;

	if (target->getType() == FLIGHT_ZVIEW_T)
	{
		Flight *flt = ((Flight_ZViewer *)target)->flt;
		Vector v = Vector(target->position - ap);
		hd = sqrt(v.X * v.X + v.Y * v.Y + v.Z * v.Z);
		hd /= world_scale;
		t = hd / gspcs->flt_specs.init_speed;
		// printf("hd: %5.2f, t: %3.2f\n",hd,t);
		DVector dv = to_vector(flt->state.velocity);
		dv *= world_scale;
		dv *= t;
		v = dv.to_vector();
		lp = target->position;
		lp += v;
		tg_z = lp.z;
		// Account for gravity drop
		lp.z += (((t * t) * g) / 2.0) * world_scale; 
	}
	else
		lp = target->position;
}

void AAA_Unit::read(istream &is)
{
char c;

	file_context = "AAA_Unit";
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

void AAA_Unit::write(ostream &os)
{
	os << "{\n";
	os << view << '\n';
	os << fps2mph(current_speed) << '\n';
	os << "}\n";
}

void AAA_Unit::youHit(Target *tg)
{
	hits++;
	if (tg->isHistory())
		kills++;
}

Target *AAA_Unit::selectTarget(Target_List &target_list)
{
Target * result = NULL;

	for (int i=0;i<target_list.idx;i++)
	{
		Target *trg = target_list.targets[i];
		if ((trg->affiliation != affiliation) &&
				(!trg->isHistory()) &&
				(trg->getType() == FLIGHT_ZVIEW_T))
		{
			float rng = distance(position,trg->position) / world_scale;
			if (rng <= gspcs->max_range && rng >= gspcs->min_range)
			{
				result = trg;
				break;
			}
		}
	}
	return (result);
}

