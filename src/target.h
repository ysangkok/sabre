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
 * File   : target.h                             *
 * Date   : March, 1997                          *
 * Author : Dan Hammer                           *
 * Base class for anything that can be targeted  *
 *************************************************/
#ifndef __target_h
#define __target_h

#include "vmath.h"
#include "clip.h"
extern float hit_scaler;

#define TARGET_BASE_T   0

class Target
{
public:
  /* Maximum amount of punishment */
  int max_damage;
  /* "Threshold" level of hurting */
  int threshold_damage;
  /* Current punishment level */
  int hurt;
  /* Just got hit */
  int hitme;
  /* Who's side? */
  int affiliation;
  /* Who last hit me / killed me */
  Target *who_got_me;
  /* 
     These last 3 variables will be
     set by Projectile class and 
     FltNode class
     don't touch 'em anywhere
     else, even in subclasses
      */
  /* How many hits I took */
  int i_hits;
  /* How many hits I made */
  int o_hits;
  /* How many shots I made */
  int shots;

  R_3DPoint hitpoint;
  R_3DPoint position;

  Target()
    {
      max_damage = 100;
      threshold_damage = 0;
      hurt = 0;
      hitme = 0;
      who_got_me = NULL;
      affiliation = -1;
      i_hits = o_hits = shots = 0;
    }

  virtual int getType()
    {
      return (TARGET_BASE_T);
    }

  virtual int isHistory()
    {
      return (hurt >= max_damage);
    }

  virtual int ouch(const R_3DPoint &, int , Target * = NULL)
  { return 0; }

  virtual int ouch(const R_3DPoint &, const R_3DPoint &, int, Target * = NULL)
  { return 0; }

  virtual R_3DPoint *get_hit_point()
  { return &hitpoint; }

  virtual R_3DPoint *get_position()
  { return &position; }

  virtual void youHit(Target *)
    {}

  virtual int ouch(const R_3DPoint &, float, int, Target * = NULL)
    { return 0; }
};

#define MAX_TARGETS 512

class Target_List
{
public:
  Target *targets[MAX_TARGETS];
  int idx;

  Target_List()
    {
      idx = 0;
    }

  int add_target(Target *tg)
    {
      if (idx < MAX_TARGETS)
	{
	  targets[idx++] = tg;
	  return(1);
	}
      else
	return(0);
    }
  
  void clear()
    {
      idx = 0;
    }
} ;

enum { C_3DOBJECT_T = TARGET_BASE_T + 1, FLIGHT_ZVIEW_T, GROUND_UNIT_T };

inline int calc_hit(const R_3DPoint &r1, const R_3DPoint &r2, const bounding_cube &bc)
{
  int r;
  bounding_cube b ;
  b = bc;
  b *= hit_scaler;
  r = inbounds(r1,r2,b);
  return r;
}

int calc_radial_damage(const R_3DPoint & r1, float radius, int damage, 
		       const bounding_cube &bc, int &dmg);
#endif


