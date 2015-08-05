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
/******************************************************
 * siminput.h                                         *
 ******************************************************/
#ifndef SIMINPUT_H
#define SIMINPUT_H

#include "sbrvkeys.h"

// Input routines
class Mouse;
class Joystick;

typedef struct tagKeyMapEntry
{
  int	fi_cmd;
  int	fi_cmd_ctrl;
  int	fi_cmd_alt;
} KeyMapEntry;

class FlightInput
{
public:
  int yoke_device;
  int rudder_device;
  int throttle_device;
  
  int m_throttle, m_rudder;
  Mouse *mouse;
  Joystick *y_joy;
  Joystick *r_joy;
  Joystick *t_joy;
  FlightInput(Mouse *ms, Joystick *yj, Joystick *tj, Joystick *rj, 
	      int mt, int mr);
  void set_devices(Mouse *ms, Joystick *yj, Joystick *tj, Joystick *rj);
  int update(Flight &, int key);
  int yoke_key(Flight &, int key);
  void yoke_mouse(Flight &);
  void yoke_joystick(Flight &);
  void throttle_mouse(Flight &flt);
  void rudder_mouse(Flight &flt);
  void throttle_joy(Flight &flt);
  void rudder_joy(Flight &flt);
  void do_joystick_button(int bt, Flight &flt);

  enum { kbd_ctrl, mouse_ctrl, joy_ctrl };

  static int kbdin;
  static int kbdin_lock;
  static int SwitchOnKey(int key, Flight_Node &node, Flight_Manager &fm); 
  static int UpdateFlightInput(FlightInput & fi, Flight_Node &node, Flight_Manager &fm);
  static void SetKeybdin(int val)
    {
      if (!kbdin_lock)
	{
	  kbdin = val;
	  kbdin_lock = 1;
	}
    }
};

#endif




