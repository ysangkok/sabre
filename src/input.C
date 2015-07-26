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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include "defs.h"
#include "vga_13.h"
#include "input.h"
#ifdef HAVE_LIBSDL
#include <SDL2/SDL.h>
#else
#include <directfb.h>
#include "../gdev/gdev.h"
#endif

InputDevice_2D::~InputDevice_2D() = default;
InputDevice::~InputDevice() = default;
Joystick::~Joystick() = default;

extern int adjusted_mouse_x, adjusted_mouse_y, adjusted_buttons;

/*
 * This resets the mouse driver.  The value returned is "true"
 * if the mouse driver is present and the mouse could be reset.
 */
int Mouse::ResetDriver( void ) const
{
#ifdef HAVE_LIBSDL
return true;
#else
  return (1);
#endif
}

#ifdef HAVE_LIBSDL
void Mouse::set_position(int, int)
{
}
#else
void Mouse::set_position(int, int)
{
  //mouse_setposition(x,y);
}
#endif

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
     int SDL_x,SDL_y;
     SDL_PollEvent(NULL);
     buttons = static_cast<int>(SDL_GetMouseState(&SDL_x, &SDL_y));
     screenx = SDL_x;
     screeny = SDL_y;
     scx = static_cast<float>(screenx);
     scy = static_cast<float>(screeny);
     x = scx / (MXSCREEN_WIDTH - 1);
     x -= 0.5;
     x *= 2.0;
     y = scy / (MXSCREEN_HEIGHT - 1);
     y -= 0.5;
     y *= 2.0;

#else
  screenx = adjusted_mouse_x;
  screeny = adjusted_mouse_y;
  buttons = adjusted_buttons;
  x = (float) adjusted_mouse_x / (MXSCREEN_WIDTH - 1);
  x -= 0.5;
  x *= 2.0;
  y = (float) adjusted_mouse_y / (MXSCREEN_HEIGHT - 1);
  y -= 0.5;
  y *= 2.0;
#endif
}

Mouse::~Mouse()
{
}
