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
/***********************************************************
 * moveable.cpp                                            *
 ***********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>
#include <fstream.h>
#include <math.h>
#include <limits.h>
#include <values.h>
#include "defs.h"
#include "pc_keys.h"
#ifdef USING_FIXED
#include "fixpoint.h"
#include "trigtab.h"
#endif
#include "vga_13.h"
#include "vmath.h"
#include "port_3d.h"
#include "cpoly.h"
#include "copoly.h"
#include "flight.h"
#include "font8x8.h"
#include "colorspc.h"
#include "viewobj.h"
#include "rtkey.h"
#include "fltlite.h"
#include "sim.h"
#include "weapons.h"
#include "moveable.h"

void Moveable::update()
{

}

void Moveable::init()
{

}

void Moveable::activate(R_3DPoint &p)
{
  active = 1;
  position = p;
  init();
  start();
}

void Moveable::activate(Flight &launcher)
{
  activate(launcher.state.flight_port.look_from);
}

void Moveable::activate(R_3DPoint &p, Vector &)
{
  activate(p);
}

void Moveable::activate(Flight &launcher, Gun_Specs *, int )
{
  activate(launcher);
}

void Moveable::activate(Launch_Params &lp)
{
  activate(lp.departure_point);
}

void Moveable::de_activate()
{
  active = 0;
}

void Moveable::add_draw_list(DrawList & dl, Port_3D & port)
{
  if (view != NULL)
    {
      view->draw_prep(port);
      if (view->visible_flag)
	dl.add_object(view);
    }
}

void TMoveable::update()
{
  if (!active)
    return;
  elapsed_time += time_frame;
  if (elapsed_time >= max_time)
    de_activate();
}

void TMoveable::init()
{
  Moveable::init();
  elapsed_time = 0.0;
}

void TMoveable::de_activate()
{
  pause();
  Moveable::de_activate();
}

void TMoveable::start()
{

}

void TMoveable::pause()
{

}

void Moveables::update()
{
  R_KEY_BEGIN(13)
    if (active_count)
      {
	active_count = 0;
	for (int i=0;i<n;i++)
	  {
	    moveables[i]->update();
	    if (moveables[i]->active)
	      active_count++;
	  }
      }
  R_KEY_END
    }

void Moveables::add_draw_list(DrawList &dl, Port_3D &port)
{
  if (active_count)
    for (int i=0;i<n;i++)
      if (moveables[i]->active)
	moveables[i]->add_draw_list(dl,port);
}

void Moveables::pause()
{
  for (int i=0;i<n;i++)
    if (moveables[i]->active)
      moveables[i]->pause();
}

void Moveables::start()
{
  for (int i=0;i<n;i++)
    if (moveables[i]->active)
      moveables[i]->start();

}

int Moveables::activate(R_3DPoint &p)
{
  if (active_count >= n)
    return 0;
  int i;
  for (i=0;i < n;i++)
    {
      if (!moveables[i]->active)
	{
	  moveables[i]->activate(p);
	  if (moveables[i]->active)
	    active_count++;
	  break;
	}
    }
  return i+1;
}

int Moveables::activate(Flight &flt)
{
  if (active_count >= n)
    return 0;
  int i;
  for (i=0;i < n;i++)
    {
      if (!moveables[i]->active)
	{
	  moveables[i]->activate(flt);
	  if (moveables[i]->active)
	    active_count++;
	  break;
	}
    }
  return i+1;
}

int Moveables::activate(Launch_Params &lp)
{
  if (active_count >= n)
    return 0;
  int i;
  for (i=0;i < n;i++)
    {
      if (!moveables[i]->active)
	{
	  moveables[i]->activate(lp);
	  if (moveables[i]->active)
	    active_count++;
	  break;
	}
    }
  return i+1;

}

void Moveables::de_activate()
{
  for (int i=0;i<n;i++)
    moveables[i]->de_activate();
  active_count = 0;
}

int Moveables::activate(Flight &flt, Gun_Specs *g_specs, int flag)
{
  if (active_count >= n)
    return 0;
  int i;
  for (i=0;i < n;i++)
    {
      if (!moveables[i]->active)
	{
	  moveables[i]->activate(flt,g_specs,flag);
	  if (moveables[i]->active)
	    active_count++;
	  break;
	}
    }
  return i+1;
}

int Moveables::activate(R_3DPoint &p, Vector &v)
{
  if (active_count >= n)
    return 0;
  int i;
  for (i=0;i < n;i++)
    {
      if (!moveables[i]->active)
	{
	  moveables[i]->activate(p,v);
	  if (moveables[i]->active)
	    active_count++;
	  break;
	}
    }
  return i+1;
}
