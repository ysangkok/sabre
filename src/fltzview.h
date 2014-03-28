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
 * File   : fltzview.h                           *
 * Date   : March, 1997                          *
 * Author : Dan Hammer                           *
 *************************************************/
#ifndef __fltzview_h
#define __fltzview_h

#include "target.h"
#include "zview.h"
#include "flight.h"

enum flight_shapes { fuselage, tail, left_wing, right_wing ,
			     left_hstab, right_hstab, cpit, lgear1,
                             lgear2, lgear3 };


/***************************************************
 * class to record a landing attempt, and whatever *
 * caused it to fail                               *
 ***************************************************/
class Landing_Report                           
{
public:
  int gear_up;
  int l_z;
  float l_z_value;
  int v;
  float v_value;
  int phi;
  float phi_value;
  int roll;
  float roll_value;
  int score;
  int landing_attempted;

  Landing_Report()
    {
      l_z_value = v_value = phi_value = 0.0;
      gear_up = v = phi = roll = l_z = 0;
      score = 0;
      landing_attempted = 0;
    }
  Landing_Report &operator =(Landing_Report &lr)
    {
      score = lr.score;
      gear_up = lr.gear_up;
      l_z = lr.l_z;
      l_z_value = lr.l_z_value;
      v = lr.v;
      v_value = lr.v_value;
      phi = lr.phi;
      phi_value = lr.phi_value;
      roll = lr.roll;
      roll_value = lr.roll_value;
      return *this;
    }

  void all_ok()
    {
      gear_up = l_z = v = phi = roll = 0;
    }
};

/*******************************************************
 * Controls viewing the flight, as well as handling    *
 * landing and damage recording.                       *
 *******************************************************/
class Weapon_Instance_List;
class Flight_ZViewer : public Z_Viewer, public Target
{
public:
  /* Pointer to flight class */
  Flight *flt;
  /* For drawing external weapons */
  Weapon_Instance_List *wil;

  /* Index of shape receiving last hit */
  int hit_shape;
  /* 
     Flags whether to hide the cockpit shape
     (For drawing from pilot's perspective)
     */
  int cpk_flg;
  /* if 1, indicates we were shotdown */
  int bagged;
 
  int ground_hits;
  int ground_kills;
  int air_hits;
  int air_kills;


  Flight_ZViewer(Z_Node_Manager *zv);

  ~Flight_ZViewer();

  void set_flight(Flight *ft);
  Flight *get_flight()
    { return flt; }
  void setWeaponInstanceList(Weapon_Instance_List *wil)
    {
      this->wil = wil;
    }
  Weapon_Instance_List *getWeaponInstanceList()
    {
      return (wil);
    }

  virtual void youHit(Target *tg);

  virtual class_types class_type()
    { return Flight_ZViewer_t; }

  virtual void draw(Port_3D &);
  virtual REAL_TYPE draw_prep(Port_3D &);

  int ouch(const R_3DPoint &, const R_3DPoint &, int, Target * = NULL);
  int ouch(const R_3DPoint &r, int n, Target *tg = NULL)
    { return Target::ouch(r,n,tg); }
  int ouch(const R_3DPoint &w0, float radius, int damage, Target *tg = NULL);


  virtual int getType()
    {
      return (FLIGHT_ZVIEW_T);
    }

  R_3DPoint *get_hit_point();
  R_3DPoint *get_position();
  Vector_Q *get_velocity();
  void calc_damage(int hit_shape, int damage);
  int landing_report(Landing_Report &);
   void set_cpk_view(int flg)
    { cpk_flg = flg; }
   
};

#endif
