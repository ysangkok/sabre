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
/******************************************************
 * fltmngr.h                                          *
 ******************************************************/
#ifndef __fltmngr_h
#define __fltmngr_h
#include "flight.h"
#include "pilot.h"

class Flight_Node
{
public:
	Flight          *flight;
	Flight_ZViewer  *viewer;
	Pilot           *pilot;
	int             user_node;
	int             user_auto;
	int             target_node;
	int             view_node;
	Flight_Specs    **specs;
	Z_Node_Manager  **z_managers;
	Pilot_Params    **pilot_params;
	Weapons_Manager &wpm;
	Weapon_Instance *wi;
	int             n_weaps;
	char            handle[17];
	Landing_Report	 lr;
	int             gear_sound_toggle;
	int             flaps_sound_toggle;
	int             speed_brakes_sound_toggle;

	Flight_Node(Flight_Specs **spcs, Z_Node_Manager **zm,
					Pilot_Params **prms, Weapons_Manager &wp)
					:specs(spcs),
					z_managers(zm),
					pilot_params(prms),
					wpm(wp)
	{
		flight = NULL;
		viewer = NULL;
		pilot = NULL;
		wi = NULL;
		user_node = 0;
		user_auto = 0;
		n_weaps = 0;
		view_node = 0;
		target_node = -1;
	}

	~Flight_Node()
	{
		if (pilot)
			delete pilot;
		if (viewer)
			delete viewer;
		if (flight)
			delete flight;
		if (wi)
			delete [] wi;
	}

	friend istream &operator >>(istream &, Flight_Node &);
	void read(istream &);

	void start()
	{
		if (pilot && flight->controls.autopilot)
			pilot->start();
		if (flight)
			flight->start();
	}

	void pause()
	{
		if (flight)
			flight->pause();
		if (pilot && flight->controls.autopilot)
			pilot->pause();
	}
	void update(Unguided_Manager *um = NULL);
	void add_draw_list(DrawList &, Port_3D &);
	void update_sound(int weapon_fired,	int landing_attempted,
							int bad_landing, int crash_and_burn);
	void stop_engine_sound();
};

class Flight_Manager
{
public:
	Flight_Specs   **specs;
	int            n_specs;
	Z_Node_Manager **z_managers;
	int            n_managers;
	Pilot_Params   **p_params;
	int            n_pparams;
	Weapons_Manager wm;
	Flight_Node    **flight_nodes;
	int            n_flights;
	int            view_node;

	int            v_4_flag;
	int            v_4_node;
	Port_3D        v_4_port;

	Flight_Manager()
	{
		n_specs = n_managers = n_pparams = n_flights = 0;
		specs = NULL;
		z_managers = NULL;
		p_params = NULL;
		flight_nodes = NULL;
		view_node = 0;
		v_4_flag = 0;
		v_4_node = -1;
	}

	~Flight_Manager();
	int read_file(char *);
	friend istream &operator >>(istream &, Flight_Manager &);
	void read(istream &);
	void update(Unguided_Manager *um = NULL);
	void start();
	void pause();
	void add_draw_list(DrawList &, Port_3D &);
	void set_flight_view(Port_3D &);
	int select_next_target(int);
	void set_view_node(int);
	Flight_Node *get_node(int n)
	{
		if (n >= 0 && n < n_flights)
			return (flight_nodes[n]);
		else
			return NULL;
	}
	Flight_Node & get_view_node()
	{ return *flight_nodes[view_node]; }
	Port_3D & get_view_port()
	{ return flight_nodes[view_node]->flight->state.flight_port; }
	Flight & get_view_flight()
	{ return *(flight_nodes[view_node]->flight); }
	Flight *get_flight(int i)
	{
		if (i < n_flights && i >= 0)
			return (flight_nodes[i]->flight);
		else
			return (NULL);
	}
	Pilot *get_pilot(int i)
	{
		if (i < n_flights && i >= 0)
			return (flight_nodes[i]->pilot);
		else
			return (NULL);
	}
	Flight_ZViewer & get_view_viewer()
	{ return *(flight_nodes[view_node]->viewer); }
	Flight_ZViewer *get_viewer(int i)
	{
		if (i < n_flights && i >= 0)
			return (flight_nodes[i]->viewer);
		else
			return NULL;
	}
	void readNodeFile(char *);
	void readNodes(istream &is);
	void readFliteFile(char *);
	void readFlites(istream &is);
};

#endif
