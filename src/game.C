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
 *       Sabre Fighter Plane Simulator           *
 * File   : game.C                               *
 * Date   : March, 1997                          *
 * Author : Dan Hammer                           *
 *************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <values.h>
#include <float.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <ctype.h>
#include <iostream>
#include <fstream>
#include <setjmp.h>
#include <stdarg.h>
#include "sim.h"
#include "siminc.h"
#include "simsnd.h"
#include "game.h"
#include "aipilot.h"
#include "stime.h"
#include "sfrmtn.h"

extern void buffer2ppm();
static float time_expired;
static int firstDemo = 1;
GameSession *GameSession::theGame = NULL;

GameSession::GameSession(const char *wrld_file,
		const char *flight_fil,
		const char *ground_unit_fil,
		Mouse *muse,
		Joystick *yoke_jstk,
		Joystick *rudder_jstk,
		Joystick *throttle_jstk,
		int muse_throttle,
		int muse_rudder,
		const char *instr_pat,
		const char *hud_pat,
		int demo_mod,
		int (*msgHook)(void))
	: 
		clist(&info_manager),
		hud(g_font,hud_pat),
		cpk(NULL),
		fi(muse,yoke_jstk,rudder_jstk,throttle_jstk,
				muse_throttle,muse_rudder)
{
	this->demo_mode = demo_mod;
	pe = NULL;
	this->world_file = wrld_file;
	this->flight_file = flight_fil;
	this->ground_unit_file = ground_unit_fil;
	this->instr_path = instr_pat;
	this->messageHook = msgHook;
	the_earth = &earth;
	time_expired = 0.0;
	Pilot::initPilot();
	theGame = this;
	Unguided_Manager::the_umanager = &um;
}

GameSession::~GameSession()
{
	if (pe != NULL)
		delete pe;
	if (cpk != NULL)
		delete cpk;
	if (theGame == this)
		theGame = NULL;
	if (Unguided_Manager::the_umanager == &um)
		Unguided_Manager::the_umanager = NULL;
}


void GameSession::doGame()
{
	setup();
	if (!demo_mode)
		play();
	else
	{
		if (firstDemo)
		{
			sound_on(SNDGLOBAL_INTRO,NORM,75);
			sound_set_lock(1);
		}
		demo();
		firstDemo = 0;
		sound_set_lock(0);
		sound_off(SNDGLOBAL_INTRO);
	}
	sound_off_all();
	printResults(std::cout);
}

void GameSession::readWorldFile()
{
	std::ifstream is;
	char c;
	Port_3D port;
	float pixl_ratio;
	char buff[100];

	if (open_is(is,world_file))
	{
		// Get path to palette file
		get_line(is,buff,sizeof(buff));
		printf("reading palette file %s\n",	buff);
		read_palette_file(buff);
		// Get path to texture file
		get_line(is,buff,sizeof(buff));
		sim_printf("reading texture file %s\n",
				buff);
		read_texture_file(buff);
		READ_TOKI('{',is,c)
			is >> world_scale >> time_factor >> hit_scaler >> 
			max_time >> shadow_level;
		is >> Port_3D::fovx >> Port_3D::fovy;
		READ_TOK('}',is,c)
			sim_printf("ws: %3.5f tf: %3.7f hs: %3.5f mt: %3.5f  sl: %3.5f\n",
					world_scale, time_factor, hit_scaler,max_time,shadow_level);
		shadow_level *= world_scale;
		is >> world_light_source;
		world_light_source *= world_scale;
		is >> port;
		pixl_ratio = C(SCREEN_WIDTH / 320.0);
		Port_3D::fovx *= pixl_ratio;
		Port_3D::fovy *= pixl_ratio;
		sim_printf("Reading terrain info\n");
		is >> earth;
		sim_printf("Reading static object definitions\n");
		is >> info_manager;
		sim_printf("Reading static object instances\n");
		is >> clist;
	}
	else
	{
		perror("reading world file");
		exit(1);
	}
	cpk = new Cockpit("f9fcpk",instr_path);
}

void GameSession::setup()
{
	pflag = 0;
	zz = 0;

	Weapons_Manager::the_weapons_manager = &fm.wm;
	sFormationData::Init();
	sim_printf("reading world file %s\n",world_file);
	readWorldFile();
	sim_printf("reading flight info %s\n",flight_file);
	fm.read_file(flight_file);
	if (ground_unit_file != NULL)
		gm.read_file(ground_unit_file);
	clist.get_targets(target_list);
	for (int i=0;i<fm.n_flights;i++)
		target_list.add_target(fm.get_viewer(i));
	gm.addTargetList(target_list);
	pe = new Palette_Effect();

	Unguided_Manager::initUnguided();
	Pilot::initPilot();
	sim_printf("%d textures\n",static_cast<unsigned int>(map_man->n_maps));
	sim_printf("%d terrain shapes\n",earth.ntshapes);
	sim_printf("%d static shape definitions\n",info_manager.ninfos);
	sim_printf("%d static shape objects\n",static_cast<unsigned int>(clist.n_objects));
	sim_printf("%d dynamic shape definitions\n",fm.n_managers);
	sim_printf("%d flights\n",fm.n_flights);
	sim_printf("%d ground units\n",gm.nunits);
}

void GameSession::play()
{
	Port_3D port;
	clear_scr(0);
	clear_zbuff();
	zz = 0;
	pflag = 0;
	timer.reset();
	time_expired = 0.0;
	sResetElapsedTime();
	get_time();
	fm.start();

	do
	{
		if (messageHook && !messageHook())
			break;
		get_time();
		if (dump_screen)
		{
			buffer2ppm();
			dump_screen = 0;
		}
		if (gpause == 0)
		{
			pflag = 0;
			draw_flight();
			update_flight();
		}
		else
		{
			if (!pflag)
			{
				pflag = 1;
				fm.set_flight_view(port);
			}
			if (gpause == 1)
				draw_flight(&port);
			if (!port_key(port))
				break;
		}
	} while (FlightInput::UpdateFlightInput(fi,fm.get_view_node(),fm));
	clear_scr(0);
}

inline int GETSTATE(Pilot *pil, char *state, int n)
{
	return(!memcmp(pil->get_dbg(),state,static_cast<size_t>(n)));
}

inline int INTERESTING(Flight_Node &nde)
{
	int result = 0;

	const sManeuverState &mvs = nde.pilot->GetManeuverStackTop();
	if (!nde.pilot->IsActive())
	{
		if (!nde.flight->state.crashed)
			result = 1;
		else
			result = 0;
	}
	else if (nde.flight->state.crashed)
		result = 0;
	else if (nde.flight->controls.bang_bang)
		result = 6;
	else if (nde.viewer->hitme)
		result = 4;
	else if (mvs.maneuver == sManeuver::GUN_ATTACK)
	{
		const sTarget &tg = nde.pilot->GetEngageTarget();
		if (tg.flags.onNose)
			result = 5;
		else
			result = 4;
	}
	else if (mvs.maneuver == sManeuver::PURSUIT)
		result = 3;
	else
	{
		const sManeuverState &mvsbot =
			nde.pilot->GetManeuverStackBottom();
		if (mvsbot.maneuver == sManeuver::ENGAGE)
		{
			if (mvsbot.state == 12)
				result = 6;
			else
				result = 2;
		}
		else
			result = 0;
	}
	return result;
}

inline int DECIDEVIEW()
{
	unsigned int dec = RANDOM(3u);
	switch (dec)
	{
		case 0:
			return(fv_track);

		case 1:
			return(fv_flyby);

		case 2:
			return(fv_front);

		default:
			abort();
	}
}

void GameSession::demo()
{
	Port_3D port;
	Flight *fc;
	float view_time;
	float maxv_time;
	bool flg;
	int maxX,ix,ii,x;

	clear_scr(0);
	clear_zbuff();
	zz = 0;

	for (int i=0;i<fm.n_flights;i++)
	{
		fc = fm.get_flight(i);
		fc->controls.autopilot = 1;
	}

	view_time = 0;
	maxv_time = C(static_cast<float>(RANDOM(6)) + 3.0);
	sResetElapsedTime();
	get_time();
	fm.start();
	do
	{
		if (messageHook && !messageHook())
			break;
		if (gpause)
		{
			if (!port_key(port))
				break;
			continue;
		}
		flg = 0;
		get_time();
		view_time += raw_time;
		Flight_Node &fn = fm.get_view_node();
		if (fn.flight->controls.autopilot)
		{
			maxX = INTERESTING(fn);
			ix = maxX;
			ii = -1;
			if (view_time >= 2.0)
			{
				for (int i=0;i<fm.n_flights;i++)
				{
					if ((x = INTERESTING(*fm.get_node(i))) != 0)
					{
						if (x > maxX)
						{
							ii = i;
							maxX = x;
						}
					}
				}
			}

			if (ii >= 0)
			{
				flg = 1;
				view_time = 0.0;
				fm.set_view_node(ii);
				fc = fm.get_flight(ii);
				fc->controls.view = static_cast<flight_view>(DECIDEVIEW());
			}

			if (view_time >= maxv_time && !flg && ix == 0)
			{
				view_time = 0.0;
				maxv_time = C(static_cast<float>(RANDOM(6)) + 3.0);
				int found_flight = 0;
				for (int i=0;i<fm.n_flights;i++)
				{
					fm.set_view_node(fm.view_node + 1);
					fc = fm.get_flight(fm.view_node);
					if (!fc->state.crashed)
					{
						found_flight = 1;
						break;
					}
				}
				if (!found_flight)
					break;
				else
				{
					fc = fm.get_flight(fm.view_node);
					fc->controls.view = static_cast<flight_view>(DECIDEVIEW());
				}
			}
		}
		draw_flight();
		update_flight();

		if (dump_screen)
		{
			buffer2ppm();
			dump_screen = 0;
		}
		if (firstDemo)
		{
			unsigned int stat = sound_status(SNDGLOBAL_INTRO);
			if (!(stat & SND_STATUS_PLAYING))
			{
				sound_set_lock(0);
				sound_off(SNDGLOBAL_INTRO);
			}
		}
	}
	while (FlightInput::UpdateFlightInput(fi,fm.get_view_node(),fm));
	clear_scr(0);
}

void GameSession::get_time()
{
	time_frame = timer.get_time();
	if (time_frame < 0.01)
		time_frame = C(0.01);
	raw_time = time_frame;
	if (time_frame > max_time)
		time_frame = max_time;
	time_expired += time_frame;
	sSetTimeFrame(time_frame);	
}

void GameSession::update_flight()
{
  Unguided_Manager::setUnguidedManager(&um);
  Unguided_Manager::setTargetList(target_list.targets,target_list.idx);
  Pilot::UpdateStatics();
  fm.update(&um);
  gm.update(&um,target_list);
  doPaletteEffect();
  mouse.Update();
  if (joystick0)
    joystick0->Update();
  if (joystick1)
    joystick1->Update();
  if (select_next)
    {
      fm.select_next_target(fm.view_node);
      select_next = 0;
    }
}

void GameSession::draw_flight(Port_3D *pport)
{
Port_3D vport;
DrawList dlist;
DrawList dlist2;
Pilot    *pilot;
bool pflg;
Flight &fc = fm.get_view_flight();
Flight_ZViewer &vwr = fm.get_view_viewer();

	vga13_begin_scene();

	zz++;
	if (zz == 30)
	{
		clear_zbuff();
		zz = 0;
	}
	else
	ztrans += (3L << Z_PREC);

	if (pport == NULL)
	{
		pport = &vport;
		pflg = 0;
		/* If this plane has crashed, force outside view */
		if (fc.state.crashed)
		{
			fc.controls.view = fv_external;
			fc.controls.vdist = 400;
			fc.controls.vphi = C(0.1);
		}
		else if (!demo_mode)
		{
		  if (vwr.hitme && vwr.isHistory())
				fc.controls.view = fv_flyby;
		}
		fm.set_flight_view(vport);
	}
	else
		pflg = 1;

#ifndef USES_DDRAW
	earth.drawBackDrop(*pport);
#endif

	/*
	Set on/off texture mapping for objects of type "SCLOUD",
	to match that of the terrain
	*/
	clist.setVisibleFlag(display_flags & CLOUDS_ON,"SCLOUD",6);
	clist.add_draw_list(dlist,*pport);
	dlist.drawz(*pport);
	fm.add_draw_list(dlist2,*pport);
	um.add_draw_list(dlist2,*pport);
	gm.add_draw_list(dlist2,*pport);
	dlist2.drawz(*pport);

	// Try drawing the airplane shape so that it
	// surrounds the pilot
	if (!pflg && IS_COCKPIT_VIEW(fc.controls.view)	&& fc.controls.vextern)
	{
		/*
		* The problem is that with the z-buffer, we have to clip at z < 1.0.
		* We need to keep the world_scale around 0.03, to avoid wobbly texture mappings
		* on the larger shapes. So attempting to show the surrounding exterior of the
		* plane would fail, as it would be clipped out.
		* The solution is to scale up the plane and then move the view point. This
		* is accomplished by the code below.
		*/
		REAL_TYPE scl;
		R_3DPoint spc;
		R_3DPoint spcw;
		Vector    v;
		R2D_TYPE  zt;

		Port_3D pp = *pport;
		// Turn off drawing the cockpit shape & shadow
		vwr.cpk_flg = 1;
		vwr.draw_shadow = 0;
		// Scale up the plane to world_scale = 1
		// by inversing the current world_scale
		scl = C(1.0 / world_scale);
		set_co_scaler(scl);
		// Get the original view point in port coords
		spc = vwr.flt->specs->view_point;
		// Times z_manager's scaler gives us world_scale = 1 in port coords
		spc *= vwr.z_manager->scaler;
		// Convert it to world coords
		vwr.flt->state.flight_port.port2world(spc,&spcw);
		// Find the delta vector between it and the origin of the airplane shape
		v = Vector(spcw - vwr.flt->state.flight_port.look_from);
		// Move our view point that distance
		pp.delta_look_from(v);
		// Get the current zbias
		zt = zbias;
		// bias the zbuffer
		zbias = R2D_TYPE(2L << Z_PREC);
		vwr.draw_prep(pp);
		// Undo any distance-abstraction
		vwr.da_level = Z_Viewer::da_full;
		vwr.draw(pp);
		// Restore zbuffer bias
		zbias = zt;
		// Restore scale
		set_co_scaler(1.0);
		vwr.cpk_flg = 0;
		vwr.draw_shadow = 1;
	}

	earth.terrain_on = display_flags & TERRAIN_ON;
	earth.render_ground(*pport);

	if (pflg)
		show_port_vars(*pport,*pport,g_font);
	else
	{
		if (fc.controls.view == fv_front)
		{
			if (fm.view_node == 0 && fc.controls.cockpit && cpk != NULL)
				cpk->draw(static_cast<Flight &>(fc));
			if (fc.controls.hud_on)
				hud.do_hud(fm.get_view_node());
		}
		pilot = (fm.get_view_node()).pilot;
		/* combat track view */
		if ((fc.controls.view == fv_track  ||
			 fc.controls.view == fv_padlock )
				&& pilot->get_target_pilot() != NULL)
		{
			int x,y;
			R_3DPoint w0 = pilot->get_target_pilot()->get_position();
			vport.transform(w0,&x,&y);
			if (!vport.over_flow)
				g_font->put_char(133,x-4,y-4,hud.hud_color,8);
		}
		PilotMessage *message = pilot->get_message();
		if (fc.controls.autopilot && !message->flag)
			show_message(0,"%s:%s",pilot->get_handle(),fc.specs->model);
		if (message->flag)
		{
			if (message->frm != NULL)
				show_message(0,"%s:%s",message->frm->get_handle(),message->mssg);
			else
				show_message(0,message->mssg);
		}
	}
	blit_buff();
	vwr.hitme = 0;

	vga13_end_scene();
}

void GameSession::doPaletteEffect()
{
  if (pe == NULL)
    return;

  if (pe->effect_on)
    pe->is_done(time_frame,1);
  else
    {
      Flight_ZViewer &zv = fm.get_view_viewer();
      if (zv.hitme)
	{
	  pe->do_effect(red_effect,C(0.1));
	  //	  zv.hitme = 0;
	}
      else
	{
	  Flight &f = fm.get_view_flight();
	  if (f.state.crashed)
	    {
	      pe->do_effect(red_effect,2.0);
	    }
	}
    }
}

inline void PRINTLN(std::ostream &os, const char *s, ...)
{
  va_list ap;
  va_start(ap,s);
  char buf[512];
  vsprintf(buf,s,ap);
  va_end(ap);
  os << buf;
}


void GameSession::printResults(std::ostream &os)
{
  int i;
  PRINTLN(os, "\nMission Debrief\n");
  PRINTLN(os, "Flight File: %s\n",flight_file);
  for (i=0;i<fm.n_flights;i++)
    {
      PRINTLN(os,"----------------------------------\n");
      PRINTLN(os,"Flight: %d\n",i);
      printResult(*fm.get_node(i),os);
    }
}


void GameSession::printResult(Flight_Node &fn, std::ostream &os)
{

  Flight *flight = fn.flight;
  Flight_ZViewer *viewer = fn.viewer;

  PRINTLN(os,"Aircraft type: %s, pilot: %s\n",
	  flight->specs->model,
	  fn.handle);
  if (viewer->bagged)
    {
      PRINTLN(os,"Aircraft destroyed by enemy fire\n");
    }
  else if (flight->state.on_ground && fn.lr.landing_attempted)
    {
      /* Rate the landing, and if bad, give reason why */
      if (fn.lr.score >= 0)
	PRINTLN(os,"Safe Landing\n");
      else if (fn.lr.score < 0)
	{
	  if (flight->state.crashed)
	    PRINTLN(os,"Crash Landing\n");
	  else
	    PRINTLN(os,"Bad Landing\n");
	  if (fn.user_node)
	    {
	      if (fn.lr.gear_up)
		PRINTLN(os," * Landing Gear Not Engaged\n");
	      if (fn.lr.l_z)
		{
		  PRINTLN(os," * Vertical Speed: %3.2f ft/s\n",
			  fn.lr.l_z_value);
		  PRINTLN(os,"          Maximum: %3.2f ft/s\n",
			  -flight->specs->l_z);
		}
	      if (fn.lr.v)
		{
		  PRINTLN(os," * Air Speed: %7.2f kts\n",
			  fn.lr.v_value * 3600.0 / 6000.0);
		  PRINTLN(os,"     Maximum: %7.2f kts\n", 
			  flight->specs->lspeed * 3600.0 / 6000.0);
		}
	      if (fn.lr.phi)
		{
		  PRINTLN(os," * Pitch : %3.2f degrees\n",
			  (SABRE_PI2 - fn.lr.phi_value) / SABRE_PI * 180);
		}
	      if (fn.lr.roll)
		{
		  PRINTLN(os," * Roll : %3.2f degrees\n",
			  (fn.lr.roll_value) / SABRE_PI * 180);
		}
	    }
	}
    }
  else
    PRINTLN(os,"Flight Interrupted\n");

  if (viewer->hurt > 0)
    {
      PRINTLN(os,"Damage to Aircraft\n");
      PRINTLN(os,"Total Damage: %d of Maximum %d\n",
	      viewer->hurt,viewer->max_damage);
    }

  if (viewer->ground_kills > 0)
    {
      PRINTLN(os,"%d ground targets destroyed\n",viewer->ground_kills);
    }

  if (viewer->air_kills > 0)
    {
      PRINTLN(os,"%d kill(s) recorded\n",viewer->air_kills);
    }
}

void /*_cdecl*/ GameSession::show_message(int bot, const char *str, ...)
{
  char buf[512];
  va_list ap;

  va_start(ap,str);
  vsprintf(buf,str,ap);
  va_end(ap);

  int poly[10];
  int l = static_cast<int>(strlen(buf)) * 6;
  int y = bot ?  SCREEN_HEIGHT - 8 : 0;
  int yd = bot ? SCREEN_HEIGHT - 1 : 7;
  if (l > 0)
    {
      int x = (SCREEN_WIDTH / 2) - l / 2;
      poly[0] = x - 1;
      poly[1] = y;
      poly[2] = x + l + 1;
      poly[3] = y;
      poly[4] = x + l + 1;
      poly[5] = yd;
      poly[6] = x - 1;
      poly[7] = yd;
      rendply(poly,4,1,&cliprect);
      g_font->font_sprintf(x,y,hud.hud_color,NORMAL,buf);
    }
}

void aiPilot::TextMessageToPlayer(const char *str, ...)
{
char buf[512];
va_list ap;

	if (GameSession::theGame != NULL)
	{
		va_start(ap,str);
		vsprintf(buf,str,ap);
		va_end(ap);
		GameSession::theGame->show_message(0,buf);
	}
}
