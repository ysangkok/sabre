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
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "simmath.h"

int SimMath::sectLinePlane(R_3DPoint &intersect_point, 
			   const R_3DPoint &l0, const R_3DPoint &l1,
			   const Vector &normal, float plane_constant)
{
  float lambda,n0;
  int result = NORMSECT;
  Vector v0(l1,l0);
  n0 = dot(normal,v0);
  if (n0 == 0.0)
    {
      float n1 = dot(normal,Vector(l0)) - plane_constant;
      if (n1 == 0)
	result = PARALLEL_NONE;
      else
	result = PARALLEL_ALL;
    }
  else
    {
      lambda = (plane_constant - dot(normal,Vector(l0))) / n0;
      intersect_point.x = l0.x + lambda * v0.X;
      intersect_point.y = l0.y + lambda * v0.Y;
      intersect_point.z = l0.z + lambda * v0.Z;
    }
  return (result);
}

