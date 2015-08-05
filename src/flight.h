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
/*************************************************************
 * flight.h                                                  *
 *************************************************************/
#ifndef FLIGHT_H
#define FLIGHT_H

#include "dvmath.h"
#include "fltobj.h"

class Flight
{
public:
  float l_time;
  Flight_Controls controls;
  Flight_State state;
  Flight_Forces forces;
  Flight_Mods mods;
  Flight_Specs *specs;

  Flight(Flight_Specs *spcs)
    : specs(spcs)
  {
    state.landing_z = 0.0;
    l_time = static_cast<REAL_TYPE>(0.1);
  }
  virtual ~Flight();
  void start();
  void update();
  void pause();
  void calcState(float);
  void calcForces(float);
  void calcRotations(float);
  void applyForces(float);
  void applyRotations(float);
  void read(std::istream &);
  void write(std::ostream &);
  friend std::istream & operator >>(std::istream & is, Flight &f)
    {
      f.read(is);
      return(is);
    }
  friend std::ostream & operator <<(std::ostream & os, Flight &f)
    {
      f.write(os);
      return(os);
    }
  inline DVector to_port(DVector &);
  inline DVector to_world(DVector &);
  static REAL_TYPE rot_acc_max;
  static REAL_TYPE rot_rate_max;
  static REAL_TYPE rot_max;
};


inline DVector Flight::to_port(DVector &v)
{
  R_3DPoint r_w,r_p;
  r_w = v.to_R_3DPoint();
  r_w += state.flight_port.look_from;
  state.flight_port.world2port(r_w,&r_p);
  return (DVector(r_p));
}

inline DVector Flight::to_world(DVector &v)
{
  R_3DPoint r_w,r_p;
  r_p = v.to_R_3DPoint();
  state.flight_port.port2world(r_p,&r_w);
  r_w -= state.flight_port.look_from;
  return (DVector(r_w));
}

inline float DAMP(float value, float d)
{
  if (value == 0.0)
    return value;
  if (value >= 0.0)
    {
      value -= d;
      if (value < 0.0)
	value = 0.0;
    }
  else
    {
      value += d;
      if (value > 0.0)
			value = 0.0;
    }
  return value;
}

inline void LIMIT(float &value, float limit)
{
  if (fabs(value) > limit)
    {
      if (value < 0.0)
	value = -limit;
      else
	value = limit;
    }
}

inline void LIMITX(float &value, float limit)
{
  if (fabs(value) > limit)
    value = limit;
}

#endif
