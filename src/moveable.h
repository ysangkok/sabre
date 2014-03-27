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
 * moveable.h                                              *
 ***********************************************************/
#ifndef __moveable_h
#define __moveable_h
#ifndef __cpoly_h
#include "cpoly.h"
#endif

#ifndef __flight_h
#include "flight.h"
#include "fltlite.h"
#endif

#ifndef __weapons_h
#include "weapons.h"
#endif

class Moveable
{
public:
  C_3DObject_Base *view;
  int active;
  R_3DPoint position;
  virtual void init();

  virtual void activate(R_3DPoint &p);
  virtual void activate(Flight &launcher);
  virtual void activate(R_3DPoint &p, Vector &);
  virtual void activate(Flight &launcher, Gun_Specs *g_specs, int = 1);
  virtual void activate(Launch_Params &);
  virtual void de_activate();
  virtual void update();
  virtual void start()
    {}
  virtual void pause()
    {}
  virtual void add_draw_list(DrawList &, Port_3D &);
  Moveable()
    {
      active = 0;
      view = NULL;
    }

  virtual ~Moveable()
    {
      if (view != NULL)
	delete view;
    }
};

class TMoveable : public Moveable
{
public:
  float max_time;
  float elapsed_time;
  virtual void init();
  virtual void update();
  virtual void de_activate();
  virtual void pause();
  virtual void start();
  TMoveable(float mtime = 0.0)
    : max_time(mtime),
    elapsed_time(0.0)
    { }
  ~TMoveable()
    {
    }
  void setDuration(float t)
    {
      max_time = t;
    }
};

class Moveables
{
public:
  int n;
  Moveable **moveables;
  int active_count;
  Moveables(int cnt, Moveable **mvs)
    : n(cnt),
    moveables(mvs),
    active_count(0)
    {}
  void update();
  void add_draw_list(DrawList &, Port_3D &);
  void pause();
  void start();
  void de_activate();
  int activate(R_3DPoint &);
  int activate(Flight &);
  int activate(Flight &, Gun_Specs *, int = 1);
  int activate(R_3DPoint &, Vector &);
  int activate(Launch_Params &);
} ;

#endif
