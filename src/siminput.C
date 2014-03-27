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
 * File   : siminput.h                           *
 * Date   : March, 1997                          *
 * Author : Dan Hammer                           *
 *************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <limits.h>
#include <values.h>
#include "defs.h"
#include "pc_keys.h"
#include "key_map.h"
#include "vmath.h"
#include "port_3d.h"
#include "sim.h"
#include "cpoly.h"
#include "obj_3d.h"
#include "copoly.h"
#include "input.h"
#include "rtkey.h"
#include "simfile.h"
#include "flight.h"
#include "flight.h"
#include "fltlite.h"
#include "weapons.h"
#include "pilot.h"
#include "zview.h"
#include "fltzview.h"
#include "unguided.h"
#include "fltmngr.h"
#include "terrain.h"
#include "kbdhit.h"
#include "siminput.h"
#include "globals.h"

extern int frame_switch;
int FlightInput::kbdin = FI_NO_KEY;
int FlightInput::kbdin_lock = 0;

extern void buffer2ppm();

int FlightInput::UpdateFlightInput(FlightInput &fi, 
				   Flight_Node &node, 
				   Flight_Manager &fm)
{
  int result = 1;
  Flight &the_flight = (Flight &) *node.flight;
  if (!fi.update(the_flight,kbdin))
    result = SwitchOnKey(kbdin,node,fm);
  kbdin_lock = 0;
  kbdin = FI_NO_KEY;
  return result;
}

int FlightInput::SwitchOnKey(int, Flight_Node &node, Flight_Manager &fm)
{

  if (kbdin == FI_EXIT_FLIGHT)
    return 0;

  Flight &flt = (Flight &) *node.flight;

  switch (kbdin)
    {

    case FI_NEXT_VIEW_NODE:
      fm.set_view_node(fm.view_node + 1);
      break;

    case FI_PLAYER_VIEW_NODE:
      fm.set_view_node(0);
      break;

    case FI_THROTTLE_FULL:
      if (!flt.controls.autopilot)
	flt.controls.throttle = 100;
      break;

    case FI_THROTTLE_ZERO:
      if (!flt.controls.autopilot)
	flt.controls.throttle = 0;
      break;

    case FI_THROTTLE_PLUS:
      if (!flt.controls.autopilot)
	if (flt.controls.throttle < 100)
	  flt.controls.throttle += 1.0;
      break;

    case FI_THROTTLE_MINUS:
      if (!flt.controls.autopilot)
	if (flt.controls.throttle > 10)
	  flt.controls.throttle -= 1.0;
      break;

    case FI_RUDDER_LEFT:
      if (!flt.controls.autopilot)
	if (flt.controls.rudder > flt.controls.rudder_min)
	  flt.controls.rudder -= 2.0;
      break;

    case FI_RUDDER_RIGHT:
      if (!flt.controls.autopilot)
	if (flt.controls.rudder < flt.controls.rudder_max)
	  flt.controls.rudder += 2.0;
      break;

    case FI_RUDDER_CENTER:
      if (!flt.controls.autopilot)
	flt.controls.rudder = 0.0;
      break;

    case FI_FLAPS_PLUS:
      if (!flt.controls.autopilot)
	if (flt.controls.flaps < flt.controls.flaps_max)
	  flt.controls.flaps += flt.controls.flaps_step;
      break;

    case FI_FLAPS_MINUS:
      if (!flt.controls.autopilot)
	if (flt.controls.flaps > 0)
	  flt.controls.flaps -= flt.controls.flaps_step;
      break;

    case FI_FLAPS_TOGGLE:
      if (!flt.controls.autopilot)
	{
	  if (flt.controls.flaps != 0)
	    flt.controls.flaps = 0;
	  else
	    flt.controls.flaps = flt.controls.flaps_max;
	}
      break;

    case FI_COCKPIT_TOGGLE:
      flt.controls.cockpit = !flt.controls.cockpit;
      break;

    case FI_HUD_TOGGLE:
      flt.controls.hud_on = !flt.controls.hud_on;
      break;

    case FI_SPEED_BRAKES_TOGGLE:
      if (!flt.controls.autopilot)
	flt.controls.speed_brakes = !flt.controls.speed_brakes;
      break;

    case FI_WHEEL_BRAKES_TOGGLE:
      if (!flt.controls.autopilot)
	flt.controls.wheel_brakes =
	  !flt.controls.wheel_brakes;
      break;

    case FI_GEAR_TOGGLE:
      if (!flt.controls.autopilot)
	flt.controls.landing_gear =
	  !flt.controls.landing_gear;
      break;

    case FI_WIREFRAME_TOGGLE:
      frame_switch = !frame_switch;
      break;

    case FI_FLIGHTPATH_INDICATOR_TOGGLE:
      flt.controls.vect_on = !flt.controls.vect_on;
      break;

    case FI_AUTOPILOT_TOGGLE:
      flt.controls.autopilot =
	!flt.controls.autopilot;
      break;

    case FI_PAUSE_TOGGLE:
      gpause = !gpause;
      break;

    case FI_WEAPON_ARM_TOGGLE:
      flt.controls.armed_w = !flt.controls.armed_w;
      break;

    case FI_RADAR_TOGGLE:
      flt.controls.radar = !flt.controls.radar;
      break;

    case FI_VIEW_FRONT:
      flt.controls.view = fv_front;
      break;
    case FI_VIEW_FRONT_UP:
      flt.controls.view = fv_front_up;
      break;
    case FI_VIEW_FRONT_DOWN:
      flt.controls.view = fv_front_down;
      break;

    case FI_VIEW_LEFT:
      flt.controls.view = fv_left;
      break;
    case FI_VIEW_LEFT_UP:
      flt.controls.view = fv_left_up;
      break;
    case FI_VIEW_LEFT_DOWN:
      flt.controls.view = fv_left_down;
      break;

    case FI_VIEW_RIGHT:
      flt.controls.view = fv_right;
      break;
    case FI_VIEW_RIGHT_UP:
      flt.controls.view = fv_right_up;
      break;
    case FI_VIEW_RIGHT_DOWN:
      flt.controls.view = fv_right_down;
      break;

    case FI_VIEW_REAR:
      flt.controls.view = fv_rear;
      break;
    case FI_VIEW_REAR_UP:
      flt.controls.view = fv_rear_up;
      break;
    case FI_VIEW_REAR_DOWN:
      flt.controls.view = fv_rear_down;
      break;

    case FI_VIEW_FRONT_LEFT:
      flt.controls.view = fv_front_left;
      break;
    case FI_VIEW_FRONT_LEFT_UP:
      flt.controls.view = fv_front_left_up;
      break;
    case FI_VIEW_FRONT_LEFT_DOWN:
      flt.controls.view = fv_front_left_down;
      break;

    case FI_VIEW_FRONT_RIGHT:
      flt.controls.view = fv_front_right;
      break;
    case FI_VIEW_FRONT_RIGHT_UP:
      flt.controls.view = fv_front_right_up;
      break;
    case FI_VIEW_FRONT_RIGHT_DOWN:
      flt.controls.view = fv_front_right_down;
      break;

    case FI_VIEW_REAR_LEFT:
      flt.controls.view = fv_rear_left;
      break;
    case FI_VIEW_REAR_LEFT_UP:
      flt.controls.view = fv_rear_left_up;
      break;
    case FI_VIEW_REAR_LEFT_DOWN:
      flt.controls.view = fv_rear_left_down;
      break;

    case FI_VIEW_REAR_RIGHT:
      flt.controls.view = fv_rear_right;
      break;
    case FI_VIEW_REAR_RIGHT_UP:
      flt.controls.view = fv_rear_right_up;
      break;
    case FI_VIEW_REAR_RIGHT_DOWN:
      flt.controls.view = fv_rear_right_down;
      break;

    case FI_VIEW_SATELLITE:
      flt.controls.view = fv_satellite;
      break;

    case FI_VIEW_EXTERNAL:
      flt.controls.view = fv_external;
      break;

    case FI_VIEW_TRACK:
      flt.controls.view = fv_track;
      break;

    case FI_VIEW_PADLOCK:
      flt.controls.view = fv_padlock;
      break;

    case FI_VIEW_TARGET_TRACK:
      flt.controls.view = fv_target_track;
      break;

    case FI_VIEW_FLYBY:
      flt.controls.view = fv_flyby;
      break;

    case FI_VIEW_WEAPON_TRACK:
      flt.controls.view = fv_weapon_track;
      break;

    case FI_VIEW_VIRTUAL:
      flt.controls.view = fv_virtual;
      break;
	
    case FI_VIEW_VIRTUAL_SNAP_FRONT:
      flt.controls.virtTheta = 0.0;
      flt.controls.virtPhi = 0.0;
      break;

    case FI_YOKE_TRIM_UP:
      if (!flt.controls.autopilot)
	if (flt.controls.trim < 30)
	  flt.controls.trim += 1.0;
      break;

    case FI_YOKE_TRIM_DOWN:
      if (!flt.controls.autopilot)
	if (flt.controls.trim > -30)
	  flt.controls.trim -= 1.0;
      break;

    case FI_YOKE_TRIM_CENTER:
      if (!flt.controls.autopilot)
	flt.controls.trim = 0;
      break;

    case FI_BANG_BANG:
      flt.controls.bang_bang = 1;
      break;

    case FI_NEXT_TARGET:
      select_next = 1;
      break;

    case FI_FRAMERATE_TOGGLE:
      stats_on = !stats_on;
      break;

    case FI_NEXT_WEAPON:
      flt.controls.bang_bang = 0;
      node.pilot->selectNextWeapon(1);
      break;

    case FI_PLANEFRAME_TOGGLE:
      flt.controls.vextern = !flt.controls.vextern;
      break;

    case FI_CONTROL_POSITIONS_INDICATOR_TOGGLE:
      flt.controls.show_controls = !flt.controls.show_controls;
      break;

    case FI_EXTERN_VIEW_DISTANCE_PLUS:
      if (flt.controls.vdist > 20.0)
	flt.controls.vdist -= 20.0;
      break;

    case FI_EXTERN_VIEW_DISTANCE_MINUS:
      flt.controls.vdist += 20.0;
      break;

    case FI_VIEW_THETA_PLUS:
      if (flt.controls.view == fv_virtual)
	{
	  flt.controls.virtTheta += flt.controls.virtStep;
	  if (flt.controls.virtTheta > two_pi)
	    flt.controls.virtTheta = flt.controls.virtTheta - two_pi;
	}
      else
	flt.controls.vtheta += flt.controls.virtStep;
      break;

    case FI_VIEW_THETA_MINUS:
      if (flt.controls.view == fv_virtual)
	{
	  flt.controls.virtTheta -= flt.controls.virtStep;
	  if (flt.controls.virtTheta < 0.0)
	    flt.controls.virtTheta = two_pi + flt.controls.virtTheta;
	}
      else
	flt.controls.vtheta -= flt.controls.virtStep;
      break;

    case FI_VIEW_PHI_PLUS:
      if (flt.controls.view == fv_virtual)
	{
	  flt.controls.virtPhi += flt.controls.virtStep;
	  if (flt.controls.virtPhi > two_pi)
	    flt.controls.virtPhi = flt.controls.virtPhi - two_pi;
	}
      else
	flt.controls.vphi += flt.controls.virtStep;
      break;

    case FI_VIEW_PHI_MINUS:
      if (flt.controls.view == fv_virtual)
	{
	  flt.controls.virtPhi -= flt.controls.virtStep;
	  if (flt.controls.virtPhi < 0.0)
	    flt.controls.virtPhi = two_pi + flt.controls.virtPhi;
	}
      else
	flt.controls.vphi -= flt.controls.virtStep;
      break;

    case FI_SCREEN_DUMP:
      dump_screen = 1;
      break;

    case FI_DISPLAY_CLOUDS_TOGGLE:
      display_flags ^= CLOUDS_ON;
      break;

    case FI_DISPLAY_TEXTURED_GROUND_TOGGLE:
      display_flags ^= TERRAIN_ON;
      break;

    case FI_RZ_WHICHLINE:
      extern int rz_which_line;
      rz_which_line = !rz_which_line;
      break;

    case FI_AI_INFO_TOGGLE:
      flt.controls.show_ai_info = !flt.controls.show_ai_info;
      break;

    }
  return 1;
}


FlightInput::FlightInput(Mouse *ms, Joystick *yj, Joystick *rj, Joystick *tr,
			 int mt, int mr)
{
  m_throttle = mt;
  m_rudder = mr;
  set_devices(ms,yj,rj,tr);
}

void FlightInput::set_devices(Mouse *ms, Joystick *yj, Joystick *rj, Joystick *tr)
{
  mouse = ms;
  y_joy = yj;
  r_joy = rj;
  t_joy = tr;

  throttle_device = rudder_device = yoke_device = kbd_ctrl;

  if (y_joy)
    yoke_device = joy_ctrl;
  else if (ms)
    yoke_device = mouse_ctrl;
  if (t_joy)
    throttle_device = joy_ctrl;
  if (r_joy)
    rudder_device = joy_ctrl;

}

/**********************************************
 * return true if key was used                *
 **********************************************/
int FlightInput::update(Flight &flt, int key)
{
  /* Skip if autopilot in control */
  if (flt.controls.autopilot)
    return(0);

  if (yoke_device == kbd_ctrl)
    /* Keyboard only */
    return (yoke_key(flt,key));

  if (yoke_device == mouse_ctrl)
    {
      /* Mouse only */
      yoke_mouse(flt);
      return(0);
    }

  /* Do yoke controls */
  yoke_joystick(flt);
  /* Do rudder */
  if (rudder_device == mouse_ctrl)
    rudder_mouse(flt);
  else if (rudder_device == joy_ctrl)
    rudder_joy(flt);
  /* Do throttle */
  if (throttle_device == mouse_ctrl)
    throttle_mouse(flt);
  else if (throttle_device == joy_ctrl)
    throttle_joy(flt);
  return (0);
}

/* use keys to control yoke */
int FlightInput::yoke_key(Flight &flt, int key)
{
  if (flt.controls.autopilot)
    return 0;

  int handled = 0;

  switch(key)
    {
    case FI_YOKE_DOWN:
      if (flt.controls.elevators < 30)
	flt.controls.elevators += 1.0;
      handled = 1;
      break;

    case FI_YOKE_UP:
      if (flt.controls.elevators > -30)
	flt.controls.elevators -= 1.0;
      handled = 1;
      break;

    case FI_YOKE_LEFT:
      if (flt.controls.ailerons < 30)
	flt.controls.ailerons += 2.0;
      handled = 1;
      break;

    case FI_YOKE_RIGHT:
      if (flt.controls.ailerons > -30)
	flt.controls.ailerons -= 2.0;
      handled = 1;
      break;

    case FI_YOKE_CENTER:
      flt.controls.ailerons =
	flt.controls.elevators = 0.0;
      handled = 1;
      break;

    }
  return handled;
}


/* Use mouse to control flight yoke */
void FlightInput::yoke_mouse(Flight &flt)
{
  float e,a;
  float m_x,m_y;

  m_x = mouse->GetX();
  m_x = -m_x;
  m_y = mouse->GetY();
  e = (m_y * 30.0);
  a = (m_x * 30.0);
  flt.controls.ailerons = a;
  if (e + flt.controls.trim > 30)
    e = 30 - flt.controls.trim;
  if (e + flt.controls.trim < -30)
    e = -30 - flt.controls.trim;
  flt.controls.elevators = e;
  if (mouse->GetButtons())
    flt.controls.bang_bang = 1;
}

/* Use mouse to control throttle & brakes */
void FlightInput::throttle_mouse(Flight &flt)
{
  float m_y;
  // 0 <= m_y <= 1
  m_y = -mouse->GetY();
  if (m_y < 0.0)
    m_y = 0.0;
  flt.controls.throttle =  (m_y * 100.0);
  if (mouse->GetButtons())
    flt.controls.speed_brakes = !flt.controls.speed_brakes;
}

/* use mouse to control rudder */
void FlightInput::rudder_mouse(Flight &flt)
{
  float m_x;
  // -1 <= m_x <= 1
  m_x = mouse->GetX();
  flt.controls.rudder = (m_x * 30.0);
}

/* use joystick to control rudder */
void FlightInput::rudder_joy(Flight &flt)
{
  float m_r;
  m_r = r_joy->GetR();
  flt.controls.rudder = (m_r * 30.0);
}

/* use joystick to control throttle */
void FlightInput::throttle_joy(Flight &flt)
{
  float m_z;
  // -1 <= m_y <= 1
  m_z = -t_joy->GetZ();
  if (fabs(m_z) >= 0.92)
    {
      if (m_z < 0)
	m_z = -1.0;
      else
	m_z = 1.0;
    }
  flt.controls.throttle = m_z * 50.0 + 50.0;
}

/* use joystick to control yoke */
void FlightInput::yoke_joystick(Flight &flt)
{
  float m_x,m_y;
  float e,a;

  m_x = y_joy->GetX();
  m_x = -m_x;
  m_y = y_joy->GetY();
  e = (int) (m_y * 30.0);
  a = (int) (m_x * 30.0);
  flt.controls.ailerons = a;
  if (e + flt.controls.trim > 30)
    e = 30 - flt.controls.trim;
  if (e + flt.controls.trim < -30)
    e = -30 - flt.controls.trim;
  flt.controls.elevators = e;
  int bt = y_joy->GetButtons();
  if (bt)
    do_joystick_button(bt,flt);
}

/* interpret joystick button */
void FlightInput::do_joystick_button(int bt, Flight &flt)
{
  switch ((int) bt)
    {
    case FI_TRIGGER_BUTTON:
      flt.controls.bang_bang = 1;
      break;

    case FI_BUTTON_2:
      flt.controls.armed_w = !flt.controls.armed_w;
      break;

    case FI_BUTTON_1:
      if (throttle_device == mouse_ctrl)
	flt.controls.cockpit = !flt.controls.cockpit;
      else
	flt.controls.speed_brakes = !flt.controls.speed_brakes;
      break;

    case FI_BUTTON_3:
      flt.controls.view = fv_track;
      break;

    case FI_CH_FRWRD:
      flt.controls.view = fv_front;
      break;

    case FI_CH_RIGHT:
      flt.controls.view = fv_right;
      break;

    case FI_CH_BACK:
      flt.controls.view = fv_rear;
      break;

    case FI_CH_LEFT:
      flt.controls.view = fv_left;
      break;
    }
}

