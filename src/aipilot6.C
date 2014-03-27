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
 * TakeOff                                                   *
 *************************************************************/
void aiPilot::DoTakeOff(sManeuverState &mv)
{
  sVector		v;
	
  EnableGroundCollision(0);
  switch (mv.state)
    {
    case 0:
      {
	ControlsOff();
	flightModel->SetEngineControlPer(0.0);
	flightModel->SetVelocity(v);
	flightModel->SetPitch(flightModel->GetRestPitch());
	flightModel->SetYawControlPer(0.0);
	flightModel->SetPitchControlPer(0.0);
	flightModel->SetRollControlPer(0.0);
	flightModel->SetFlaps(1.0);
	flightModel->ExtendLandingGear();
	flightModel->SetWheelBrakes(0);
	mv.data3 = 0.0;
	mv.state = 1;
      }	
    break;

    case 1:
      ControlsOff();
      SETCONTROL(yawRateCtlDirect,0.0);
      SETCONTROL(pitchRateCtlDirect,0.0);
      flightModel->SetVelocity(v);
      if (GetClearForTakeoff())
	mv.state = 2;
      break;
		
    case 2:
      mv.data3 += timeFrame;
      ControlsOff();
      SETROLLPID(0.0);
      SETYAWPID(0.0);
      if (mv.data3 >= 0.1)
	{
	  flightModel->IncEngineControlPer(0.01);
	  mv.data3 = 0.0;
	}
      if (flightModel->GetEngineControlPer() >= 1.0)
	{
	  mv.state = 3;
	  flightModel->SetWheelBrakes(0);
	}
      break;

    case 3:
      ControlsOff();
      SETROLLPID(0.0);
      SETYAWPID(0.0);
      flightModel->SetEngineControlPer(1.0);
      flightModel->SetWEP(1);
      if (flightModel->GetAirSpeedFPS() > flightModel->GetStallSpeedFPS() + 10.0)
	mv.state = 4;
      break;

    case 4:
      ControlsOff();
      SETROLLPID(0.0);
      SETYAWPID(0.0);
      SETCONTROL(gCtl,1.0);
      if (flightModel->GetHeightAGL() > 100.0)
	{
	  flightModel->SetFlaps(0.0);
	  flightModel->RetractLandingGear();
	  PushManeuver(sManeuver::CLIMB,0,mv.data0,100.0);
	  mv.state = 5;
	}
      break;

    case 5:
      if (mnvrStackPtr == mv.stackLevel)
	{
	  PushManeuver(sManeuver::STANDARD_TURN,IMNVR_LEFT,
		       aiPILOT_CIRCLE_BANK_DEG,
		       standardTurnSpeedMPH);
	  mv.state = 6;
	  mv.done = 1;
	}
      break;

    case 6:
      if (mnvrStackPtr == mv.stackLevel)
	PushManeuver(sManeuver::STANDARD_TURN,IMNVR_LEFT,
		     aiPILOT_CIRCLE_BANK_DEG,
		     standardTurnSpeedMPH);
      break;
    }
}

/*************************************************************
 * Landing                                                   *
 *************************************************************/
void aiPilot::DoLand(sManeuverState &)
{
  EnableGroundCollision(0);
  /*	
	switch (mv.state)
	{
	case 0:
	PushManeuver(sManeuver::STANDARD_TURN,0,aiPILOT_CIRCLE_BANK_DEG,
	standardTurnSpeedMPH);
	mv.state = 1;
	break;

	case 1:
	if (GetClearForLanding())
	{
	ClearManeuverStackTo(mv.stackLevel);
	mv.state = 2;
	}
	break;

	case 2:
	{
	sPoint turnPoint;
	const char *fieldId = navInfo.waypoint.GetFieldId();
	int runwayNo = navInfo.waypoint.GetRunwayNo();
	iAirfield *airfield = iAirfield::GetiAirfield(fieldId);
	if (airfield)
	{
	iRunway *runway = airfield->GetRunway(runwayNo);
	if (runway)
	{
	sVector v0,w0,w1;
	sREAL d0;
	sREAL dE;
	sREAL alpha,beta;
	sPoint position;
	sAttitude attitude;
	sVector	 mat[3];
	sREAL		 t0;
	sREAL		 tsSq;
	sREAL		 distance;
	sPoint	 tpCheck;

	t0 = tan(_degree * aiPILOT_CIRCLE_BANK_DEG);
	sREAL turnSpeedFPS = sMPH2FPS(standardTurnSpeedMPH);
	tsSq = turnSpeedFPS * turnSpeedFPS;
	t0 *= 32.2;
	sREAL turnRadius = tsSq / t0;
	runway->GetLandingTurnPoint(turnPoint,finalApproachDistance,turnRadius,0);
	runway->GetLandingTurnPoint(tpCheck,finalApproachDistance,0,0);
	turnPoint.z = finalApproachAGL;
					
	GetPositionAndAttitude(position,attitude);
	position.z = finalApproachAGL;

	d0 = Distance(position,turnPoint);
	alpha = acos(turnRadius / d0);
	dE = d0 * sin(alpha);
	beta = Pi_2 - alpha;
	LOG("turnRadius: %5.3f, dE: %5.3f  beta %3.3f",turnRadius,dE,beta / _degree);
					
	w1 = turnPoint - position; 
	sVector2sAttitude(w1,attitude);
	attitude.yaw += beta;
	attitude.pitch = attitude.roll = 0.0;
	v0.y = 1.0;					
	sMatrix3Build(attitude,mat);
	sMatrix3Rotate(mat,v0,w0);
	w0 *= dE;
	navInfo.navPoint = position + w0;
	distance = Distance(navInfo.navPoint,turnPoint);

	LOG("navPoint %5.1f %5.1f %5.1f",
	navInfo.navPoint.x, navInfo.navPoint.y, navInfo.navPoint.z);
	navInfo.navPoint = turnPoint;

	NAV_ALTITUDE = finalApproachAGL;
	navInfo.waypoint.speed = standardTurnSpeedMPH;
	}
	}
	PushManeuver(sManeuver::NAVIGATE);
	mv.state = 3;
	}
	break;

	case 3:
	if (navInfo.range <= 800.0 )
	{
	ClearManeuverStackTo(mv.stackLevel);
	const char *fieldId = navInfo.waypoint.GetFieldId();
	int runwayNo = navInfo.waypoint.GetRunwayNo();
	iAirfield *airfield = iAirfield::GetiAirfield(fieldId);
	if (airfield)
	{
	iRunway *runway = airfield->GetRunway(runwayNo);
	if (runway)
	{
	navInfo.navPoint = runway->GetPosition();
	NAV_ALTITUDE = navInfo.navPoint.z;
	}
	}
	PushManeuver(sManeuver::STANDARD_TURN,0,aiPILOT_CIRCLE_BANK_DEG,
	standardTurnSpeedMPH);
	mv.state = 4;
	}
	break;

	case 4:
	GetNavigationGeometry(navInfo);
	if (fabs(NAV_YAW) <= Pi_4 * 0.2)
	{
	ClearManeuverStackTo(mv.stackLevel);
	PushManeuver(sManeuver::NAVIGATE);
	mv.state = 5;
	}
	break;
	
	case 5:
	break;

	}
	*/

}

/************************************************************
 * Taxiing                                                  *
 ************************************************************/
void aiPilot::DoTaxi(sManeuverState &)
{
  EnableGroundCollision(0);
}





