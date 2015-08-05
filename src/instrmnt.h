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
 * File   : instrmnt.h                           *
 * Date   : March, 1997                          *
 * Author : Dan Hammer                           *
 *************************************************/
#ifndef INSTRUMENT_H
#define INSTRUMENT_H

extern int cockpit_y, cockpit_x;
class TextrMap;
class Indicator
{
public:
  TextrMap *tmap;
  int x,y;
  virtual void underpaint(Flight &) = 0;
  virtual void display(Flight &) = 0;
  virtual void read_from(std::istream &) = 0;
  virtual void init() = 0;
  virtual ~Indicator();
};

class Scaled_Indicator : public Indicator
{
public:
  int width,color;
  float scale;
  float range;
  void read_from(std::istream &is)
  {
    is >> x >> y >> width >> range;
    is >> scale >> color;
    x += cockpit_x;
    y += cockpit_y;
  }
  virtual void init();
};

class ArtHor : public Scaled_Indicator
{
public:
  int height;
  Point s[5];
  Point g[5];
  void underpaint(Flight &);
  void display(Flight &);
  void init();
};

class Aileron_I : public Scaled_Indicator
{
public:
  int cx;
  void display(Flight &);
  void underpaint(Flight &);
  void init();
};

class Elevator_I : public Scaled_Indicator
{
public:
  int cy;
  void display(Flight &);
  void underpaint(Flight &);
  void init();
};

class Rudder_I : public Scaled_Indicator
{
public:
  int cx;
  void display(Flight &);
  void underpaint(Flight &);
  void init();
};

class LED : public Scaled_Indicator
{
public:
  void on();
  void display(Flight &);
  void underpaint(Flight &);
};

class Landing_Gear_LED : public LED
{
public:
  void display(Flight &);
};

class Wheel_Brakes_LED : public LED
{
public:
  void display(Flight &);
};

class Air_Brakes_LED : public LED
{
  void display(Flight &);
};

class Dial_Indicator : public Scaled_Indicator
{
public:
  int cx,cy;
  void display(Flight &);
  void underpaint(Flight &);
  void show_dial(float value);
  void init();
};

class Air_Speed_I : public Dial_Indicator
{
  void display(Flight &);
};

class Altitude_I : public Dial_Indicator
{
  void display(Flight &);
};

class Compass : public Dial_Indicator
{
  void display(Flight &);
};

class Throttle_I : public Dial_Indicator
{
  void display(Flight &);
  void init();
};

class Instrument_Panel
{
public:
  Indicator **indicators;
  int max_indicators;
  int indicators_idx;
  Instrument_Panel(int max_i);
  ~Instrument_Panel();
  void add_indicator(Indicator *);
  friend std::istream &operator >>(std::istream &is, Instrument_Panel &);
  void underpaint(Flight &);
  void display(Flight &);
  void init();
};

void setup_instruments(const char *, Instrument_Panel &);
extern Instrument_Panel instrument_panel;

#endif
