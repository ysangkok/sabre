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
 * File   : aipilot2.cpp                         *
 * Date   : April, 1998                          *
 * Author : Dan Hammer                           *
 *************************************************/
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include "aipilot.h"
#include "aipildef.h"

/*************************************************************
 * Maneuver update routines                                  *
 *************************************************************/

/*************************************************************
 * Surface target attack                                     *
 *************************************************************/

void aiPilot::DoLevelBomb(sManeuverState &)
{
  sREAL     timeToTarget;
  sREAL     timeOfFall;
  sVector   v;
  sAttitude att;

  GetSurfaceTargetGeometry(surfaceTarget);
  timeOfFall = sSQRT(flightModel->GetHeightAGL() / C(16.0));
  timeToTarget = SURFACE_TARGET_RANGE / flightModel->GetAirSpeedFPS();
  SetNoseOn(SURFACE_TARGET_PITCH,SURFACE_TARGET_YAW, Pi_4 * C(0.4));
  SETCONTROLX(speedCtl, sMPH2FPS(navInfo.waypoint.speed), C(0.01));
  GetAttitude(surfaceTarget.worldPoint,v,att);
  if ((fabs(att.yaw) < (Pi - Pi_2)) && timeOfFall >= timeToTarget)
    {
      isBombing = 1;
      DropBomb();
    }
  else
    isBombing = 0;
}

/*****************************************************************
 * Evade, don't fight a pursuer ... for training/target practice *
 *****************************************************************/
void aiPilot::DoEvade(sManeuverState &mv)
{
  enum { INIT, INITX, BASE, JINK, REAR, CLIMB, TARGET_DEAD, AVOID_GROUND, TURN, EXTEND };

  curTarget = &evadeTarget;
  GetTargetFlags(evadeTarget);
  if (TARGET_ACTIVE)
    GetTargetGeometry(TARGET_IDX,TARGET_GEOMETRY);

  engageMaxSpeed = flightModel->GetMaxSpeedFPS() * engageMaxSpeedMul;
	
  /*
   * If target is "dead", go straight and level and wait for further
   * instructions
   */
  if (mv.state != AVOID_GROUND && !TARGET_ACTIVE && mv.state != TARGET_DEAD)
    {
      ClearManeuverStackTo(mv.stackLevel);
      PushManeuver(sManeuver::STRAIGHT_AND_LEVEL,IMNVR_LOOPBIT);
      mv.state = TARGET_DEAD;
    }

  /* check ground collision */
  if (mv.state != AVOID_GROUND)
    {
      sREAL secs,agl;
      sREAL rcvrAGL;
      secs = (grndColSecs);
      agl  = (grndColAGL);
      rcvrAGL = (grndColRcvrAGL);
      if (flightModel->GetHeightAGL() + deltaAltitude * secs <= agl)
	{
	  mv.data3 = static_cast<sREAL>(mv.state);
	  mv.state = AVOID_GROUND;
	  ClearManeuverStackTo(mv.stackLevel);
	  PushManeuver(sManeuver::PULLUP,0,
		       rcvrAGL,
		       (grndColPullUpAngle));
	}
    }


  /*
    if (mv.state != INITX &&
    mv.state != JINK &&
    TARGET_FLAGS.rear && 
    TARGET_FLAGS.headOnAspect &&
    TARGET_RANGE <= REAR_THREAT_DISTANCE)
    {
    ClearManeuverStackTo(mv.stackLevel);
    PushManeuver(sManeuver::RQ_EVADE);
    mv.state = JINK;
    }
    */

  switch (mv.state)
    {

    case INIT:
      {
	unsigned int turn_bits;
	if (TARGET_YAW < 0.0)
	  turn_bits = IMNVR_LEFT;
	else
	  turn_bits = IMNVR_RIGHT;
	mv.data3 = 5.0;
	mv.data4 = 0.0;
	PushManeuver(sManeuver::BREAK_TURN,turn_bits);
	mv.state = INITX;
      }
    break;

    case INITX:
      mv.data4 += timeFrame;
      if (mv.data4 >= mv.data3)
	ClearManeuverStackTo(mv.stackLevel);
      if (mnvrStackPtr == mv.stackLevel)
	mv.state = BASE;
      break;

    case BASE:
      {
	flightModel->SetWEP(1);
	flightModel->SetEngineControlPer(1.0);
	if (TARGET_FLAGS.rear)
	  {
	    if (TARGET_RANGE <= REAR_THREAT_DISTANCE)
	      {
		PushManeuver(sManeuver::RQ_EVADE);
		mv.state = JINK;
	      }
	    PushManeuver(sManeuver::RQ_ENGAGE);
	    mv.state = REAR;
	  }
	else if (TARGET_FLAGS.forward)
	  {
	    unsigned int turn_bits;
	    if (TARGET_YAW < 0.0)
	      turn_bits = IMNVR_LEFT;
	    else
	      turn_bits = IMNVR_RIGHT;
	    PushManeuver(sManeuver::HARD_TURN,turn_bits);
	    mv.state = TURN;
	  }
	else
	  {
	    PushManeuver(sManeuver::EXTEND,0,engageMaxSpeed,20.0,extAngle);
	    mv.state = EXTEND;
	  }
      }
    break;


    case JINK:
      if (mnvrStackPtr == mv.stackLevel)
	mv.state = BASE;
      break;

    case REAR:
      if (mnvrStackPtr == mv.stackLevel)
	mv.state = BASE;
      break;

    case TARGET_DEAD:
      if (mnvrStackPtr == mv.stackLevel)
	mv.state = BASE;
      break;

    case AVOID_GROUND:
      if (mnvrStackPtr == mv.stackLevel)
	mv.state = BASE;
      break;

    case EXTEND:
      if (mnvrStackPtr == mv.stackLevel)
	mv.state = BASE;
      break;
    }
}






