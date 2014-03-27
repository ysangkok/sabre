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
/******************************************************************
 * globals                                                        *
 ******************************************************************/
/* 1999-12-08: Changed by Milan Zamazal <pdm@debian.org> to point to default
   library location on Debian, see #ifdef DEBIAN. */
#include "stdio.h"
#include "string.h"
#include "vmath.h"
#include "port_3d.h"
#include "font8x8.h"
#include "pilot.h"
#include "input.h"
#include "plltt.h"
#include "transblt.h"
#include "globals.h"

#ifndef DEFAULT_LIB
#ifdef SABREWIN
#define DEFAULT_LIB ".\\lib"
#else
#ifdef DEBIAN
#define DEFAULT_LIB "/usr/share/games/sabre/lib"
#else
#define DEFAULT_LIB "./lib"
#endif
#endif
#endif

Mouse mouse;
Joystick *joystick0 = NULL;
Joystick *joystick1 = NULL;
// int mouse_avail;
int no_mouse = 0;
int stats_on = 0;
int gpause = 0;
int unpause = 0;
int track_extra = 0;
int do_random = 0;
int no_crash = 0;
int routine_key = 0;
int select_next = 0;
Vector world_light_source;
int cockpit_y = 100;
int cockpit_x = 0;
int p_dbg = 0;
Pilot *mon_pilot = NULL;
int mouse_throttle = 0;
int mouse_rudder = 0;
char *lib_path = DEFAULT_LIB;
int dump_screen = 0;

// Allows altering time rate
float time_factor;
float world_scale = 0.045;
// current elapsed time, 'cooked'
float time_frame = 0.1;
// current elapsed time, raw
float raw_time = 0.1;
// increases chances of a hit by expanding bounding rects of targets
float hit_scaler = 1.0;
// player's hit scaler
float player_hit_scaler = 1.0;
// allows us to scale up mobile shapes
float shape_scaler = 1.0;
// maximum time
float max_time = 0.1;
// shadow level
REAL_TYPE shadow_level = 0.5;
// display flags
int display_flags = CLOUDS_ON | TERRAIN_ON;
