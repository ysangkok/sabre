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
#ifndef GLOBALS_H
#define GLOBALS_H

extern int sound_avail;
extern Mouse mouse;
extern Joystick *joystick0;
extern Joystick *joystick1;
extern unsigned char cockpit[];
extern int cockpit_X_size,cockpit_Y_size,cockpit_trans_color;
extern int mouse_avail;
extern int no_mouse;
extern REAL_TYPE phi_incr;
extern Vector delta_v;
extern int stats_on;
extern bool gpause;
extern int unpause;
extern int track_extra;
extern bool do_random;
extern int no_crash;
extern int routine_key;
extern int select_next;
extern Vector world_light_source;
extern int cockpit_y;
extern int cockpit_x;
extern int p_dbg;
extern Pilot *mon_pilot;
extern int mouse_throttle;
extern int mouse_rudder;
extern const char *lib_path;
extern REAL_TYPE shadow_level;
extern int dump_screen;

// Allows altering time rate
extern float time_factor;
// world_scale is feet per unit
extern float world_scale;
// current elapsed time, 'cooked'
extern float time_frame;
// current elapsed time, raw
extern float raw_time;
// increases chances of a hit by expanding bounding rects of targets
extern float hit_scaler;
// allows us to scale up mobile shapes
extern float shape_scaler;
// maximum time
extern float max_time;
// display flags
extern int display_flags;
#define CLOUDS_ON 0x01
#define TERRAIN_ON 0x02
#endif
