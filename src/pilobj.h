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
#ifndef pilobj_h
#define pilobj_h

#include "flight.h"
#include "weapons.h"

enum pursuit_types { lead, lag, pure };

enum turn_dirs { left_turn, right_turn };

class Pilot_Params
{
public:
  char  id[16];            // id
  int   skill_level;       // general non-specific skill level
  int   affiliation;       // whose side we're on
  float update_time;       // time interval between updates
  float v_acquire_range;   // range for visual target acquisition  (in miles)
  float hitScaler;		   // marksmanship factor
  float shootRadius;			// when to start firing
  float posG;              // positive g limit
  float negG;              // negative g limit
  int   log;               // maintain pilot log
	
  Pilot_Params()
  {
    skill_level = 10;
    affiliation = 0;
    update_time = 0.1;
	 v_acquire_range = 3.0 * 5280.0;
	 hitScaler = 1.3;
	 shootRadius = _degree * 2.0;
	 posG = 8.0;
	 negG = -3.0;
	 log = 0;
  }
  friend istream &operator>>(istream &, Pilot_Params &);
};

class Pilot;

class PilotMessage
{
public:
  int flag;
  float time_limit;
  float elapsed_time;
  char *mssg;
  Pilot *frm;
  PilotMessage()
  {
    flag = 0;
    time_limit = 1.0;
    elapsed_time = 0.0;
    mssg = NULL;
    frm = NULL;
  }
  void update();
  int set_message(char *mss, int priority, Pilot *fr);
};

#endif