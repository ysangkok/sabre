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
 * File   : obj_3d.C                             *
 * Date   : March, 1997                          *
 * Author : Dan Hammer                           *
 *************************************************/
#include <stdio.h>
#include <values.h>
#include <math.h>
#include <iostream>
#include <string.h>
#include "vmath.h"
#include "target.h"

int calc_radial_damage(const R_3DPoint &r1, float radius, int damage,
		       const bounding_cube &bcube, int &dmg)
{
  R_3DPoint cp;
  float d;
  int result = 0;
  bounding_cube bc = bcube;
  float rad_sq = radius * radius * hit_scaler;
  
  cp = bc.centerPoint();
  d = distance_squared(cp,r1);
  if (d <= rad_sq)
    {
      /* woops! */
      // dmg = (int) (((float)damage) * d / rad_sq);
      dmg = (int) (((float)damage) * (1.0 - (d / rad_sq)));
      result = 1;
    }
  return (result);
}









