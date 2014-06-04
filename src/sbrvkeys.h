/*
    Sabre Fighter Plane Simulator
    Copyright (C) 1997	Dan Hammer

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
 * Sabre Fighter Plane Simulator                 *
 * File   : sbrvkeys.h                           *
 * Date   : December, 1997                       *
 * Author : Dan Hammer                           *
 * Virtual key codes                             *
 *************************************************/
#ifndef __sbrvkeys_h
#define __sbrvkeys_h
// Define universal key codes
enum 
{
	FI_NO_KEY,
	FI_EXIT_FLIGHT,		
	FI_YOKE_UP,
	FI_YOKE_DOWN,
	FI_YOKE_LEFT,
	FI_YOKE_RIGHT,
	FI_YOKE_CENTER,
	FI_AILERONS_CENTER,
	FI_ELEVATORS_CENTER,
	FI_YOKE_TRIM_UP,
	FI_YOKE_TRIM_DOWN,
	FI_YOKE_TRIM_CENTER,
	FI_THROTTLE_FULL,
	FI_THROTTLE_ZERO,
	FI_THROTTLE_PLUS,
	FI_THROTTLE_MINUS,
	FI_RUDDER_RIGHT,
	FI_RUDDER_LEFT,
	FI_RUDDER_CENTER,
	FI_FLAPS_TOGGLE,
	FI_FLAPS_PLUS,
	FI_FLAPS_MINUS,
	FI_GEAR_TOGGLE,
	FI_SPEED_BRAKES_TOGGLE,
	FI_WHEEL_BRAKES_TOGGLE,
	FI_AUTOPILOT_TOGGLE,
	FI_HUD_TOGGLE,
	FI_PAUSE_TOGGLE,
	FI_WEAPON_ARM_TOGGLE,
	FI_BANG_BANG,
	FI_NEXT_WEAPON,
	FI_NEXT_TARGET,
	FI_COCKPIT_TOGGLE,
	FI_NEXT_VIEW_NODE,
	FI_PLAYER_VIEW_NODE,
	FI_VIEW_FRONT,
	FI_VIEW_FRONT_UP,
	FI_VIEW_FRONT_DOWN,
	FI_VIEW_LEFT,
	FI_VIEW_LEFT_UP,
	FI_VIEW_LEFT_DOWN,
	FI_VIEW_RIGHT,
	FI_VIEW_RIGHT_UP,
	FI_VIEW_RIGHT_DOWN,
	FI_VIEW_REAR,
	FI_VIEW_REAR_UP,
	FI_VIEW_REAR_DOWN,
	FI_VIEW_FRONT_LEFT,
	FI_VIEW_FRONT_LEFT_UP,
	FI_VIEW_FRONT_LEFT_DOWN,
	FI_VIEW_FRONT_RIGHT,
	FI_VIEW_FRONT_RIGHT_UP,
	FI_VIEW_FRONT_RIGHT_DOWN,
	FI_VIEW_REAR_LEFT,
	FI_VIEW_REAR_LEFT_UP,
	FI_VIEW_REAR_LEFT_DOWN,
	FI_VIEW_REAR_RIGHT,
	FI_VIEW_REAR_RIGHT_UP,
	FI_VIEW_REAR_RIGHT_DOWN,
	FI_VIEW_UP,
	FI_VIEW_DOWN,
	FI_VIEW_SATELLITE,
	FI_VIEW_EXTERNAL,
	FI_VIEW_TRACK,
	FI_VIEW_TARGET_TRACK,
	FI_VIEW_FLYBY,
	FI_VIEW_WEAPON_TRACK,
	FI_VIEW_VIRTUAL,
	FI_VIEW_VIRTUAL_SNAP_FRONT,
	FI_VIEW_THETA_PLUS,
	FI_VIEW_THETA_MINUS,
	FI_VIEW_PHI_PLUS,
	FI_VIEW_PHI_MINUS,
	FI_VIEW_PADLOCK,
	FI_EXTERN_VIEW_DISTANCE_PLUS,
	FI_EXTERN_VIEW_DISTANCE_MINUS,
	FI_WIREFRAME_TOGGLE,
	FI_FRAMERATE_TOGGLE,
	FI_DISPLAY_CLOUDS_TOGGLE,
	FI_DISPLAY_TEXTURED_GROUND_TOGGLE,
	FI_FLIGHTPATH_INDICATOR_TOGGLE,
	FI_RADAR_TOGGLE,
	FI_PLANEFRAME_TOGGLE,
	FI_CONTROL_POSITIONS_INDICATOR_TOGGLE,
	FI_SCREEN_DUMP,
	FI_RZ_WHICHLINE,
	FI_AI_INFO_TOGGLE,
	FI_VIEW_DELTA_PLUS,
	FI_VIEW_DELTA_MINUS,
	FI_VIEW_X_PLUS,
	FI_VIEW_X_MINUS,
	FI_VIEW_Y_PLUS,
	FI_VIEW_Y_MINUS,
	FI_VIEW_Z_PLUS,
	FI_VIEW_Z_MINUS
};

// Define joystick button codes
#define FI_TRIGGER_BUTTON 1
#define FI_BUTTON_1       2
#define FI_BUTTON_2       8
#define FI_BUTTON_3       4
#define FI_CH_FRWRD      15
#define FI_CH_RIGHT      11
#define FI_CH_BACK        7
#define FI_CH_LEFT        3

#endif
