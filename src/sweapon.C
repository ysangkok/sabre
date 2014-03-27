/*
    Sabre Fighter Plane Simulator 
    Copyright (c) 1997/1998 Dan Hammer
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
 *         Sabre Fighter Plane Simulator         *
 * File   : sweapon.cpp                          *
 * Date   : April, 1998                          *
 * Author : Dan Hammer                           *
 *************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "smath.h"
#include "sweapon.h"

int sWeaponLimits::UpdateBurst(int inFireParams)
{
  if (!inFireParams)
    burstState = BURST_INIT;
  else if (burstState == BURST_INIT)
  {
    burstTimeAccumulator = 0.0;
    burstState = BURST_BURST;
  }
  else if (burstState == BURST_BURST)
  {
    if (burstTimeAccumulator > burstTime)
    {
      burstState = BURST_PAUSE;
      burstTimeAccumulator = 0.0;
    }
  }
  else 
  {
    if (burstTimeAccumulator > burstPauseTime)
    {
      burstState = BURST_BURST;
      burstTimeAccumulator = 0.0;
    }
  }
  return (burstState == BURST_BURST);
}

