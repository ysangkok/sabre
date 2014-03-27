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
 *       Sabre Fighter Plane Simulator           *
 * File   : hud.C                                *
 * Date   : March, 1997                          *
 * Author : Dan Hammer                           *
 *************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <values.h>
#include <string.h>
#include <math.h>
#include <iostream.h>
#include <fstream.h>
#include "defs.h"
#include "pc_keys.h"
#include "grafix.h"
#include "vga_13.h"
#include "traveler.h"
#include "pen.h"
#include "convpoly.h"
#include "vmath.h"
#include "port_3d.h"
#include "cpoly.h"
#include "copoly.h"
#include "font8x8.h"
#include "flight.h"
#include "fltlite.h"
#include "weapons.h"
#include "unguided.h"
#include "bits.h"
#include "zview.h"
#include "fltzview.h"
#include "simfile.h"
#include "pilot.h"
#include "fltmngr.h"
#include "rtkey.h"
#include "sim.h"
#include "input.h"
#include "globals.h"
#include "siminput.h"
#include "colorspc.h"
#include "earth.h"
#include "hud.h"
#include "aipilot.h"
#include "aimath.h"

extern float raw_time;
#define NOSE_CHAR 128
#define YOKE_CHAR (unsigned char) 136
#define RUDDER_CENTER_CHAR (unsigned char) 139
#define RUDDER_CHAR (unsigned char) 140

void Hud::read_file(char *path)
{
	ifstream is;
	if (open_is(is,path))
		is >> *this;
}

istream &operator >>(istream &is, Hud &hd)
{
	char c = ' ';

	READ_TOK('{',is,c);
	color_spec cs,cs1,cs2,cs3,cs4,cs5;
	is >> cs >> cs1 >> cs2 >> cs3 >> cs4 >> cs5;
	hd.hud_color = cs.color;
	hd.vector_color = cs1.color;
	hd.gunsight_color1 = cs2.color;
	hd.gunsight_color2 = cs3.color;
	hd.look_at_color = cs4.color;
	hd.target_color = cs5.color;
	is >> hd.flt1_x >> hd.flt1_y;
	is >> hd.flt2_x >> hd.flt2_y;
	is >> hd.flt3_x >> hd.flt3_y;
	is >> hd.flt4_x >> hd.flt4_y;
	is >> hd.gns_x >> hd.gns_y;
	is >> hd.trg_x >> hd.trg_y;
	READ_TOK('}',is,c);
	return is;
}

void Hud::do_hud(Flight_Node &my_node)
{
float d1,d2,alt;
Flight & my_flight = *(my_node.flight);
Port_3D &port = my_flight.state.flight_port;
Pilot &pilot = *my_node.pilot;

	alt = port.look_from.z / world_scale;

	// altitude, speed, load
	the_font->font_sprintf(1,1,hud_color,NORMAL," %07.2f KTS", (my_flight.state.z_vel * 3600.0 ) / 6000.0);
	the_font->font_sprintf(1,8,hud_color,NORMAL,"%08.2f", my_flight.state.agl / world_scale);
	the_font->font_sprintf(1,16,hud_color,NORMAL,"%08.2f %1.1f",alt,my_flight.state.load);

	// throttle settings & indicators
	do_hud_stuff(SCREEN_WIDTH-(15*6),1,my_flight);

	if (my_flight.controls.autopilot &&	pilot.GetTask() == aiPilot::ENGAGE_TARGET
		 && my_flight.controls.show_ai_info)
		do_hud_target_flags(0,8,pilot.GetEngageTarget().flags);
	else
	{
		// additional flight info
		the_font->font_sprintf(SCREEN_WIDTH-(8*6),
			8,hud_color,NORMAL,"%08.1f",my_flight.state.climb_vel * 60.0);
		the_font->font_sprintf(SCREEN_WIDTH-(5*6),
			16,hud_color,NORMAL,"%5.1f",57.29578 * my_flight.state.heading);
		// combat info
	}

	d1 = ((float) my_flight.mods.battle_damage) / 
		((float) my_flight.specs->max_damage) * 100.0;
	gns_x = 1;
	gns_y = SCREEN_HEIGHT-8;

	if (stats_on)
	{
		if (raw_time > 0.0)
			d2 = 1.0 / raw_time;
		else
			d2 = 0.0;
		the_font->font_sprintf(gns_x,gns_y,hud_color,NORMAL,
										"%3.0f%% %d %3.1f",
										d1,
										pilot.get_sel_weapon()->rounds_remaining,
										d2);
	}
	else
	{
		char *cc;
		const char *pp;
		if (my_flight.controls.autopilot)
		{
			if (my_flight.controls.show_ai_info)
				pp = pilot.get_dbg();
			else
			{
				const sManeuverState &mvs = pilot.GetManeuverStackTop();
				pp = ((sManeuverState &)mvs).GetManeuverString(); 
			}
		}
		else
			pp = "";
		if (!my_flight.state.on_ground && my_flight.state.near_stall < 0.05)
		{
			if (my_flight.state.stalled)
				cc = "*STALL*";
			else
				cc = "STALL";
		}
		else
			cc = "";
		if (my_flight.controls.armed_w)
			the_font->font_sprintf(gns_x,gns_y,hud_color,NORMAL,
											"%s %s%3.0f%% %s %d",
											pp,
											cc,
											d1,
											pilot.get_sel_weapon()->weapon->w_specs->wep_name,
											pilot.get_sel_weapon()->rounds_remaining
											);
		else
			the_font->font_sprintf(gns_x,gns_y,hud_color,NORMAL,
											"%s %s%3.0f%%",
											pp,
											cc,
											d1
											);
	}

	if (my_flight.controls.show_controls)
		show_controls(my_flight,port,pilot);
	if (pilot.GetTask() == aiPilot::FORMATION_FLY)
	{
		do_hud_formation_geometry(pilot.GetFormationTarget().geometry,port);
		show_selected_target(my_flight,port,pilot);
	}
	else if (pilot.get_target_pilot() != NULL)
	{
		do_hud_target_geometry(pilot.GetEngageTarget().geometry,port);
		show_selected_target(my_flight,port,pilot);
	}
	else
	{
		R_3DPoint navPoint;
		const sNavInfo &navInfo = pilot.GetNavInfo();
		sPoint2R_3DPoint(navInfo.waypoint.to,navPoint);
		show_target_guides(navPoint,port,1);	
	}
}

void Hud::do_hud_stuff(int x, int y, Flight &my_flight)
{
	if (my_flight.controls.autopilot)
		hud_stuff[0] = 'A';
	else
		hud_stuff[0] = ' ';
	if (my_flight.controls.landing_gear)
		hud_stuff[1] = 'G';
	else
		hud_stuff[1] = ' ';
	if (my_flight.controls.wheel_brakes)
		hud_stuff[2] = 'W';
	else
		hud_stuff[2] = ' ';
	if (my_flight.controls.speed_brakes)
		hud_stuff[3] = 'B';
	else
		hud_stuff[3] = ' ';
	if (my_flight.controls.flaps)
	{
		hud_stuff[4] = 'F';
		the_font->font_sprintf(x,y,hud_color,NORMAL,"%s %02.0f %03.0f%%",hud_stuff,
										my_flight.controls.flaps,
										my_flight.controls.throttle);
	}
	else
	{
		hud_stuff[4] = ' ';
		the_font->font_sprintf(x,y,hud_color,NORMAL,"%s    %03.0f%%",hud_stuff,
										my_flight.controls.throttle);
	}
}

void Hud::do_hud_target_flags(int left, int y, const sTargetFlags &flgs)
{
char flagStr[32];
int  x;

	if (left)
		x = 0;
	else
		x = SCREEN_WIDTH - 16*6;

	sprintf(flagStr,"%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
			flgs.rangeClose    ? 'C' : 'c',
			flgs.rangeOpen     ? 'O' : 'o',

			flgs.high          ? 'H' : 'h',
			flgs.low           ? 'L' : 'l',
     
			flgs.veryHigh      ? 'H' : 'h',
			flgs.veryLow       ? 'L' : 'l',

			flgs.forward			? 'F' : 'f',
			flgs.rear				? 'R' : 'r',
          
			flgs.onNose        ? 'N' : 'n',
			flgs.onTail        ? 'T' : 't',

			flgs.turning       ? 'R' : 'r',
			flgs.turning_not   ? 'r' : 'R',

			flgs.headOnAspect  ? 'H' : 'h',
			flgs.tailAspect    ? 'T' : 't',

			flgs.highAOT       ? 'A' : 'a',
			flgs.lowAOT		    ? 'T' : 't'
			);
	the_font->font_sprintf(x,y,hud_color,NORMAL,"%s",flagStr);
}

void Hud::do_hud_formation_geometry(const sTargetGeometry &tg, Port_3D &port)
{
R_3DPoint w,p;
int x,y,x1,y1;
char range_str[32];
char speed_str[32];

	y = SCREEN_HEIGHT - 28;
	x = SCREEN_WIDTH - 7 * 6;
	sPoint2R_3DPoint(tg.formationPoint,w);
	port.world2port(w,&p);
	port.port2screen(p,&x1,&y1);
	if (!port.over_flow && y1 >= 0 && y1 <= SCREEN_HEIGHT - 1 &&
			x1 >= 0 && x1 <= SCREEN_WIDTH - 1)
		the_font->put_char(127,x1-4,y1-4,hud_color,8);
	if (tg.range > 3000.0)
		sprintf(range_str," %05.1f",tg.formationRange / 6000.0);
	else
		sprintf(range_str," %05.2f",tg.formationRange);
	sprintf(speed_str,"%05.2f",fps2kts(tg.airSpeed));
	the_font->font_sprintf(x,y,hud_color,NORMAL,
									"%s",
									speed_str);
	the_font->font_sprintf(x,y+7,hud_color,NORMAL,
									"%s",
									range_str);
	the_font->font_sprintf(x,y+14,hud_color,NORMAL,
									"%5.2f",
									tg.dRange);
}

void Hud::do_hud_target_geometry(const sTargetGeometry &tg, Port_3D &)
{
int x,y;
char range_str[32];

	y = SCREEN_HEIGHT - 28;
	x = SCREEN_WIDTH - 7 * 6;

	if (tg.range > 3000.0)
		sprintf(range_str," %05.1f",tg.range / 6000.0);
	else
		sprintf(range_str," %05.2f",tg.range);
	the_font->font_sprintf(x,y,hud_color,NORMAL,
									"%06.2f",
									tg.angleOff / _PI * 180);
	the_font->font_sprintf(x,y+7,hud_color,NORMAL,
									"%06.2f",
									tg.aspect / _PI * 180);

	the_font->font_sprintf(x,y+14,hud_color,NORMAL,
									"%s",
									range_str);
	the_font->font_sprintf(x,y+21,hud_color,NORMAL,
									"%5.2f",
									tg.dRange);
}

void Hud::show_controls(Flight &my_flight, Port_3D &port, Pilot &pilot)
{
int x1,y1;
int x2,y2;

	port.transform(my_flight.state.flight_port.look_at,&x1,&y1);
	the_font->put_char(NOSE_CHAR,x1-3,y1-4,look_at_color,8);
	if (my_flight.controls.armed_w)
		show_gunsight(pilot,port);
	x2 = x1 - ((int) my_flight.controls.ailerons);
	y2 = y1 + ((int) my_flight.controls.elevators);
	the_font->put_char(YOKE_CHAR,x2-3,y2-4,look_at_color,8);
	the_font->put_char(RUDDER_CENTER_CHAR,x1-3,SCREEN_HEIGHT-3,hud_color,8);
	x2 = x1 + ((int) my_flight.controls.rudder);
	the_font->put_char(RUDDER_CHAR,x2-3,SCREEN_HEIGHT-11,hud_color,8);
	if (my_flight.controls.vect_on)
		show_vector(my_flight,port);
}

void Hud::show_vector(Flight &my_flight, Port_3D &port)
{
int x1,y1;
R_3DPoint lf;
R_3DPoint vr;
Port_3D *fport = &my_flight.state.flight_port;

	if (my_flight.state.velocity.magnitude > 0
			&& !my_flight.controls.armed_w)
	{
		DVector v = to_vector(my_flight.state.velocity);
		vr = R_3DPoint(v.to_vector());
		lf = fport->look_from + vr;
		port.transform(lf,&x1,&y1);
		if (!port.over_flow)
			the_font->put_char((unsigned char)129,
										x1-3,y1-4,vector_color,
										8);
	}
}

void Hud::show_selected_target(Flight &flight, Port_3D &port, Pilot &pilot)
{
R_3DPoint w,p;
int x1,y1;
int x0,y0;
unsigned char c;

	if (pilot.get_target_pilot())
	{
		if (flight.controls.view == fv_front)
		{
			if (pilot.get_in_range())
			{
				w = pilot.get_gun_track_point();
				c = 127;
			}
			else
			{
				w = pilot.get_lead_track_point();
				c = 133;
			}
		}
		else if (flight.controls.view <= fv_track)
		{
			w = pilot.get_track_point();
			c = 133;
		}
		else
		{
			w = flight.state.flight_port.look_from;
			c = 133;
		}
		port.world2port(w,&p);
		port.port2screen(p,&x1,&y1);
		if (!port.over_flow && y1 >= 0 && y1 <= SCREEN_HEIGHT - 1 &&
				x1 >= 0 && x1 <= SCREEN_WIDTH - 1)
		{
			the_font->put_char(c,x1-4,y1-4,hud_color,8);
			port.transform(pilot.get_track_point(),&x0,&y0);
			port.transform(pilot.get_lead_track_point(),&x1,&y1);
			b_linedraw(x0,y0,x1,y1,hud_color,&port.screen);
		}
		if (flight.controls.view == fv_front)
			show_target_guides(p,port,0);			
	}
}

void Hud::show_gunsight(Pilot &pilot, Port_3D &port)
{
int x2,y2;
R_3DPoint p1;

	if (pilot.get_sel_weapon())
	{
		int wpt = WPSPECS(pilot.get_sel_weapon())->wep_type;
		switch (wpt)
		{
			case gun_t:
			port.world2port(pilot.get_gun_point(),&p1);
			break;

			default:
			p1.y = p1.x = 0.0;
			p1.z = 1.0;
			break;
		}
		port.port2screen(p1,&x2,&y2);
		the_font->put_char((unsigned char)130,x2-3,y2-4,hud_color,8);
	}
}


void Hud::show_target_guides(const R_3DPoint &targetLoc, Port_3D &port, int flag)
{
R_3DPoint p;
int       sc_x,sc_y;
int		 x,y,x1,y1,x2,y2;

	if (flag)
		port.world2port(targetLoc,&p);
	else
		p = targetLoc;
	port.port2screen(p,&sc_x,&sc_y);


	if (p.z > 0)
	{
		x = sc_x;
		y = sc_y;
	}
	else
	{
		if (p.x < 0)
			x = port.screen.topLeft.x;
		else
			x = port.screen.botRight.x;
		if (p.y < 0)
			y = port.screen.botRight.y;
		else
			y = port.screen.topLeft.y;
	}
	x1 = x;
	if (x1 < port.screen.topLeft.x)
		x1 = port.screen.topLeft.x;
	else if (x1 > port.screen.botRight.x)
		x1 = port.screen.botRight.x;
	x2 = x1;
	y1 = 0;
	y2 = 5;
	b_linedraw(x1,y1,x2,y2,hud_color,&port.screen);
	y1 = y;
	if (y1 < port.screen.topLeft.y)
		y1 = port.screen.topLeft.y;
	else if (y1 > port.screen.botRight.y)
		y1 = port.screen.botRight.y;
	y2 = y1;
	x1 = 0;
	x2 = 5;
	b_linedraw(x1,y1,x2,y2,hud_color,&port.screen);
}
