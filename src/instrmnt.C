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
 * File   : instrmnt.C                           *
 * Date   : March, 1997                          *
 * Author : Dan Hammer                           *
 *************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <values.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <iostream>
#include <fstream>
#include "defs.h"
#include "grafix.h"
#include "vga_13.h"
#include "traveler.h"
#include "pen.h"
#include "convpoly.h"
#ifdef USING_FIXED
#include "fixpoint.h"
#include "trigtab.h"
#else
#define ConvertToFix(a) (a)
#define fix2f(a) (a)
#define f2fix(a) (a)
#define fix2s(a) (static_cast<int>(a))
#endif
#include "vmath.h"
#include "txtrmap.h"
#include "port_3d.h"
#include "cpoly.h"
#include "font8x8.h"
#include "flight.h"
#include "rotate.h"
#include "colorspc.h"
#include "simfile.h"
#include "bits.h"
#include "instrmnt.h"

extern int frame_switch;

static ArtHor arthor;
static Aileron_I aileron_i;
static Elevator_I elevator_i;
static Rudder_I rudder_i;
static Landing_Gear_LED landing_gear_led;
static Wheel_Brakes_LED wheel_brakes_led;
static Air_Brakes_LED air_brakes_led;
static Air_Speed_I air_speed_i;
static Altitude_I altitude_i;
static Compass compass;
static Throttle_I throttle_i;

Instrument_Panel instrument_panel(13);

static unsigned char *b_ptr;

void ArtHor::init()
{
  int h_2;
  height = static_cast<int>(range);
  h_2 = height / 2;
  s[0] = Point(x,y);
  s[1] = Point(x+width,y);
  s[2] = Point(x+width,y+h_2);
  s[3] = Point(x,y+h_2);
  s[4] = s[0];
  g[0] = Point(x,y);
  g[1] = Point(x+width,y);
  g[2] = Point(x+width,y+height);
  g[3] = Point(x,y+height);
  g[4] = g[0];
}

void ArtHor::underpaint(Flight &flight)
{
  Point s1[5];
  Point g1[5];
  int sc_y;
  REAL_TYPE ang,sin_ang,cos_ang;
  Port_3D &port = flight.state.flight_port;
  Vector v1 = Vector(port.look_at - port.look_from);
  Vector v2 = v1;
  v2.Z = 0;
  REAL_TYPE dot = v1.Dot(v2);
  dot = 1 - dot;
  if (port.look_from.z > port.look_at.z)
    dot = -dot;
  dot *= f2fix(scale);
  sc_y = static_cast<int>(y + (height / 2) + dot);
  REAL_TYPE cx = x + (width / 2);
  REAL_TYPE cy = y + (height / 2);
  if (sc_y < y + 3)
    sc_y = y + 3;
  else if (sc_y > y + height - 3)
    sc_y = y + height - 3;
  s[2].y = sc_y;
  s[3].y = sc_y;
  if (static_cast<bool>(port.roll))
    ang = SABRE_2PI - port.roll;
  else
    ang = 0;
  if (flight.state.negative_phi)
    ang = limit_angle(ang + SABRE_PI);
  sin_ang = sSIN(ang);
  cos_ang = sCOS(ang);
  for (int i=0;i<5;i++)
    {
      s1[i] = point_rotate(s[i],cx,cy,sin_ang,cos_ang);
      g1[i] = point_rotate(g[i],cx,cy,sin_ang,cos_ang);
    }
  fill_convpoly(reinterpret_cast<int *>(g1),5,color+1);
  fill_convpoly(reinterpret_cast<int *>(s1),5,color);
}

void ArtHor::display(Flight &)
{
  /*
    int sc_y, sc_x;

    Port_3D &port = flight.state.flight_port;

    Vector v1 = Vector(port.look_at - port.look_from);
    Vector v2 = v1;
    v2.Z = 0;
    REAL_TYPE dot = v1.Dot(v2);
    dot = 1 - dot;
    if (port.look_from.z > port.look_at.z)
    dot = -dot;
    dot *= f2fix(scale);
    sc_y = y + (height / 2) + fix2s(dot);
    if (sc_y < y)
    sc_y = y;
    else if (sc_y > y + height)
    sc_y = y + height;
    h_line(x,sc_y,width,15);
    */
}
void Aileron_I::init()
{
  cx = x + (static_cast<int>(range) / 2);
}
void Aileron_I::display(Flight &flt)
{
  REAL_TYPE fr = f2fix(range);
  REAL_TYPE fa = REAL_TYPE(flt.controls.ailerons);
  REAL_TYPE fx = (fr * fa) / 61 ;
  int i = fix2s(fx);
  i++;
  v_line(cx - i,y,width,color);
}

void Elevator_I::init()
{
  cy = y + (static_cast<int>(range) / 2);
}

void Elevator_I::display(Flight &flt)
{
  REAL_TYPE fr = f2fix(range);
  REAL_TYPE fa = REAL_TYPE(flt.controls.elevators + flt.controls.trim);
  // 	REAL_TYPE fx = (fr * fa) / 31 ;
  REAL_TYPE fx = (fr * fa) / 61;
  int i = fix2s(fx);
  //	i;
  h_line(x,cy + i,width,color);
}

void Rudder_I::init()
{
  cx =  x + (static_cast<int>(range) / 2);
}

void Rudder_I::display(Flight &flt)
{
  REAL_TYPE fr = f2fix(range);
  REAL_TYPE fa = REAL_TYPE(flt.controls.rudder);
  //	REAL_TYPE fx = (fr * fa) / 31 ;
  REAL_TYPE fx = (fr * fa) / 61;
  int i = fix2s(fx);
  i++;
  v_line(cx - i,y,width,color);
}

void LED::on()
{
  extern unsigned char  led[];
  extern int led_X_size,led_Y_size;
	trans_blit(x,y,x+led_X_size-1,y+led_Y_size-1,led,b_ptr);
}

void Landing_Gear_LED::display(Flight &flt)
{
  if (flt.controls.landing_gear == 1)
    on();
}

void Wheel_Brakes_LED::display(Flight &flt)
{
  if (flt.controls.wheel_brakes == 1)
    on();
}

void Air_Brakes_LED::display(Flight &flt)
{
  if (flt.controls.speed_brakes == 1)
    on();
}

void Dial_Indicator::init()
{
  cx = x + width / 2;
  cy = y + width / 2;
}

void Dial_Indicator::show_dial(float value)
{
  float angle,fx,fy;
  int d_x,d_y;
  angle = (6.2831853f * value) / range;
  angle -= 1.5707963;
  fx = C(static_cast<double>(width) * cos(angle));
  fy = C(static_cast<double>(width) * sin(angle));
  d_x = static_cast<int>(fx);
  d_y = static_cast<int>(fy);
  b_linedraw(x,y,x+d_x,y+d_y,color,NULL);
}

void Air_Speed_I::display(Flight &flt)
{
  show_dial((flt.state.z_vel * 3600.0f ) / 6000.0f);
}

void Altitude_I::display(Flight &flt)
{
  color++;
  range = 100000;
  show_dial(
	    fix2f(flt.state.flight_port.look_from.z)
	    / world_scale
	    );
  range = 1000;
  color--;
  show_dial(
	    fix2f(flt.state.flight_port.look_from.z)
	    / world_scale
	    );
}

void Compass::display(Flight &flt)
{
  float angle,fx,fy;
  int d_x,d_y;
  angle = flt.state.heading - 1.5707963f;
  fx = C(static_cast<double>(width) * cos(angle));
  fy = C(static_cast<double>(width) * sin(angle));
  d_x = static_cast<int>(fx);
  d_y = static_cast<int>(fy);
  b_linedraw(x,y,x+d_x,y+d_y,color,NULL);
}

void Throttle_I::init()
{
  tmap = map_man->get_map_ptr("throttle");
  if (tmap == NULL)
    printf("missing throttle txr!\n");
}

void Throttle_I::display(Flight &flt)
{
  if (tmap != NULL)
    {
      REAL_TYPE rf = ConvertToFix(range);
      REAL_TYPE tf = ConvertToFix(flt.controls.throttle);
      REAL_TYPE yf = (tf * rf) / 100;
      int dy = y - fix2s(yf);
      trans_blit(x,dy,x+tmap->map_w-1,
		 dy+tmap->map_h-1,tmap->getBytes(),b_ptr);
    }
}

Instrument_Panel::Instrument_Panel(int max_i)
{
  max_indicators = max_i;
  indicators = new Indicator *[max_indicators];
  indicators_idx = 0;
}

void Instrument_Panel::add_indicator(Indicator *ind)
{
  if (indicators_idx < max_indicators)
    indicators[indicators_idx++] = ind;
}

std::istream &operator >>(std::istream &is, Instrument_Panel &ip)
{
  char c;
  int i;

  is >> c;
  while (is && c!='{')
    is >> c;
  for (i = 0;i < ip.indicators_idx; i++)
    {
      while (is && c != '(')
	is >> c;
      if (is)
	{
	  ip.indicators[i]->read_from(is);
	  is >> c;
	}
      while (is && c != ')')
	is >> c;
    }
  /*
  is >> n;
  for (i=0;i<n;i++)
    is >> cs;
  is >> c;
  while (is && c != '}')
    is >> c;
    */
  return is;
}

void Instrument_Panel::underpaint(Flight &flt)
{
  b_ptr = lock_xbuff();
  if (!b_ptr)
	 return;
  int fs = frame_switch;
  frame_switch = 0;
  for (int i = 0; i < indicators_idx; i++)
    indicators[i]->underpaint(flt);
  frame_switch = fs;
  free_xbuff();
}

void Instrument_Panel::display(Flight &flt)
{
  b_ptr = lock_xbuff();
  if (!b_ptr)
	return;
  int fs = frame_switch;
  frame_switch = 0;
  for (int i = 0; i < indicators_idx; i++)
    indicators[i]->display(flt);
  frame_switch = fs;
  free_xbuff();
}

void Instrument_Panel::init()
{
  for (int i = 0; i < indicators_idx; i++)
    indicators[i]->init();
}

void setup_instruments(const char *path, Instrument_Panel &ip)
{
  ip.indicators_idx = 0;
  ip.add_indicator(&arthor);
  ip.add_indicator(&aileron_i);
  ip.add_indicator(&elevator_i);
  ip.add_indicator(&rudder_i);
  ip.add_indicator(&landing_gear_led);
  ip.add_indicator(&wheel_brakes_led);
  ip.add_indicator(&air_brakes_led);
  ip.add_indicator(&air_speed_i);
  ip.add_indicator(&altitude_i);
  ip.add_indicator(&compass);
  ip.add_indicator(&throttle_i);

  std::ifstream is;
  if (open_is(is,path))
    is >> ip;
  is.close();
  ip.init();
}
Indicator::~Indicator() = default;
void Scaled_Indicator::init() {}
void Elevator_I::underpaint(Flight&) {}
void Dial_Indicator::underpaint(Flight&) {}
void Dial_Indicator::display(Flight&) {}
void Aileron_I::underpaint(Flight&) {}
void LED::underpaint(Flight&) {}
void LED::display(Flight&) {}
void Rudder_I::underpaint(Flight&) {}
Instrument_Panel::~Instrument_Panel() { if (indicators) delete indicators; }
