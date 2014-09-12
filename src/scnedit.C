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
 * File   : scnedit.C                            *
 * Date   : June, 1997                           *
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
#include "stime.h"
#include "vga_13.h"
#include "scnedit.h"

extern void buffer2ppm();

SceneEdit::SceneEdit(const char *wrld_file,Mouse *,int (*msgHook)(void))
  : clist(&info_manager)
{
  this->world_file = wrld_file;
  this->messageHook = msgHook;
  cliprect.topLeft = Point(0,0);
  cliprect.botRight = Point(SCREEN_WIDTH-1,SCREEN_HEIGHT-1);
  the_earth = &earth;
}

SceneEdit::~SceneEdit()
{

}

void SceneEdit::doEdit()
{
  setup();
  edit();
}

void SceneEdit::readWorldFile()
{
std::ifstream is;
char c;
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
}

void SceneEdit::setup()
{
  zz = 0;
  readWorldFile();
	timer.reset();
	sResetElapsedTime();
}

void SceneEdit::edit()
{
	clear_scr(0);
	do
	{
		if (messageHook && !messageHook())
			break;
		time_frame = timer.get_time();
		raw_time = time_frame;
		sSetTimeFrame(time_frame);	
		if (dump_screen)
		{
			buffer2ppm();
			dump_screen = 0;
		}
		drawScene(port);
	} while(port_key(port)) ;
	clear_scr(0);
}

void SceneEdit::drawScene(Port_3D &prt)
{
DrawList dlist;
  
	vga13_begin_scene();

	zz++;
	if (zz == 30)
	{
		clear_zbuff();
		zz = 0;
	}
	else
		ztrans += (3L << Z_PREC);

#ifdef USES_DDRAW
	earth.terrain_on = display_flags & TERRAIN_ON;
//	earth.draw_horizon(prt);
	earth.render_ground(prt);
#else
	earth.draw_horizon(prt);
	clist.setVisibleFlag(display_flags & CLOUDS_ON,"SCLOUD",6);
	clist.add_draw_list(dlist,prt);
	dlist.draw(prt);
	earth.terrain_on = display_flags & TERRAIN_ON;
	earth.render_ground(prt);
#endif
	show_port_vars(prt,prt,g_font);
	blit_buff();

	vga13_end_scene();
}

void __cdecl SceneEdit::show_message(int bot, char *str, ...)
{
  char buf[512];
  va_list ap;

  va_start(ap,str);
  vsprintf(buf,str,ap);
  va_end(ap);

  int poly[10];
  int l = (int) strlen(buf) * 6;
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
      g_font->font_sprintf(x,y,4,NORMAL,buf);
    }
}








