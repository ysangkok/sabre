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
 *           Sabre Fighter Plane Simulator              *
 * Version: 0.1                                  *
 * File   : input.C  Linux version               *
 * Date   : March, 1997                          *
 * Author : Dan Hammer                           *
 * Device input (mouse & joystick)               *
 * 12/98 David Vandewalle : SDL Port             *
 *************************************************/
#define HAVE_LIBSDL

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <iostream.h>
#include <fstream.h>
#include "defs.h"
#include "vga_13.h"
#include "input.h"
#ifdef HAVE_LIBSDL
#include <SDL/SDL.h>
#else
#ifndef SABREWIN
#include "vga.h"
#include "vgagl.h"
#include "vgamouse.h"
#endif
#endif

const int mouse_manual_flag = 0;

/*
 * This resets the mouse driver.  The value returned is "true"
 * if the mouse driver is present and the mouse could be reset.
 */
int Mouse::ResetDriver( void ) const
{
#ifdef HAVE_LIBSDL
// init mouse events for SDL ...
return true;
#else
#ifndef SABREWIN
  if (mouse_manual_flag)
    {
      if (mouse_init("/dev/mouse",MOUSE_MICROSOFT,
		     MOUSE_DEFAULTSAMPLERATE) > 0)
	{
	  mouse_setxrange(0,SCREEN_WIDTH-1);
	  mouse_setyrange(0,SCREEN_HEIGHT-1);
	  mouse_setwrap(MOUSE_NOWRAP);
	}
      else
	return(0);
    }
  else
    vga_setmousesupport(1);
#endif
  return (1);
#endif
}

void Mouse::set_position(int x, int y)
{
#ifdef HAVE_LIBSDL

#else
#ifndef SABREWIN
  mouse_setposition(x,y);
#endif
#endif
}

/*
 * void Mouse::Update( void )
 *
 * Updates the mouse's internal state information.
 *
 */
void Mouse::Update( void )
{
#ifdef HAVE_LIBSDL
     float scx,scy;
     unsigned short SDL_x,SDL_y;
     SDL_PollEvent(NULL);
     buttons = SDL_GetMouseState(&SDL_x, &SDL_y);    
     screenx = SDL_x;
     screeny = SDL_y;
     scx = (float) screenx;
     scy = (float) screeny;
     x = scx / (MXSCREEN_WIDTH - 1);
     x -= 0.5;
     x *= 2.0;
     y = scy / (MXSCREEN_HEIGHT - 1);
     y -= 0.5;
     y *= 2.0;

#else
#ifndef SABREWIN
  mouse_update();
  float scx,scy;
  buttons = mouse_getbutton();
  screenx = mouse_getx();
  screeny = mouse_gety();
  scx = (float) screenx;
  scy = (float) screeny;
  x = scx / (MXSCREEN_WIDTH - 1);
  x -= 0.5;
  x *= 2.0;
  y = scy / (MXSCREEN_HEIGHT - 1);
  y -= 0.5;
  y *= 2.0;
#endif
#endif
}

Mouse::~Mouse()
{
#ifdef HAVE_LIBSDL

#else
#ifndef SABREWIN
  if (mouse_manual_flag)
    mouse_close();
#endif
#endif
}
