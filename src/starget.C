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
 * File   : starget.C                            *
 * Date   : April, 1998                          *
 * Author : Dan Hammer                           *
 *************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "starget.h"

sTargetGeometry::sTargetGeometry()
{
  angleOff = aspect = 
    range = dRange = 0.0;
  for (int i=0;i<T_AVERAGING_N;i++)
    ranges[i] = 0.0;
  rng_idx = 0;
  gunTOF = 1.0;
}

sTargetInfo::sTargetInfo()
{
  strcpy(model,"DEFAULT");
  tacticalCaps[sPilotTactics::ANGLES_FIGHT] = 1;
  tacticalCaps[sPilotTactics::ENERGY_FIGHT] = 0;
  tacticalCaps[sPilotTactics::DIVE_BOMB] = 0;
  tacticalCaps[sPilotTactics::TORPEDO_BOMB] = 0;
  tacticalCaps[sPilotTactics::LEVEL_BOMB] = 0;
  maxSpeed = 325.0;
  weaponsRange = 2000.0;
  weaponsEffect = 4.0;
}

/*********************************************
 * Target flags
 *********************************************/
/* 
 * The flags are in pairs.
 * A flag pair (for example, rangeClose & rangeOpen)
 * can have 3 states:
 * 1 0
 * 0 1
 * 0 0
 * allowing for the concept of "getting closer",
 * "getting further away" or "staying same distance"
 * To determine a pair value for a given target,
 * n pairs are averaged.
 */
/* Add to flags averaging counter */
void sTargetFlags::Add(const sTargetFlags &fl)
{
  rangeClose += fl.rangeClose;
  rangeOpen += fl.rangeOpen;
  turning += fl.turning;
  turning_not += fl.turning_not;
  below += fl.below;
  above += fl.above;
  high += fl.high;
  low += fl.low;
  veryHigh += fl.veryHigh;
  veryLow += fl.veryLow;
  headOnAspect += fl.headOnAspect;
  tailAspect += fl.tailAspect;
  onNose += fl.onNose;
  onTail += fl.onTail;
  forward += fl.forward;
  rear += fl.rear;
  highAOT += fl.highAOT;
  lowAOT += fl.lowAOT;
}

/*
 * Set on/off flags based on
 * differential
 */
void SetFunct(int &x, int &y, int t = 3)
{
  int dd = 0;

  dd = x - y;
  if (dd >= t)
    {
      x = 1;
      y = 0;
    }
  else if (dd <= -t)
    {
      y = 1;
      x = 0;
    }
  else
    {
      x = y = 0;
    }
}

/*
 * Determine state of all flag pairs
 */
void sTargetFlags::Set(int max)
{
  SetFunct(rangeClose,rangeOpen,max);
  SetFunct(turning,turning_not,max);
  SetFunct(above,below,max);
  SetFunct(high,low,max);
  SetFunct(veryHigh,veryLow,max);
  SetFunct(headOnAspect,tailAspect,max);
  SetFunct(onNose,onTail,max);
  SetFunct(forward,rear,max);
  SetFunct(lowAOT,highAOT);
}

void sTarget::GetTargetInfo(unsigned long targetIdx, sPoint &position, 
			    sVector &velocity, sVector &heading)
{
}

int sTarget::GetTargetActive(unsigned long targetIdx)
{
  return 0;
}
