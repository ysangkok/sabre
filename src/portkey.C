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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <ctype.h>
#include <iostream>
#include <fstream>
#include <setjmp.h>
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
#include "obj_3d.h"
#include "group_3d.h"
#include "copoly.h"
#include "earth.h"
#include "font8x8.h"
#include "kbdhit.h"
#include "portkey.h"
#include "fltzview.h"
#include "fltmngr.h"
#include "siminput.h"

REAL_TYPE delta_angle = 0.05;
REAL_TYPE delta = 10.0;
extern REAL_TYPE world_scale;
extern int frame_switch;
extern void blit_buff();
extern float raw_time;
extern float time_frame;
extern int gpause;
extern int dump_screen;

int port_key(Port_3D &port)
{
Vector lf,la;

	if (FlightInput::kbdin == FI_EXIT_FLIGHT)
		return 0;

	Vector v = Vector(port.look_at - port.look_from);
	v.Normalize();
	Vector v2 = v;
	v2 *= delta;
	switch (FlightInput::kbdin)
	{

	case FI_VIEW_DELTA_PLUS:
		delta += 0.1;
		break;

	case FI_VIEW_DELTA_MINUS:
		if (delta > 0.2)
		 delta -= 0.1;
		break;

	case FI_EXTERN_VIEW_DISTANCE_PLUS:
		port.delta_look_from(Vector(0,0,delta));
		break;

	case FI_EXTERN_VIEW_DISTANCE_MINUS:
		port.delta_look_from(Vector(0,0,-delta));
		break;

	case FI_VIEW_FRONT:
		lf = Vector(Vector(port.look_from) + v2);
		la = lf + v;
		port.set_view(R_3DPoint(lf),R_3DPoint(la));
		break;

	case FI_VIEW_REAR:
		v2 *= -1;
		lf = Vector(Vector(port.look_from) + v2);
		la = lf + v;
		port.set_view(R_3DPoint(lf),R_3DPoint(la));
		break;

	case FI_VIEW_THETA_PLUS:
		port < delta_angle;
		break;

	case FI_VIEW_THETA_MINUS:
		port < -delta_angle;
		break;

	case FI_VIEW_PHI_PLUS:
		port > -delta_angle;
		break;

	case FI_VIEW_PHI_MINUS:
		port > delta_angle;
		break;

	case FI_WIREFRAME_TOGGLE:
		frame_switch = !frame_switch;
		break;

	case FI_VIEW_X_PLUS:
		port.delta_look_from(Vector(delta,0,0));
		break;

	case FI_VIEW_X_MINUS:
		port.delta_look_from(Vector(-delta,0,0));
		break;

	case FI_VIEW_Y_PLUS:
		port.delta_look_from(Vector(0,delta,0));
		break;

	case FI_VIEW_Y_MINUS:
		port.delta_look_from(Vector(0,-delta,0));
		break;

	case FI_VIEW_Z_PLUS:
		port.delta_look_from(Vector(0,0,delta));
		break;

	case FI_VIEW_Z_MINUS:
		port.delta_look_from(Vector(0,0,-delta));
		break;
	}
	FlightInput::kbdin_lock = 0;
	FlightInput::kbdin = FI_NO_KEY;

	return 1;
}

extern R_3DPoint pp1,pp0;
extern TPoly *tppoly;

void show_port_vars(Port_3D &port, Port_3D &show_port , SimFont *the_font)
{
  int x1,y1;
  //  REAL_TYPE ground_level = the_earth->getGroundLevel(port.look_from);
  //  if (tppoly != NULL)
  //  tppoly->hilite(show_port,12);
  the_font->font_sprintf(0,9,4,NORMAL,"x: %5.3f\ty: %5.3f\tz: %5.3f\t\t %3.3f",
			 port.look_from.x / world_scale,
                         port.look_from.y / world_scale,
                         port.look_from.z / world_scale,
                         delta / world_scale);

  the_font->font_sprintf(0,18,4,NORMAL,"theta: %2.3f\tphi: %2.3f\troll: %2.3f\t %5.3f",
			 port.slook_from.theta,
			 port.slook_from.phi,
			 port.roll,
			 1.0 / time_frame);
  show_port.transform(port.look_at,&x1,&y1);
  the_font->put_char((unsigned char)128,x1-4,y1-4,4,8);
}
