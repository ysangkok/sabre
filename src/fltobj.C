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
 * File   : fltobj.C                             *
 * Date   : February, 1998                       *
 * Author : Dan Hammer                           *
 *************************************************/
#include <stdio.h>
#include <string.h>
#include <iostream.h>
#include <fstream.h>
#include <math.h>
#include <limits.h>
#include <values.h>
#include "defs.h"
#include "simerr.h"
#include "sim.h"
#include "pc_keys.h"
#include "vmath.h"
#include "port_3d.h"
#include "rtkey.h"
#include "simfile.h"
#include "fltobj.h"
#include "aipilot.h"
#include "aipildef.h"

const float g = 32.16;
Flight_Specs **Flight_Specs::g_specs = NULL;
int Flight_Specs::nspecs = 0;

/********************************************************
 * Flight_Specs members                                 *
 ********************************************************/
Flight_Specs::Flight_Specs()
{
    weight = 0.0;
    max_thrust = 0.0;
    max_speed = 0.0;
    corner_speed = 0.0;
    drag_factor = 0.0;
    idrag_factor = 0.0;
    lift_factor = 0.0;
    wing_aoa = 0.0;
    max_aoa = 0.0;
    drag_aoa = 0.0;
    drag_yaw = 0.0;
    drag_wb = 0.0;
    drag_gr = 0.0;
    drag_sb = 0.0;
    drag_whls = 0.0;
    control_pitch = 0.0;
    control_yaw = 0.0;
    control_roll = 0.0;
    return_pitch = 0.0;
    return_yaw = 0.0;
    pitch_drag = 0.0;
    yaw_drag = 0.0;
    roll_drag = 0.0;
    pitch_damp = 0.0;
    yaw_damp = 0.0;
    roll_damp = 0.0;
    lspeed = 0.0;
    l_z = 0.0;
    max_wb = 0.0;
    l_aoa_max = 0.0;
    adv_yaw = 0.0;
    adv_roll = 0.0;
    g_height = 0.0;
    flap_lift = 0.01;
    flap_drag = 0.003;
    fuel_cap = 100.0;
    wb_damp = 2.0;
    load_limit = 9.9;
    stall_spin = 0.5;
    max_damage = 100;
    max_aoa_factor = 0.03;
    engine_type = JET_TYPE;
    flags = 0;
	 default_sPID(&pitchPid);
	 default_sPID(&yawPid);
	 default_sPID(&rollPid);
	 default_sPID(&gPid);
	 airSpeedPid.p = aiPILOT_SPEED_P;
	 airSpeedPid.i = aiPILOT_SPEED_I;
	 airSpeedPid.d = aiPILOT_SPEED_D;
	 targetClosurePid.p = aiPILOT_TC_P;
	 targetClosurePid.i = aiPILOT_TC_I;
	 targetClosurePid.d = aiPILOT_TC_D;
	 formationWingLen = DEFLT_FORMATION_WINGLEN;
	 formationOffset = aiPILOT_FORMATION_OFFSET;
	 dihedralX = 2.0;
	 dihedralY = 0.2;
	 dihedralZ = 4.0;
}

Flight_Specs *Flight_Specs::getSpecs(char *model)
{
Flight_Specs *result = NULL;

	if (g_specs)
	{
		for (int i=0;i<nspecs;i++)
		if (g_specs[i] && !strcmp(model,g_specs[i]->model))
		{
			result = g_specs[i];
			break;
		}
	}
	return (result);
}

int Flight_Specs::read_file(char *path)
{
int result = 0;
ifstream infile;

	if (open_is(infile,path))
	{
		result = 1;
		read(infile);
		infile.close();
	}
	return result;
}

int Flight_Specs::write_file(char *path)
{
int result = 0;

	ofstream outfile;
	if (open_libos(outfile,path))
	{
		result = 1;
		write(outfile);
		outfile.close();
	}
	return (result);
}

void Flight_Specs::read(istream &is)
{
char c;

	READ_TOKI('{',is,c);

	is >> weight >> max_thrust >> max_speed >> corner_speed;
	is >> lift_factor >> max_aoa >> wing_aoa;
	is >> drag_factor >> idrag_factor ;
	is >> drag_aoa >> drag_yaw >> drag_wb;
	is >> drag_gr >> drag_sb >> drag_whls;
	is >> control_pitch >> control_yaw >> control_roll;
	is >> return_pitch >> return_yaw;
	is >> pitch_drag >> yaw_drag >> roll_drag;
	is >> pitch_damp >> yaw_damp >> roll_damp;
	is >> lspeed >> l_z >> l_aoa_max >> max_wb;
	is >> adv_yaw >> adv_roll >> g_height ;
	is >> flap_lift >> flap_drag;
	is >> fuel_cap >> wb_damp;
	is >> load_limit >> stall_spin;
	is >> max_damage;
	is >> view_point >> max_aoa_factor;
	is >> engine_type >> flags;
	is >> model;
	is >> pitchPid.base >> pitchPid.p >> pitchPid.i >> pitchPid.d;
	is >> rollPid.base >> rollPid.p >> rollPid.i >> rollPid.d;
	is >> yawPid.base >> yawPid.p >> yawPid.i >> yawPid.d;
	is >> gPid.base >> gPid.p >> gPid.i >> gPid.d;

	is >> airSpeedPid.p >> airSpeedPid.i >> airSpeedPid.d;
	is >> targetClosurePid.p >> targetClosurePid.i >> targetClosurePid.d;

	is >> formationWingLen >> formationOffset;
	is >> dihedralX >> dihedralY >> dihedralZ;

	pitchPid.base *= pitchPid.base;
	rollPid.base *= rollPid.base;
	yawPid.base *= yawPid.base;
	gPid.base *= gPid.base;
	model[8] = '\0';
	max_speed = kts2fps(max_speed);
	corner_speed = kts2fps(corner_speed);
	lspeed = kts2fps(lspeed);
	max_wb = kts2fps(max_wb);
	g_height *= world_scale;

	READ_TOK('}',is,c);
}

void Flight_Specs::write(ostream &os)
{
	os << "{\n";
	os << weight << max_thrust << fps2kts(max_speed) << fps2kts(corner_speed);
	os << lift_factor << max_aoa << wing_aoa;
	os << drag_factor << idrag_factor ;
	os << drag_aoa << drag_yaw << drag_wb;
	os << drag_gr << drag_sb << drag_whls;
	os << control_pitch << control_yaw << control_roll;
	os << return_pitch << return_yaw;
	os << pitch_drag << yaw_drag << roll_drag;
	os << pitch_damp << yaw_damp << roll_damp;
	os << fps2kts(lspeed) << l_z << l_aoa_max << fps2kts(max_wb);
	os << adv_yaw << adv_roll << g_height / world_scale;
	os << flap_lift << flap_drag;
	os << fuel_cap << wb_damp;
	os << load_limit << stall_spin;
	os << max_damage;
	os << view_point << max_aoa_factor;
	os << engine_type << flags;
	os << dihedralX << dihedralY << dihedralZ;
	os << "}\n";
}

void Flight_Controls::read(istream &is)
{
	is >> throttle >> landing_gear >> wheel_brakes;
	is >> cockpit >> hud_on >> vextern;
	is >> autopilot >> armed_w >> radar ;
	is >> vdist >> vtheta >> vphi;
	is >> view;
	if (throttle)
		engine_on = 1;
	// Default radar to on for now
	radar = 1;
}

void Flight_Controls::write(ostream &os)
{
	os << elevators << ' ' << ailerons << ' '  << throttle << '\n';
	os << rudder << ' ' << speed_brakes << '\n' ;
	os << flaps << ' ' << wheel_brakes << ' ' << landing_gear << '\n';
}

istream &operator >>(istream &is, Vector_Q &vq)
{
	char c;
	is >> c;
	while (is && c != '(')
		is >> c;
	if (is)
	{
		is >> vq.magnitude >> vq.direction >> c;
		while (is && c!= ')')
			is >> c;
	}
	return is;
}

DVector to_vector(Vector_Q &vq)
{
	return DVector(vq.direction.X * vq.magnitude,
						vq.direction.Y * vq.magnitude,
						vq.direction.Z * vq.magnitude);
}

DVector operator +(Vector_Q &vq, DVector &v)
{
  DVector v1 = vq.direction * vq.magnitude;
  return DVector(v + v1);
}

Vector_Q operator +(Vector_Q &vq1, Vector_Q &vq2)
{
// Convert magnitudes
float r_mag;
DVector v1 = vq1.direction * vq1.magnitude;
DVector v2 = vq2.direction * vq2.magnitude;
DVector v_result = v1 + v2;

	// Get magnitude of result
	r_mag = v_result.Magnitude();
	// Normalize result
	if (r_mag > 0)
	{
		v_result.X = v_result.X / r_mag;
		v_result.Y = v_result.Y / r_mag;
		v_result.Z = v_result.Z / r_mag;
	}
	else
		v_result.X = v_result.Y = v_result.Z = 0;
	return Vector_Q(r_mag,v_result);
}

void Flight_State::read(istream &is)
{
float kph;

	is >> kph >> flight_port;
	if (kph > 0)
	{
		velocity.magnitude = kts2fps(kph);
		velocity.direction = flight_port.view_normal;
	}
}

