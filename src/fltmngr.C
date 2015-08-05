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
 *           Sabre Fighter Plane Simulator       *
 * File   : fltmngr.C                            *
 * Date   : March, 1997                          *
 * Author : Dan Hammer                           *
 *************************************************/
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <limits.h>
#include <values.h>
#include "defs.h"
#include "sim.h"
#include "vmath.h"
#include "port_3d.h"
#include "cpoly.h"
#include "obj_3d.h"
#include "copoly.h"
#include "flight.h"
#include "fltlite.h"
#include "weapons.h"
#include "pilot.h"
#include "zview.h"
#include "fltzview.h"
#include "unguided.h"
#include "simfile.h"
#include "simsnd.h"
#include "fltmngr.h"
#include "siminput.h"
#include "simfilex.h"
#include "aiflite.h"
#include "sfrmtn.h"

extern bool gpause;
extern bool do_random;
extern bool no_crash;

//#define F2FIX(a) (a)
//#define FIX2F(a) (a)

void Flight_Node::read(std::istream &is)
{
  char buff[64];
  int n;
  char c;
  Flight_Specs *spcs = NULL;
  Z_Node_Manager *zm = NULL;

  READ_TOKI('[',is,c)

    if (simfileX::readinput(is,buff,sizeof(buff),n) == simfileX::STR_INPUT)
      {
	spcs = Flight_Specs::getSpecs(buff);
	if (spcs == NULL)
	  {
	    printf("Couldn't locate specs: %s\n",buff);
	    spcs = Flight_Specs::g_specs[0];
	  }
      }
    else
      {
	MYCHECK(n >= 0 && n < Flight_Specs::nspecs);
	spcs = Flight_Specs::g_specs[n];
      }
  MYCHECK(spcs != NULL);
  flight = new Flight(spcs);

  // read in flight params
  is >> *(flight);
  // RANDOMize params
  if (do_random)
    {
      Port_3D *prt = & flight->state.flight_port;
      Vector v(C(frand(C(10000.0) * world_scale)), C(frand(C(10000.0) * world_scale)), C(frand(C(3000.0) * world_scale)));
      prt->delta_look_from(v);
      prt->set_theta(frand(SABRE_2PI));
      flight->state.velocity.direction = prt->view_normal;
    }
  // set sound toggles
  gear_sound_toggle = flight->controls.landing_gear;
  flaps_sound_toggle = flight->controls.flaps == 0.0;
  speed_brakes_sound_toggle = flight->controls.speed_brakes;

  // Put on autopilot if we're not the player
  if (!user_node)
    flight->controls.autopilot = 1;

  if (simfileX::readinput(is,buff,sizeof(buff),n) == simfileX::STR_INPUT)
    {
      zm = Z_Node_Manager::getZNode(buff);
      if (zm == NULL)
	{
	  printf("Couln't locate zm: %s\n", buff);
	  zm = Z_Node_Manager::g_zmanagers[0];
	}
    }
  else
    {
      MYCHECK(n >= 0 && n < Z_Node_Manager::nzmanagers);
      zm = Z_Node_Manager::g_zmanagers[n];
    }
  viewer = new Flight_ZViewer(zm);
  viewer->set_flight(flight);


  // Get weapon list
  if (simfileX::readinput(is,buff,sizeof(buff),n) == simfileX::STR_INPUT)
    wi = wpm.build_instance_list(n,&n_weaps,buff);
  else
    wi = wpm.build_instance_list(n,&n_weaps);

  MYCHECK(wi != NULL);

  viewer->setWeaponInstanceList(new Weapon_Instance_List(wi,n_weaps));

  // Create pilot. If this is not a user node,
  // set the controls of the flight to autopilot
  is >> n;
  MYCHECK(pilot_params[n] != NULL);
  pilot = new Pilot(flight,pilot_params[n],
		    wi,n_weaps,
		    NULL,
		    viewer);
  pilot->SetFlightModelUpdateFlag(0);
  if (Pilot::npilots < Pilot::maxpilots - 1)
    Pilot::pilots[Pilot::npilots++] = pilot;
  // check for waypoint
  is >> n;
  is >> handle;
  handle[16] = '\0';
  is >> c;
  READ_TOK(']',is,c)
    pilot->set_handle(handle);
  pilot->init();
  /* set the affiliation */
  viewer->affiliation = pilot->getAffiliation();
}

std::istream &operator >>(std::istream &is, Flight_Node &fn)
{
  fn.read(is);
  return is;
}

/*
#define DEBUG_WEAPS
*/

/******************************************************
 * Update the flight                                  *
 *****************************************************/
void Flight_Node::update(Unguided_Manager *um)
{
  int weapon_fired = 0;
  int landing_attempted = 0;
  int bad_landing = 0;
  int crash_and_burn = 0;

  // Skip if crashed
  if (flight->state.crashed)
    {
      /* 
	 If the pilot's still in there,
	 well, no more cloud-hopping for
	 him
      */
      pilot->set_functioning(0);
      pilot->set_target(NULL);
      pilot->set_dbg("DEAD");
      update_sound(0,0,0,0);
      return;
    }
  
  // Make sure the Target::position member is updated!
  viewer->position = flight->state.flight_port.look_from;

  /* See if autopilot is on */
  if (pilot && flight->controls.autopilot != 0)
    {
      if (user_node && !user_auto)
	{
	  pilot->start();
	  user_auto = 1;
	}
      pilot->update(1);
    }
  else 
    {
      if (user_auto)
	{
	  pilot->pause();
	  user_auto = 0;
	}
      // Maintain tracking of target for hud
      pilot->update(0);
    }
  weapon_fired = 0;

  /* Check for a weapons launch */
  if (um != NULL && pilot->get_sel_weapon() != NULL)
    {
      /* 
	 don't let user fire unless they've armed the weapon
	 (controls.gunsight is 1)
      */
      if (view_node)
	{
	  if (!flight->controls.armed_w && !flight->controls.autopilot)
	    flight->controls.bang_bang = 0;
	}
      weapon_fired = pilot->get_sel_weapon()->update(*flight,um,viewer);
      if (weapon_fired)
	viewer->shots += weapon_fired;
    }

  /*If crashing possible, check for one*/
  if (!flight->state.crashed)
    {
      landing_attempted = viewer->landing_report(lr);
      if (landing_attempted && lr.score < 0)
	bad_landing = 1;
      if (flight->state.crashed)
	{
	  crash_and_burn = 1;
	  R_3DPoint p = flight->state.flight_port.look_from;
	  if (um)
	    um->boom(p,0);
	}
    }
  /* update sounds */
  update_sound(weapon_fired, landing_attempted, bad_landing, crash_and_burn);
}

/************************************************************
 * update sound effects 
 ************************************************************/
void Flight_Node::update_sound(int weapon_fired, int landing_attempted,
			       int bad_landing, int crash_and_burn)
{
  int	vol = 100;
  int	wantWind = 0;
  if (!sound_get_active())
    return;
  /*
   *  If not current view node, only do crash & burn sound
   */
  if (!view_node)
    {
      if (crash_and_burn)
	{
	  vol = sound_calc_distant_vol(flight->state.flight_port.look_from,9000.0);
	  sound_on(SNDGLOBAL_CRASHBURN,NORM,vol);
	}
      return;
    }

  /*
   *  Update sound affiliation
   */
  sound_set_affiliation(pilot->getAffiliation());

  /*
   * Play either engine sound or wind
   */
  if (flight->state.crashed)
    wantWind = 1;
  else
    {
      /*
       *  Reduce volume of all effects if we're in an outside view
       */
      if (!IS_COCKPIT_VIEW(flight->controls.view))
	{
	  wantWind = 1;
	  vol = sound_calc_distant_vol(flight->state.flight_port.look_from,1000.0);
	  vol = vol - 30;
	  if (vol < 0)
	    vol = 0;
	}
		
      /*
       *	Adjust engine sound
       */
      if ((flight->controls.throttle > 10) &&	(viewer->hurt < viewer->max_damage))
	{
	  sound_on(flight->specs->model,LOOP,vol);
	  sound_freq(flight->specs->model,static_cast<int>(flight->controls.throttle * 100));
	}
      else
	{
	  sound_off(const_cast<const char *>(flight->specs->model));
	  wantWind = 1;
	}
    }
  if (wantWind)
    sound_on(SNDGLOBAL_WIND,LOOP);
  else
    sound_off(SNDGLOBAL_WIND);

  /*
   * Turn on weapon firing sound
   */
  if (weapon_fired)
    sound_on(WPSPECS(pilot->get_sel_weapon())->wep_name,NORM,vol);
  /*
   * Hit by weapon?
   */
  if (viewer->hitme)
    sound_on(SNDGLOBAL_HITME,NORM,vol);

  /*
   *  Landing gear, flaps & speed brakes
   */
  if (gear_sound_toggle != flight->controls.landing_gear)
    sound_on(SNDGLOBAL_GEAR,NORM,vol);
  if (flaps_sound_toggle != (flight->controls.flaps == 0.0))
    sound_on(SNDGLOBAL_FLAPS,NORM,vol);
  if (speed_brakes_sound_toggle != flight->controls.speed_brakes)
    sound_on(SNDGLOBAL_SPEED_BRAKES,NORM,vol);
  gear_sound_toggle = flight->controls.landing_gear;
  flaps_sound_toggle = flight->controls.flaps == 0.0;
  speed_brakes_sound_toggle = flight->controls.speed_brakes;

  /*
   * Landing attempted
   */
  if (landing_attempted && flight->controls.landing_gear)
    sound_on(SNDGLOBAL_SCREECH,NORM,vol);

  /*
   * Bad landing?
   */
  if (bad_landing && !crash_and_burn)
    sound_on(SNDGLOBAL_BADLANDING,NORM,vol);
  else if (crash_and_burn)
    sound_on(SNDGLOBAL_CRASHBURN,NORM,vol);
}

void Flight_Node::stop_engine_sound()
{
  sound_off(const_cast<const char *>(flight->specs->model));
}

void Flight_Node::add_draw_list(DrawList &dlist, Port_3D &port)
{
  if (view_node && IS_COCKPIT_VIEW(flight->controls.view) && !gpause)
    return;
  viewer->draw_prep(port);
  viewer->cpk_flg = 0;
  if (viewer->visible_flag)
    dlist.add_object(viewer);
}

Flight_Manager::~Flight_Manager()
{
  if (specs != NULL)
    {
      for (int i=0;i<n_specs;i++)
	{
	  if (specs[i] != NULL)
	    delete specs[i];
	}
      delete [] specs;
      Flight_Specs::g_specs = NULL;
    }
  if (z_managers != NULL)
    {
      for (int i=0;i<n_managers;i++)
	if (z_managers[i] != NULL)
	  delete z_managers[i];
      delete [] z_managers;
      Z_Node_Manager::g_zmanagers = NULL;
    }
  if (p_params != NULL)
    {
      for (int i=0;i<n_pparams;i++)
	if (p_params[i])
	  delete p_params[i];
      delete [] p_params;
    }
  if (flight_nodes != NULL)
    {
      for (int i=0;i<n_flights;i++)
	if (flight_nodes[i])
	  delete flight_nodes[i];
      delete [] flight_nodes;
    }
  aiFlite::FlushaiFlites();
  aiPilot::FlushaiPilots();
}

int Flight_Manager::read_file(const char *path)
{
  int result = 0;
  std::ifstream infile;

  if (open_is(infile,path))
    {
      result = 1;
      infile >> *this;
      infile.close();
    }
  return result;
}

void Flight_Manager::read(std::istream &is)
{
  char buff[BUFSIZ];
  bool got_line;
  std::ifstream is2;
  int i;

  is.seekg(0L);
  // Get flight specification file(s)
  got_line = get_line(is,buff,sizeof(buff));
  MYCHECK(got_line);
  n_specs = atoi(buff);
  MYCHECK(n_specs > 0);
  // Create specs
  specs = new Flight_Specs *[n_specs];
  MYCHECK(specs != NULL);
  Flight_Specs::nspecs = n_specs;
  Flight_Specs::g_specs = specs;
  for (i = 0; i < n_specs; i++)
    {
      got_line = get_line(is,buff,sizeof(buff));
      MYCHECK(got_line);
      sim_printf("Reading spec file: %s\n",
	     buff);
      specs[i] = new Flight_Specs();
      MYCHECK(specs[i] != NULL);
      specs[i]->read_file(buff);
    }

  // Get shape file(s)
  got_line = get_line(is,buff,sizeof(buff));
  MYCHECK(got_line);
  // Create z_managers
  n_managers = atoi(buff);
  MYCHECK(n_managers > 0);
  z_managers = new Z_Node_Manager *[n_managers];
  MYCHECK(z_managers != NULL);
  for (i=0;i<n_managers;i++)
    {
      z_managers[i] = new Z_Node_Manager();
      MYCHECK(z_managers[i] != NULL);
      got_line = get_line(is,buff,sizeof(buff));
      MYCHECK(got_line);
      sim_printf("Reading shape file: %s\n",buff);
      z_managers[i]->read_file(buff);
      z_managers[i]->setId(buff);
    }
  Z_Node_Manager::nzmanagers = n_managers;
  Z_Node_Manager::g_zmanagers = z_managers;

  // Get pilot specification file
  got_line = get_line(is,buff,sizeof(buff));
  MYCHECK(got_line);
  sim_printf("Reading pilot spec file: %s\n",
	     buff);
  if (open_is(is2,buff))
    {
      // Create pilot parameters array
      n_pparams = read_int(is2);
      p_params = new Pilot_Params *[n_pparams];
      MYCHECK(p_params != NULL);
      for (i=0;i<n_pparams;i++)
	{
	  p_params[i] = new Pilot_Params();
	  MYCHECK(p_params[i] != NULL);
	  is2 >> *(p_params[i]);
	}
      is2.close();
    }

  // Get weapons specification file
  got_line = get_line(is,buff,sizeof(buff));
  MYCHECK(got_line);
  sim_printf("Reading weapons spec file: %s\n",
	     buff);
  wm.read_file(buff);

  // Get waypoints specification file
  got_line = get_line(is,buff,sizeof(buff));
  MYCHECK(got_line);
  /*
    waypoints now in "group" file --
    ignored here
    if (*buff == '@')
    WayPoint::readWayPoints(is);
    else
    WayPoint::readWayPointFile(buff);
  */
  // Get nodes file
  got_line = get_line(is,buff,sizeof(buff));
  MYCHECK(got_line);
  sim_printf("Reading nodes file: %s\n",
	     buff);
  if (*buff == '@')
    readNodes(is);
  else
    readNodeFile(buff);
  got_line = get_line(is,buff,sizeof(buff));
  if (got_line)
    {
      sim_printf("Reading group file: %s\n",
		 buff);
      readFliteFile(buff);
    }
}

std::istream & operator >>(std::istream &is, Flight_Manager &fm)
{
  fm.read(is);
  return is;
}

void Flight_Manager::readNodes(std::istream &is)
{
  int i;

  Pilot::npilots = 0;
  // Create flight nodes
  is >> n_flights;
  flight_nodes = new Flight_Node *[n_flights];
  MYCHECK(flight_nodes != NULL);
  for (i=0;i<n_flights;i++)
    {
      flight_nodes[i] = new Flight_Node(specs,z_managers,
					p_params,wm);
      MYCHECK(flight_nodes[i] != NULL);
      if (i == 0)
	{
	  flight_nodes[i]->user_node = 1;
	  flight_nodes[i]->view_node = 1;
	}
      is >> *(flight_nodes[i]);
    }
}

void Flight_Manager::readNodeFile(const char *path)
{
  std::ifstream is;

  if (open_is(is,path))
    readNodes(is);
}


void Flight_Manager::readFlites(std::istream &is)
{
#define MAX_SWAYPOINT_INFOS 16
  swaypoint_info waypoints[MAX_SWAYPOINT_INFOS];
  int nwaypoints;
  int nflites;
  int npilots;
  char fliteId[aiB_HANDLE_SIZE];
  char pilotHandle[aiB_HANDLE_SIZE];
  char waypointTaskStr[32];
  char formationTypeStr[32];
  char maneuverTypeStr[32];
  char c;
  aiFlite *theFlite = NULL;
  int i,j;
#define NWPTASKS 15
  static simfileX::dict wptask[NWPTASKS] =
  {
    { "navigate", swpNAVIGATE },
    { "cap", swpCAP },
    { "intercept", swpINTERCEPT },
    { "engage_player", swpENGAGE_PLAYER },
    { "engage_flite", swpENGAGE_FLITE },
    { "strafe", swpSTRAFE_ATTACK },
    { "rocket", swpROCKET_ATTACK },
    { "level_bomb", swpLEVEL_BOMB	},
    { "dive_bomb", swpDIVE_BOMB },
    { "torpedo_bomb", swpTORPEDO_BOMB },
    { "airshow", swpAIRSHOW },
    { "targets", swpTARGETS },
    { "takeoff", swpTAKEOFF },
    { "land", swpLAND },
    { "maneuver", swpMANEUVER }
  };
#define NFRMTNTYPES 8
  static simfileX::dict frmtntype[NFRMTNTYPES] =
  {
    { "wedge", sfWEDGE },
    { "vic", sfVIC },
    { "echelon_right", sfECHELON_R },
    { "echelon_left", sfECHELON_L },
    { "trail", sfTRAIL },
    { "ladder", sfLADDER },
    { "diamond", sfDIAMOND },
    { "fighting_wing", sfFIGHTING_WING }
  };
#define NMNVRTYPES 16
  static simfileX::dict mnvrtypes[NMNVRTYPES] =
  {
    { "straight_and_level", sManeuver::STRAIGHT_AND_LEVEL },
    { "standard_turn", sManeuver::STANDARD_TURN },
    { "climb", sManeuver::CLIMB },
    { "descend", sManeuver::DESCEND },
    { "break_turn", sManeuver::BREAK_TURN },
    { "hard_turn", sManeuver::HARD_TURN },
    { "immelman", sManeuver::IMMELMAN },
    { "split_s", sManeuver::SPLIT_S },
    { "jink", sManeuver::JINK },
    { "wingover", sManeuver::WINGOVER },
    { "climbing_turn", sManeuver::CLIMBING_TURN },
    { "snap_roll", sManeuver::SNAP_ROLL },
    { "chandelle", sManeuver::CHANDELLE },
    { "zoom", sManeuver::ZOOM },
    { "barrel_roll", sManeuver::BARREL_ROLL },
    { "spiral_dive", sManeuver::SPIRAL_DIVE }
  };

  aiFlite::SetOwnership(1);
	
  nflites = simfileX::readint(is,'(',')');
  if (nflites > aiMAX_FLITES)
    nflites = aiMAX_FLITES;
  for (i=0;i<nflites;i++)
    {
      READ_TOKI('{',is,c);

      simfileX::readstr(is,fliteId,sizeof(fliteId),'"','"');
      npilots = simfileX::readint(is,'(',')');
      if (npilots > 0)
	{
	  theFlite = new aiFlite(npilots,0,fliteId);
	  for (j=0;j<npilots;j++)
	    {
	      simfileX::readstr(is,pilotHandle,sizeof(pilotHandle),'"','"');
	      aiPilot *pil = aiPilot::GetaiPilot(pilotHandle);
	      if (pil)
		theFlite->Add(pil);
	    }
	}
      nwaypoints = simfileX::readint(is,'(',')');
      if (nwaypoints > 0)
	{
	  if (nwaypoints > MAX_SWAYPOINT_INFOS)
	    nwaypoints = MAX_SWAYPOINT_INFOS;
	  for (j=0;j<nwaypoints;j++)
	    {
	      READ_TOKI('{',is,c);
	      swaypoint_info &waypoint_info = waypoints[j];

	      simfileX::readdictinput(is,waypointTaskStr,sizeof(waypointTaskStr),
				      waypoint_info.task,wptask,NWPTASKS);
	      if (waypoint_info.task < swpNAVIGATE)
		waypoint_info.task = swpNAVIGATE;
	      if (waypoint_info.task > swpMANEUVER)
		waypoint_info.task = swpMANEUVER;

	      simfileX::readdictinput(is,formationTypeStr,sizeof(formationTypeStr),
				      waypoint_info.formation_type,
				      frmtntype,NFRMTNTYPES);
	      if (waypoint_info.formation_type < sfWEDGE)
		waypoint_info.formation_type = sfWEDGE;
	      if (waypoint_info.formation_type > sfFIGHTING_WING)
		waypoint_info.formation_type = sfFIGHTING_WING;

	      READ_TOKI('(',is,c);
	      is >> waypoint_info.to_x >> waypoint_info.to_y >> waypoint_info.to_z ;
	      is >> waypoint_info.speed >> waypoint_info.altitude >> waypoint_info.time ;
	      waypoint_info.speed = kts2fps(waypoint_info.speed);
	      READ_TOKI(')',is,c);
							
	      READ_TOKI('(',is,c);
	      if (waypoint_info.task == swpMANEUVER)
		simfileX::readdictinput(is,maneuverTypeStr,sizeof(maneuverTypeStr),
					waypoint_info.i_task_modifier[0],
					mnvrtypes,NMNVRTYPES);
	      else
		is >> waypoint_info.i_task_modifier[0];
	      is >> waypoint_info.i_task_modifier[1];
	      is >> waypoint_info.i_task_modifier[2];
	      READ_TOKI(')',is,c);

	      READ_TOKI('(',is,c);
	      is >> waypoint_info.d_task_modifier[0] >> waypoint_info.d_task_modifier[1]
		 >> waypoint_info.d_task_modifier[2];
	      READ_TOKI(')',is,c);

	      simfileX::readstr(is,waypoint_info.c_task_modifier,
				sizeof(waypoint_info.c_task_modifier),'"','"');
	      READ_TOKI('}',is,c);
	    }
	  theFlite->SetWaypoints(waypoints,nwaypoints);
	}
      if (theFlite)
	{
	  aiFlite::AddaiFlite(theFlite);
	  theFlite->SetNewTargetDelayTime(1.0);
	  theFlite->Init();
	}
      READ_TOKI('}',is,c);
    }
}

void Flight_Manager::readFliteFile(const char *path)
{
  std::ifstream is;

  if (open_is(is,path))
    readFlites(is);
}

void Flight_Manager::set_view_node(int vn)
{
  if (vn >= n_flights)
    vn = 0;
  else if (vn < 0)
    vn = n_flights - 1;
  flight_nodes[view_node]->view_node = 0;
  flight_nodes[view_node]->stop_engine_sound();
  sound_set_affiliation(flight_nodes[view_node]->pilot->getAffiliation());
  view_node = vn;
  flight_nodes[view_node]->view_node = 1;
}

bool Flight_Manager::select_next_target(int fr)
{
  int i;
  bool foundit = 0;
  int ii = flight_nodes[fr]->target_node;
  for (i=ii+1;i<n_flights;i++)
    {
      if (
	  (i != fr)
	  && (flight_nodes[fr]->pilot->get_target_flight() !=
	      flight_nodes[i]->flight)
	  )
	{
	  flight_nodes[fr]->pilot->set_target(flight_nodes[i]->pilot);
	  flight_nodes[fr]->target_node = i;
	  foundit = 1;
	  break;
	}
    }

  if (!foundit)
    {
      for (i=0;i<ii;i++)
	{
	  if (
	      (i != fr)
	      && (flight_nodes[fr]->pilot->get_target_flight() !=
		  flight_nodes[i]->flight)
	      )
	    {
	      flight_nodes[fr]->pilot->set_target(flight_nodes[i]->pilot);
	      flight_nodes[fr]->target_node = i;
	      foundit = 1;
	      break;
	    }
	}
    }
  return (foundit);
}

void Flight_Manager::update(Unguided_Manager *um)
{
  int i;
  um->check_hits();
  for (i=0;i<n_flights;i++)
    {
      if (!flight_nodes[i]->flight->state.crashed)
	flight_nodes[i]->pilot->update_sbrflight();
    }
  um->update();
  for (i=0;i<n_flights;i++)
    flight_nodes[i]->update(um);
  aiFlite::aiFlitesUpdate(time_frame);
}

void Flight_Manager::start()
{
  for (int i=0;i<n_flights;i++)
    flight_nodes[i]->start();
}

void Flight_Manager::pause()
{
  for (int i=0;i<n_flights;i++)
    flight_nodes[i]->pause();
}

void Flight_Manager::add_draw_list(DrawList &dlist, Port_3D &port)
{
  for (int i=0;i<n_flights;i++)
    flight_nodes[i]->add_draw_list(dlist,port);
}

inline float get_diff(float port_angle, float goal_angle)
{
  float result;
  result = C(fabs(port_angle - goal_angle));
  if (result > SABRE_PI)
    result = C(fmod(SABRE_2PI, result));
  if (port_angle > goal_angle)
    result = -result;
  return result;
}

void Flight_Manager::set_flight_view(Port_3D &vport)
{
  R_3DPoint lf,la,r1,r2,tpoint;
  Vector v,v1;
  REAL_TYPE ang = half_pi * C(0.333);
  REAL_TYPE ang1 = one_fourth_pi;

  if (view_node != v_4_node)
    {
      v_4_flag = 0;
      v_4_node = -1;
    }

  Flight &flt = *(flight_nodes[view_node]->flight);

  vport = flt.state.flight_port;
  Port_3D &fport = flt.state.flight_port;

  if (flt.controls.view != fv_flyby)
    v_4_flag = 0;
	
  if (IS_COCKPIT_VIEW(flt.controls.view))
    {
      R_3DPoint vp = flt.specs->view_point;
      Z_Node_Manager &zm = *(flight_nodes[view_node]->viewer->z_manager);
      vp *= zm.scaler * world_scale;
      fport.port2world(vp,&lf);
      v = Vector(lf - flt.state.flight_port.look_from);
      vport.delta_look_from(v);
    }
	
  switch (flt.controls.view)
    {
    case fv_virtual:
      vport.rotate(flt.controls.virtPhi,0.0,flt.controls.virtTheta);
      break;

    case fv_front:
      break;

    case fv_front_up:
      vport.rotate(-ang,0.0,0.0);
      break;

    case fv_front_down:
      vport.rotate(ang,0.0,0.0);
      break;

    case fv_front_left:
      vport.rotate(0.0,0.0,-ang1);
      break;

    case fv_front_left_up:
      vport.rotate(-ang,0.0,-ang1);
      break;

    case fv_front_left_down:
      vport.rotate(ang,0.0,-ang1);
      break;

    case fv_front_right:
      vport.rotate(0.0,0.0,ang1);
      break;

    case fv_front_right_up:
      vport.rotate(-ang,0.0,ang1);
      break;

    case fv_front_right_down:
      vport.rotate(ang,0.0,ang1);
      break;

    case fv_left:
      vport.rotate(0.0,0.0,-half_pi);
      break;

    case fv_left_up:
      vport.rotate(-ang,0.0,-half_pi);
      break;

    case fv_left_down:
      vport.rotate(ang,0.0,-half_pi);
      break;

    case fv_right:
      vport.rotate(0.0,0.0,half_pi);
      break;

    case fv_right_up:
      vport.rotate(-ang,0.0,half_pi);
      break;

    case fv_right_down:
      vport.rotate(ang,0.0,half_pi);
      break;

    case fv_rear:
      vport.rotate(0.0,0.0,pi);
      break;

    case fv_rear_up:
      vport.rotate(-ang,0.0,pi);
      break;

    case fv_rear_down:
      vport.rotate(ang,0.0,pi);
      break;

    case fv_rear_left:
      vport.rotate(0.0,0.0,pi+ang1);
      break;

    case fv_rear_left_up:
      vport.rotate(-ang,0.0,pi+ang1);
      break;

    case fv_rear_left_down:
      vport.rotate(ang,0.0,pi+ang1);
      break;

    case fv_rear_right:
      vport.rotate(0.0,0.0,pi-ang1);
      break;

    case fv_rear_right_up:
      vport.rotate(-ang,0.0,pi-ang1);
      break;

    case fv_rear_right_down:
      vport.rotate(ang,0.0,pi-ang1);
      break;
		
    case fv_external:
    l1:
    vport.set_roll(0);
    vport.set_phi(C(1.57));
    vport.set_theta(0);
    vport > flt.controls.vphi;
    vport < flt.controls.vtheta;
    v = to_nvector(vport.look_from,vport.look_at);
    v *= (flt.controls.vdist * world_scale);
    r1 = R_3DPoint(v) + vport.look_from;
    v1 = to_nvector(r1,vport.look_from);
    r2 = R_3DPoint(v1) + r1;
    vport.set_view(r1,r2);
    break;

    case fv_padlock:
      {
	Pilot *tpilot;
	R_3DPoint lf2;
	tpilot = flight_nodes[view_node]->pilot->get_target_pilot();
	if (tpilot == NULL)
	  goto l1;
	lf2 = tpilot->get_position();
	vport.rotate_to_point(lf2);
      }
      break;

    case fv_track:
    case fv_target_track:
      {
	Pilot *pilot;
	Port_3D *vp;
	if (flt.controls.view == fv_track)
	  {
	    pilot = flight_nodes[view_node]->pilot;
	    vp = &vport;
	  }
	else
	  {
	    pilot = flight_nodes[view_node]->pilot->get_target_pilot();
	    if (pilot == NULL)
	      goto l1;
	    vp = pilot->get_port();
	    vport = *vp;
	  }

	if (pilot->get_target_flight() &&
	    pilot->get_target_pilot())
	  {
	    REAL_TYPE vx = C(12.0) * world_scale;
	    R_3DPoint p1 = vp->look_from + R_3DPoint(vx,vx,vx);
	    tpoint = pilot->get_target_pilot()->get_position();
	    v = to_nvector(tpoint,p1);
	    v *= (flt.controls.vdist * world_scale);
	    r1 = R_3DPoint(v) + p1;
	    v1 = to_nvector(r1,tpoint);
	    r2 = R_3DPoint(v1) + r1;
	    vport.set_view(r1,r2);
	    vport.set_roll(0);
	  }
	else
	  goto l1;
      }
      break;

    case fv_satellite:
      {
	float x,y,z;
	float d;
	x = vport.look_from.x;
	y = vport.look_from.y;
	z = vport.look_from.z;
	d = flt.controls.vdist * world_scale;
	z += d;
	lf = R_3DPoint(x,y,z);
	la = seg2point(lf,vport.look_from);
	vport.set_view(lf,la);
	vport.set_roll(0);
	break;
      }

    case fv_flyby:
      if (!v_4_flag)
	{
	  vport.set_roll(0);
	  vport.set_phi(C(1.57));
	  vport.set_theta(0);
	  vport > flt.controls.vphi;
	  vport < flt.controls.vtheta;
	  v = to_nvector(vport.look_from,vport.look_at);
	  v *= (flt.controls.vdist * world_scale);
	  r1 = R_3DPoint(v) + vport.look_from;
	  v1 = to_nvector(r1,vport.look_from);
	  r2 = R_3DPoint(v1) + r1;
	  vport.set_view(r1,r2);
	  v_4_port = vport;
	  v_4_flag = 1;
	  v_4_node = view_node;
	}
      else
	{
	  v_4_port.set_view(v_4_port.look_from,vport.look_from);
	  vport = v_4_port;
	}
      break;
    }
  flt.controls.vPoint = vport.look_from;
  sound_set_viewpoint(vport.look_from);
}
