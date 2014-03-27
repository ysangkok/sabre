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
 * File   : aipilot4.cpp                         *
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
 * Intercept                                                 *
 * Get within engagement distance                            *
 *************************************************************/
void aiPilot::DoIntercept(sManeuverState &mv)
{
	enum { BASE, TURN, EXTEND, AVOID_GROUND, CLIMB, NOSEON, DONE };

	engageMaxSpeed = flightModel->GetMaxSpeedFPS() * 0.7;
	EnableGroundCollision(1);
	curTarget = &interceptTarget;

	GetTargetGeometry(TARGET_IDX,TARGET_GEOMETRY);
	GetTargetFlags(interceptTarget);
	/* check ground collision */
	if (mv.state != AVOID_GROUND)
	{
		if (flightModel->GetHeightAGL() + (deltaAltitude * grndColSecs) <= grndColAGL)
		{
			ClearManeuverStackTo(mv.stackLevel);
			PushManeuver(sManeuver::PULLUP,0,
						grndColRcvrAGL,
						grndColPullUpAngle);
				mv.state = AVOID_GROUND;
			return;
		}
	}
	/* check if we're done */
	if (mv.state != AVOID_GROUND && mv.state != DONE && (!TARGET_ACTIVE || 
											TARGET_RANGE < ENGAGE_DISTANCE))
		mv.state = DONE;

	switch (mv.state)
	{
	case BASE:
		if (flightModel->GetAirSpeedFPS() < engageMaxSpeed * 0.6)
		{
			PushManeuver(sManeuver::EXTEND,0,engageMaxSpeed * 1.1,20,8);          
			mv.state = EXTEND;
		}
		else if (TARGET_FLAGS.rear)
		{
			if (TARGET_YAW < 0.0)
				PushManeuver(sManeuver::HARD_TURN,IMNVR_LEFT);
			else
				PushManeuver(sManeuver::HARD_TURN,IMNVR_RIGHT);
			mv.state = TURN;
		}
		else if (TARGET_FLAGS.forward && !TARGET_FLAGS.low)
		{
			PushManeuver(sManeuver::CLIMB,0,TARGET_ALTITUDE + 500,200);
			mv.state = CLIMB;   
		}
		else
		{
			SetNoseOn(TARGET_PITCH,TARGET_YAW);
			flightModel->SetWEP(1);
			flightModel->SetEngineControlPer(1.0);
			mv.state = NOSEON;
		}
		break;

	case NOSEON:
		if (TARGET_FLAGS.onNose)
			mv.state = BASE;
		SetNoseOn(TARGET_PITCH,TARGET_YAW);
		break;

	case TURN:
		if (TARGET_FLAGS.forward)
			ClearManeuverStackTo(mv.stackLevel);
		if (mnvrStackPtr == mv.stackLevel)
			mv.state = BASE;
		break;        

	case CLIMB:
		if (TARGET_FLAGS.low || TARGET_FLAGS.rear)
			ClearManeuverStackTo(mv.stackLevel);
		if (mnvrStackPtr == mv.stackLevel)
			mv.state = BASE;
		break;

	case EXTEND:
		if (mnvrStackPtr == mv.stackLevel)
			mv.state = BASE;
		break;

	case DONE:
		if (mnvrStackPtr == mv.stackLevel)
			PushManeuver(sManeuver::STRAIGHT_AND_LEVEL);
		break;
	}
}

/*************************************************************
 * Navigation and formation flying                           *
 *************************************************************/
void aiPilot::DoNavigate(sManeuverState &mv)
{
	EnableGroundCollision(1);
	GetNavigationGeometry(navInfo);
	/* check ground collision */
	if (mv.state != 6)
	{
		if (flightModel->GetHeightAGL() + (deltaAltitude * grndColSecs) <= grndColAGL)
		{
			mv.state = 6;
			ClearManeuverStackTo(mv.stackLevel);
			PushManeuver(sManeuver::PULLUP,0,
						grndColRcvrAGL,
						grndColPullUpAngle);
			return;
		}
	}
	
	if (mv.state != 6)
	{
		if (NAV_RANGE <= 100.0)
		{
			 ClearManeuverStackTo(mv.stackLevel);
			 if (mv.state != 5)
			 {
				PushManeuver(sManeuver::STANDARD_TURN,IMNVR_LEFT,
								aiPILOT_CIRCLE_BANK_DEG,
								sMPH2FPS(aiPILOT_CIRCLE_SPEED_MPH));
				mv.state = 5;			
			 }
		}
		else if (mv.state == 5)
		{
			 ClearManeuverStackTo(mv.stackLevel);
			 mv.state = 0;
		}
	}

	switch (mv.state)
	{
	case 0:
		PushManeuver(sManeuver::STRAIGHT_AND_LEVEL);
		mv.state = 1;
		break;

	case 1:
		if (mnvrStackPtr == mv.stackLevel)
			mv.state = 2;
		break;


	case 2:
		if (fabs(NAV_YAW) < Pi_4)
			mv.state = 4;
		else
		{
			int turn_bits;
			if (NAV_YAW > 0.0)
				turn_bits = IMNVR_RIGHT;
			else
				turn_bits = IMNVR_LEFT;
			PushManeuver(sManeuver::STANDARD_TURN,turn_bits,
							aiPILOT_CIRCLE_BANK_DEG,
							sMPH2FPS(aiPILOT_CIRCLE_SPEED_MPH));
			mv.state = 3;
		}
		break;

	case 3:
		if (fabs(NAV_YAW) < Pi_4)
		{
			ClearManeuverStackTo(mv.stackLevel);
			mv.state = 4;
		}
		SETCONTROL(speedCtl,navInfo.waypoint.speed);
		break;

	case 4:
	{
		sREAL altDiff = NAV_ALTITUDE - flightModel->GetAltitudeFPS();
		if ((fabs(NAV_YAW) < _degree) && (fabs(altDiff) > NAV_ALT_THRESH))
		{
			if (altDiff > 0)
				PushManeuver(sManeuver::CLIMB,0,NAV_ALTITUDE,10.0); 
			else
			{
				sREAL descentAngle = atan(fabs(altDiff) / NAV_RANGE);
				if (descentAngle > Pi_4)
					descentAngle = Pi_4;
				PushManeuver(sManeuver::DESCEND,0,NAV_ALTITUDE,10.0,descentAngle / _degree);
			}
			mv.state = 7;
			break;
		}
		
		if (fabs(NAV_YAW) < _degree)
		{
			PushManeuver(sManeuver::STRAIGHT_AND_LEVEL);
			mv.state = 8;
			break;
		}

	   sREAL rollNeeded;
		sREAL limitRoll = Pi_4;
		ControlsOff();
		rollNeeded = atan(flightModel->GetAirSpeedFPS() *
						 NAV_YAW / 32.2);
		if (fabs(rollNeeded) > limitRoll)
		{
			if (rollNeeded < 0.0)
			  rollNeeded = -limitRoll;
			else
			  rollNeeded = limitRoll;
		}
		SETROLLPID(rollNeeded);
		SETPITCHPID(NAV_PITCH);
		SETYAWPID(NAV_YAW);
		SETCONTROL(speedCtl,navInfo.waypoint.speed);
		break;
	}

	case 5:
		if (mnvrStackPtr == mv.stackLevel)
			PushManeuver(sManeuver::STANDARD_TURN,IMNVR_LEFT,
								aiPILOT_CIRCLE_BANK_DEG,
								sMPH2FPS(aiPILOT_CIRCLE_SPEED_MPH));
		break;

	case 6:
		if (mnvrStackPtr == mv.stackLevel)
			mv.state = 0;
		SETCONTROL(speedCtl,navInfo.waypoint.speed);
		break;

	case 7:
		if (fabs(NAV_YAW) > _degree * 12.0)
			ClearManeuverStackTo(mv.stackLevel);
		if (mnvrStackPtr == mv.stackLevel)
			mv.state = 4;
		SETCONTROL(speedCtl,navInfo.waypoint.speed);
		break;

	case 8:
		if (fabs(NAV_YAW) > _degree * 3.0)
			ClearManeuverStackTo(mv.stackLevel);
		if (fabs(NAV_ALTITUDE - flightModel->GetAltitudeFPS()) > NAV_ALT_THRESH)
			ClearManeuverStackTo(mv.stackLevel);
		if (mnvrStackPtr == mv.stackLevel)
			mv.state = 4;
		SETCONTROL(speedCtl,navInfo.waypoint.speed);
		break;

	}
}

/*
 *  Fly in formation with the player or another
 *  aiPilot
 */

void aiPilot::DoFormation(sManeuverState &mv)
{
	EnableGroundCollision(1);
	curTarget = &formationTarget;
	GetTargetGeometry(TARGET_IDX,TARGET_GEOMETRY);
	GetTargetFlags(*curTarget);

	/* check ground collision */
	if (mv.state != 6)
	{
		if (flightModel->GetHeightAGL() + (deltaAltitude * grndColSecs) <= grndColAGL)
		{
			mv.state = 6;
			ClearManeuverStackTo(mv.stackLevel);
			PushManeuver(sManeuver::PULLUP,0,
						flightModel->GetHeightAGL() + grndColRcvrAGL,
						grndColPullUpAngle);
			return;
		}
	}

	switch (mv.state)
	{
		case 0:
		default:
		{
			sREAL rollNeeded;
			ControlsOff();
			if (fabs(FORMATION_PITCH) > _degree * 30.0 ||
				 fabs(FORMATION_YAW) > _degree * 30.0)
				rollNeeded = Point2Roll(TARGET_GEOMETRY.formationPoint);
			else
				rollNeeded = atan(flightModel->GetAirSpeedFPS() *
							FORMATION_YAW / 32.2);
			SETROLLPID(rollNeeded);
			SETPITCHPID(FORMATION_PITCH);
			SETYAWPID(FORMATION_YAW);
			flightModel->SetWEP(0);
			sREAL speedGoal = CalcClosureSpeed(formationTarget.geometry,1);
			SETCONTROL(speedCtl,speedGoal);
		}
		break;

		case 6:
		if (mnvrStackPtr == mv.stackLevel)
			mv.state = 0;
		break;
	}
}

void aiPilot::DoAirShow(sManeuverState &mv)
{
	EnableGroundCollision(1);
	GetNavigationGeometry(navInfo);
	/* check ground collision */
	if (mv.state != 6)
	{
		if (flightModel->GetHeightAGL() + (deltaAltitude * grndColSecs) <= grndColAGL)
		{
			LOG("Pulling Up: AGL = %5.3f, deltaAltitude = %5.3f, grndColSecs = %5.3f, grndColAGL = %5.3f",
				  flightModel->GetHeightAGL(),
				  deltaAltitude,
				  grndColSecs,
				  grndColAGL);
			mv.state = 6;
			ClearManeuverStackTo(mv.stackLevel);
			PushManeuver(sManeuver::PULLUP,0,
						grndColRcvrAGL,
						grndColPullUpAngle);
			return;
		}
	}


	switch (mv.state)
	{
	case 0:
		mv.data4 = 0.0;
		mv.data3 = sRandPer() * 5.0 + 5.0;
		PushManeuver(sManeuver::STRAIGHT_AND_LEVEL,IMNVR_LOOPBIT);
		mv.state = 1;
		break;
	
	case 1:
		mv.data4 += timeFrame;
		if (mv.data4 >= mv.data3)
		{
			ClearManeuverStackTo(mv.stackLevel);
			mv.state = 2;
		}
		break;

	case 2:
	{
		sREAL altMin = 3000.0;
		sREAL altDiff = altMin - flightModel->GetAltitudeFPS();
		if ((fabs(NAV_YAW) < _degree * 2) && altDiff > 2000)
		{
			PushManeuver(sManeuver::CLIMB,0,altMin + 500, 
							 NAV_ALT_THRESH / 4.0);
			mv.state = 7;
			break;
       }

		int maneuver = (int) mv.data0++;
		if (mv.data0 > 5)
			mv.data0 = 0;

		switch (maneuver)
		{
			
		case 0:
		{
			int turn_bits;
			if (sFlipCoin())
				turn_bits = IMNVR_RIGHT;
			else
				turn_bits = IMNVR_LEFT;
			PushManeuver(sManeuver::STANDARD_TURN,turn_bits,
							sRandPer() * 30.0 + 15.0,
							sRandPer() * 300.0);
			mv.data4 = 0.0;
			mv.data3 = sRandPer() * 16.0 + 8.0;
			mv.state = 5;
		}
			break;

		case 1:
		{
			PushManeuver(sManeuver::INVERT);
			mv.data4 = 0.0;
			mv.data3 = sRandPer() * 16.0 + 16.0;
			mv.state = 5;
		}
			break;

		case 2:
		{
			int turn_bits;
			if (sFlipCoin())
				turn_bits = IMNVR_RIGHT;
			else
				turn_bits = IMNVR_LEFT;
			PushManeuver(sManeuver::STANDARD_TURN,turn_bits,
							sRandPer() * 30.0 + 15.0,
							aiPILOT_CIRCLE_BANK_DEG);
			mv.data4 = 0.0;
			mv.data3 = sRandPer() * 10.0 + 3.0;
			mv.state = 5;
		}
			break;

		case 3:
		{
			int turn_bits;
			if (sFlipCoin())
				turn_bits = IMNVR_RIGHT;
			else
				turn_bits = IMNVR_LEFT;
			PushManeuver(sManeuver::AILERON_ROLL,turn_bits);
			mv.data4 = 0.0;
			mv.data3 = sRandPer() * 16.0 + 16.0;
			mv.state = 5;		
		}
			break;

		case 4:	
		{
			int turn_bits;
			if (sFlipCoin())
				turn_bits = IMNVR_RIGHT;
			else
				turn_bits = IMNVR_LEFT;
			PushManeuver(sManeuver::BREAK_TURN,turn_bits);
			mv.data4 = 0.0;
			mv.data3 = sRandPer() * 10.0 + 6.0;
			mv.state = 5;
		}
			break;

		case 5:
		default:
			PushManeuver(sManeuver::SPLIT_S,0,splitSGs);
			mv.state = 6;
			break;

		}
	}
	break;

	case 5:
		mv.data4 += timeFrame;
		if (mv.data4 >= mv.data3)
			ClearManeuverStackTo(mv.stackLevel);
		if (mnvrStackPtr == mv.stackLevel)
			mv.state = 0;
		break;

	case 6:
		if (mnvrStackPtr == mv.stackLevel)
			mv.state = 0;
		break;

	case 7:
		mv.data4 += timeFrame;
        if (fabs(NAV_YAW) > _degree * 12.0)
          ClearManeuverStackTo(mv.stackLevel);
        if (mnvrStackPtr == mv.stackLevel)
          mv.state = 0;
        break;

	case 8:
		PushManeuver(sManeuver::EXTEND,0,flightModel->GetMaxSpeedFPS());
		mv.state = 9;
		break;

	case 9:
		mv.data4 += timeFrame;
		if (mv.data4 >= mv.data3)
		{
			ClearManeuverStackTo(mv.stackLevel);
			mv.state = 0;
		}
		if (mnvrStackPtr == mv.stackLevel)
		{
			mv.state = 10;
			mv.data3 = 30.0;
			mv.data4 = 0.0;
			PushManeuver(sManeuver::IMMELMAN,0,immelmanGs);
		}
		break;

	case 10:
		mv.data4 += timeFrame;
		if (mv.data4 >= mv.data3)
			ClearManeuverStackTo(mv.stackLevel);
		if (mnvrStackPtr == mv.stackLevel)
			mv.state = 0;
		break;

	}
}

