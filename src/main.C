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
/*************************************************
 *        Sabre Fighter Plane Simulator          *
 * Version: 0.1                                  *
 * File   : main.C                               *
 * Date   : March, 1997                          *
 * Author : Dan Hammer                           *
 * Main file                                     *
 *************************************************/
/* Small fix by Milan Zamazal <pdm@debian.org> in 1999 to disable big security
   hole, see #ifdef DEBIAN.
   1999-12-17: Without SVGAlib compilation fix. */
#define USING_SOUNDCLIENT

#include <stdio.h>
#include <stdlib.h>
#include <values.h>
#include <float.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <ctype.h>
#include <iostream.h>
#include <fstream.h>
#include <setjmp.h>
#include <stdarg.h>
#ifdef DEBIAN
#ifndef DEBIAN_NO_LIBVGA
#include <vga.h>
#endif
#else
#include <vga.h>
#endif

#include "siminc.h"
#include "simsnd.h"
#include "game.h"
#include "scnedit.h"
#include "sbrkeys.h"

#ifndef REV_DATE
#define REV_DATE "01/99"
#endif

/****************************************************************
 * globals                                                      *
 ****************************************************************/

jmp_buf jump_buff;
char mssg[64];
int xx = 0, yy = 0;
int mode = 0;
extern int frame_switch;
int mouse_x,mouse_y;
char *world_file  = "a.wld";
char *ed_world_file = "ed.wld";
char *flight_file = "a.flt";
char *ground_file = NULL;
/*
char *ground_file = "a.gru";
*/
char *hud_file    = "a.hud";
char *cpk_file    = "a.cpk";
char *sound_file =  "a.wvsf";

/*
 *  control flags
 */
int js0 = 0;   // use joystick0 as yoke
int js1 = 0;   // use joystick1 as throttle
int js2 = 0;   // use joystick1 as rudder
int kys  = 0;  // use keyboard as yoke
int mt  = 0;   // use mouse as throttle
int mr = 0;    // use mouse as rudder

int demo = 0;
int edit_scene = 0;
int want_sound = 0;
int mouse_avail = 1;

void (*oldsigfpe)(int);

#ifdef SNDSERV
SoundClient *sndcli_ptr = NULL;
#endif

#define PRINTLN(a) { g_font->font_sprintf(xx,yy,5,NORMAL,(a)); \
  yy += 7; }

inline void println(char *format, ...)
{
  char buff[60];
  va_list ap;
  va_start(ap,format);
  vsprintf(buff,format,ap);
  va_end(ap);
  PRINTLN(buff)
    }

int graphics_start();
void doGame();
void doSceneEdit();
int messageLoop();
void mysigsegv(int seg);
void mysigfpe(int fp);

/*************************************************************
 * MAIN                                                      *
 *************************************************************/
main(int argc, char *argv[])
{
#ifdef DEBIAN
#ifndef DEBIAN_NO_LIBVGA
  vga_disabledriverreport();
  if (vga_init())
    {
      printf("Cannot initialize VGA device.\n");
      return -1;
    }
#endif
#endif
  int i;
  int m;
  printf("Sabre Fighter Plane Simulator Version %s %s\n",
	 VERSION,REV_DATE);
  for (i=1;i<argc;i++)
    {
      if (!strcmp(argv[i],"-o"))
	world_file = argv[++i];
      else if (!strcmp(argv[i],"-j0"))
      	js0 = 1; // joystick 0 for yoke control
      else if (!strcmp(argv[i],"-j1"))
	js1 = 1; // joystick 1 for throttle
      else if (!strcmp(argv[i],"-j2"))
	js2 = 1; // joystick 1 for rudder
      else if (!strcmp(argv[i],"-kys")) // keyboard for yoke
	kys = 1;
      else if (!strcmp(argv[i],"-mt"))  // mouse for throttle
	mt = 1;
      else if (!strcmp(argv[i],"-mr"))  // mouse for rudder
	mr = 1;
      else if (!strcmp(argv[i],"-flt"))
	flight_file = argv[++i];
      else if (!strcmp(argv[i],"-grnd"))
	ground_file = argv[++i];
      else if (!strcmp(argv[i],"-wld"))
	world_file = argv[++i];
      else if (!strcmp(argv[i],"-rnd"))
	do_random = 1;
      else if (!strcmp(argv[i],"-no_crash"))
	no_crash = 1;
      else if (!strcmp(argv[i],"-demo"))
	demo = 1;
      else if (!strcmp(argv[i],"-ed"))
	edit_scene = 1;
      else if (!strcmp(argv[i],"-mode"))
	{
	  m = atoi(argv[++i]);
	  VGAMODE = m;
	}
      /* window_x, window_y */
      else if (!strcmp(argv[i],"-window"))
	{
	  extern int window_width, window_height;
	  window_width = atoi(argv[++i]);
	  window_height = atoi(argv[++i]);
	}
      else if (!strcmp(argv[i],"-rz_steps"))
	{
	  extern int rz_steps;
	  rz_steps = atoi(argv[++i]);
	}
      else if (!strcmp(argv[i],"-sound"))
	want_sound = 1;
      else if (!strcmp(argv[i],"--print-banner"))
	return(0);
    }

#ifdef USING_SOUNDCLIENT
  if (want_sound)
    {
      printf("Initing sound immediately\n");
      sound_init(0L);
      printf("Loading sounds\n");
      read_sound_file(sound_file);
    }
#endif

  mouse_avail = mouse.IsAvailable();
  if (!mouse_avail)
    printf("Mouse not available\n");
  srand(time(NULL));
  
  if (setjmp(jump_buff))
    {
      printf("Exiting\n");
      return(1);
    }

  if (js0)
    {
      printf("Initing joystick 0\n");
      joystick0 = new Joystick(0);
      if (!joystick0->IsAvailable())
	{
	  printf("Error activating joystick 0\n");
	  delete joystick0;
	  joystick0 = NULL;
	}
    }

  if (js1 || js2)
    {
      printf("Initing joystick 1\n");
      joystick1 = new Joystick(1);
      if (!joystick1->IsAvailable())
	{
	  printf("Error activating joystick 1\n");
	  delete joystick1;
	  joystick1 = NULL;
	}
    }

  printf("Initing graphics\n");
  if (graphics_start())
    {
      printf("chaining SIGFPE handler\n");
      oldsigfpe = signal(SIGFPE,mysigfpe);
  
      /*
      printf("chaining SIGSEGV handler\n");
      oldsigsegv = signal(SIGSEGV,mysigsegv);
      */
      if (edit_scene)
	doSceneEdit();
      else
	doGame();
      restorecrtmode();
#ifdef USING_SOUNDCLIENT
      sound_destroy();
#endif
      return 0;
    }
#ifdef USING_SOUNDCLIENT
  sound_destroy();
#endif
  return 1;
}

/*******************************************
 * called by Game object, use to get key   
 * input
 *******************************************/
int messageLoop()
{
  int key;
  int sabreVKey;
  key = kbhit.getch();
  if (key > 0)
    SbrKeyboard::PressKey(key);
  sabreVKey = SbrKeyboard::GetNextSabreVKey();
  if (sabreVKey != FI_NO_KEY)
    FlightInput::SetKeybdin(sabreVKey);
  return 1;
}
 
/*******************************************
 * start up graphics stuff
 *******************************************/
int graphics_start(void)
{
  curpen->color = 2;
  curpen->position = Point(0,0);
  curpen->width = 1;
  printf("Initing graphic interface\n");
  init_vga_13();
  printf("Initing edge bounds\n");
  init_edge_bounds();
  printf("Initing zbuffer\n");
  tr_init();
  printf("Setting port_3D params\n");
  Port_3D::initPort3D();
  printf("Initing font object\n");
  g_font = new ConsoleFont("simfnt-8x8");
  g_font->put_width = 6;
  map_man = new TextrMap_Manager();
  Pilot::initPilot();
  if (mouse_avail)
    mouse.set_position((MXSCREEN_WIDTH - 1) / 2,
		       (MXSCREEN_HEIGHT - 1) / 2);
  return 1;
}

void doGame()
{
  Joystick *yokeJoy = NULL;
  Joystick *throttleJoy = NULL;
  Joystick *rudderJoy = NULL;
  Mouse    *yokeMouse = NULL;
  int      mouseThrottle = 0;
  int      mouseRudder = 0;

  /*
   * determine yoke device
   */
  if (joystick0 && js0)
    {
      yokeJoy = joystick0;
      if (joystick1 && js1)
	throttleJoy = joystick1;
      else if (mt)
	mouseThrottle = 1;
      if (joystick1 && js2)
	rudderJoy = joystick1;
      else if (mr)
	mouseRudder = 1;
    }
  else if (!kys)
    yokeMouse = &mouse;
  else
    {
      /* 
	 using keyboard for yoke, 
	 allow mouse for throttle & rudder
	 */
      if (mt)
	mouseThrottle = 1;
      if (mr)
	mouseRudder = 1;
    }
  if (yokeMouse == NULL)
    mouseThrottle = mouseRudder = 0;
  GameSession theGame(world_file,
		      flight_file,
		      ground_file,
		      yokeMouse,
		      yokeJoy,
		      rudderJoy,
		      throttleJoy,
		      mouseThrottle,
		      mouseRudder,
		      cpk_file,
		      hud_file,
		      demo,
		      messageLoop);
  theGame.doGame();
  theGame.printResults(cout);
}

void doSceneEdit()
{
  SceneEdit theEdit(world_file,
		      &mouse);
  theEdit.doEdit();
}

void error_jump(char *format, ... )
{
  restorecrtmode();
  if (format != NULL)
    {
      va_list ap;
      va_start(ap,format);
      vfprintf(stderr,format,ap);
      va_end(ap);
    }
  printf("\nPress Any Key ...");
  getchar();
  longjmp(jump_buff,1);
}

/*************************************************************
 * SIGSEGV handler                                           *
 ************************************************************/
void (*oldsigsegv)(int);

void mysigsegv(int seg)
{
  printf("sabre: got sigsegv!\n");
  printf("routine_key: %d\n",
	 routine_key);
  printf("calling chained sigsegv handler\n");
  (*oldsigsegv)(seg); 
  restorecrtmode();
  abort();
}

/*************************************************************
 * SIGFPE handler                                            *
 *************************************************************/
void mysigfpe(int fp)
{
  printf("Sabre Fighter Plane Simulator %s %s\n",VERSION,__DATE__);
  printf("Received floating point error\n");
  printf("Routine Key: %d\n",routine_key);
  (*oldsigfpe)(fp);
  restorecrtmode();
  abort();
}
