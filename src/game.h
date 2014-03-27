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
 * File   : game.h                               *
 * Date   : March, 1997                          *
 * Author : Dan Hammer                           *
 *************************************************/
#ifndef __game_h
#define __game_h

class GameSession
{
public:
	char                *world_file;
	char                *flight_file;
	char                *ground_unit_file;
	Flight_Manager      fm;
	Unguided_Manager    um;
	Ground_Unit_Manager gm;
	Earth_Watch2        earth;
	C_3DInfoManager     info_manager;
	C_DrawList2         clist;
	Hud                 hud;
	Cockpit             *cpk;
	FlightInput         fi;
	MTimer              timer;
	Palette_Effect      *pe;
	Target_List         target_list;
	int                 pflag;
	int                 zz;
	int                 demo_mode;
	char                *instr_path;
	int                 (*messageHook)(void);
	GameSession(char *world_file,
			char *flight_file,
			char *ground_unit_file,
			Mouse *yoke_mouse, 
			Joystick *yoke_jstk, 
			Joystick *rudder_jstk,
			Joystick *throttle_jstk, 
			int mouse_throttle, 
			int mouse_rudder,
			char *instr_path,
			char *hud_path,
			int = 0,
			int (*) (void) = NULL);
	~GameSession();
	void doGame();
	void setup();
	void readWorldFile();
	void play();
	void demo();
	void get_time();
	void update_flight();
	void draw_flight(Port_3D *pport = NULL);
	void doPaletteEffect();
	void printResults(ostream &);
	void printResult(Flight_Node &, ostream &);
	void __cdecl show_message(int bot, char *str, ... );
	Flight_Controls *getFlightControls()
	{
		Flight &flt = fm.get_view_flight();
		return (&flt.controls);
	}
	Flight_Node *getFliteNode()
	{
		Flight_Node &node = fm.get_view_node();
		return (&node);
	}
	void setDevices(Mouse *yoke_mouse, Joystick *yoke_joystick,
						 Joystick *rudder_joystick, Joystick *throttle_joystick)
	{
		fi.set_devices(yoke_mouse,yoke_joystick,rudder_joystick,throttle_joystick);
	}

	static GameSession *theGame;
};

#endif
