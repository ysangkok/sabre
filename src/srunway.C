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
 * File   : srunway.cpp                          *
 * Date   : April, 1998                          *
 * Author : Dan Hammer                           *
 *************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "srunway.h"

sRunway::sRunway(const srunway_info &rwi)
{
sPoint position;
sAttitude attitude;

	position = sPoint(C(rwi.x), C(rwi.y), C(rwi.z));
	attitude.yaw = C(rwi.heading);
	finalApproachHeading = attitude.yaw;
	takeoffHeading = sIncrementAngle(attitude.yaw,Pi);
	attitude.roll = attitude.pitch = 0.0;
	length = C(rwi.length);
	width = C(rwi.width);
	sSlewer::SetPositionAndAttitude(position,attitude);
}

sRunway::~sRunway() = default;

void sRunway::GetLandingTurnPoint(sPoint &turnPoint, sREAL approachDistance, sREAL turnRadius,
									int clockWise)
{
sVector v0,w0;

	SelectTakeoffHeading();
	if (clockWise)
		v0.x = turnRadius;
	else
		v0.x = -turnRadius;
	v0.y = approachDistance;
	BodyVector2WorldVector(v0,w0);
	turnPoint = pos + w0;
}

void sRunway::GetTakeoffPositionAndAttitude(sPoint &position, sAttitude &attitude)
{
sVector v0,w0;

	SelectFinalApproachHeading();
	v0.y = length;
	BodyVector2WorldVector(v0,w0);
	position = pos + w0;
	attitude.roll = attitude.pitch = 0.0;
	attitude.yaw = takeoffHeading;
}
