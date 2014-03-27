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
 * File   : aigunner.cpp                         *
 * Date   : April, 1998                          *
 * Author : Dan Hammer                           *
 * aiGunner class definition                     *
 *************************************************/
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include "aigunner.h"

#define TARGET_GEOMETRY		target.geometry
#define TARGET_INFO			target.info
#define TARGET_IDX			target.idx
#define TARGET_FLAGS			target.flags
#define TARGET_ACTIVE		target.active
#define TARGET_RANGE			TARGET_GEOMETRY.range
#define TARGET_DRANGE		TARGET_GEOMETRY.dRange
#define TARGET_ATTITUDE		TARGET_GEOMETRY.relAttitude
#define TARGET_YAW			TARGET_GEOMETRY.relAttitude.yaw
#define TARGET_PITCH			TARGET_GEOMETRY.relAttitude.pitch
#define TARGET_ASPECT		TARGET_GEOMETRY.aspect
#define TARGET_VELOCITY		TARGET_GEOMETRY.worldVelocity
#define TARGET_POSITION		TARGET_GEOMETRY.worldPosition
#define TARGET_ALTITUDE		TARGET_GEOMETRY.worldPosition.z
#define TARGET_HEADING		TARGET_GEOMETRY.worldHeading
#define LEAD_POINT			TARGET_GEOMETRY.gunLeadPoint
#define LEAD_ATTITUDE		TARGET_GEOMETRY.gunLeadAttitude
#define LEAD_VECTOR			TARGET_GEOMETRY.gunLeadVector
#define LEAD_YAW				TARGET_GEOMETRY.gunLeadAttitude.yaw
#define LEAD_PITCH			TARGET_GEOMETRY.gunLeadAttitude.pitch
#define CONE_MAX_PITCH		weaponLimits.coneOfFire.max.pitch
#define CONE_MAX_YAW			weaponLimits.coneOfFire.max.yaw
#define CONE_MIN_PITCH		weaponLimits.coneOfFire.min.pitch
#define CONE_MIN_YAW			weaponLimits.coneOfFire.min.yaw
#define GUN_PITCH				gunAttitude.pitch
#define GUN_YAW				gunAttitude.yaw
#define GUN_LEAD_RANGE_SQ	TARGET_GEOMETRY.gunLeadRangeSq


/*******************************************
 * aaiGunner members                          *
 *******************************************/

/*
 * default constructor
 */
aiGunner::aiGunner()
{
	index = 0;
	flightModel = NULL;
	timeFrame = 0.0;
	isDead = isEjected = 0;
	affiliation = 0;
	hasTarget = 0;
	bulletRadius = 2.6;
	gunPitchRate = aiGUN_PITCH_RATE;
	gunYawRate = aiGUN_YAW_RATE;
}

aiGunner::aiGunner(sFlightModel *flightModel, int index,
					  unsigned long ownerIdx, int affiliation, sREAL bulletRadius,
                 sREAL gunPitchRate, sREAL gunYawRate,
                 sAttitude *jiggleAtt)
{
	this->flightModel = flightModel;
	this->index = index;
	this->ownerIdx = ownerIdx;
	this->affiliation = affiliation;
	this->bulletRadius = bulletRadius;
	isDead = isEjected = 0;
	hasTarget = 0;
	timeFrame = 0.0;
	this->gunPitchRate = gunPitchRate;
	this->gunYawRate = gunYawRate;
	if (jiggleAtt != NULL)    
		this->jiggleAtt = *jiggleAtt;
}

/*
 * Initialization
 */
void aiGunner::Init()
{
	timeFrame = 0.0;
	SetDeath(0);
	SetEjected(0);
	hasTarget = 0;
	weaponLimits.Init();
}

void aiGunner::Destroy()
{

}

void aiGunner::Update(double timeFrame)
{
	this->timeFrame = timeFrame;
	if (!IsActive())
		return;
	weaponLimits.Update(timeFrame);
	if (hasTarget)
	{
		GetTargetGeometry();
		AdjustGun();
		Shoot();
		hasTarget = sTarget::GetTargetActive(TARGET_IDX);
	}
}

void aiGunner::AdjustGun()
{
	sREAL pitchStep,yawStep;
	sREAL d;

	if (TARGET_RANGE > weaponLimits.maxRange)
		return;
	if (sIsBetween(CONE_MIN_PITCH,CONE_MAX_PITCH,LEAD_PITCH))
	{
		d = sGetCircularDistance(GUN_PITCH,LEAD_PITCH);
		pitchStep = gunPitchRate * timeFrame;
		if (pitchStep > fabs(d))
			pitchStep = fabs(d);
		if (d < 0)
			pitchStep = -pitchStep;
		GUN_PITCH = sIncrementAngle(GUN_PITCH,pitchStep);
	}
	if (sIsBetween(CONE_MIN_YAW,CONE_MAX_YAW,LEAD_YAW))
	{
		d = sGetCircularDistance(GUN_YAW,LEAD_YAW);
		yawStep = gunYawRate * timeFrame;
		if (yawStep > fabs(d))
			yawStep = fabs(d);
		if (d < 0)
			yawStep = -yawStep;
		GUN_YAW = sIncrementAngle(GUN_YAW,yawStep);
	}
}

/*
 * return index of current target
 */
unsigned long aiGunner::GetTargetIdx()
{
  return (TARGET_IDX);
}

void aiGunner::SetTarget(unsigned long targetIdx)
{
	TARGET_IDX = targetIdx;
	hasTarget = 1;
	target.threatValue = ITARGET_MIN_THREAT_VALUE;
}

/*
 *  Return whether or not we are in shoot params
 */
int aiGunner::Shoot()
{
	int result;
	GetWeaponLimits(weaponLimits);
	result = (GUN_LEAD_RANGE_SQ <= weaponLimits.maxRange * weaponLimits.maxRange &&
					sIsBetween(CONE_MIN_PITCH,CONE_MAX_PITCH,LEAD_PITCH) &&
					sIsBetween(CONE_MIN_YAW,CONE_MAX_YAW,LEAD_YAW));
	result = weaponLimits.UpdateBurst(result);
	return (result);
}


/**************************************************************
 * calculate target geometry                                  *
 **************************************************************/
void aiGunner::GetTargetGeometry()
{
	sREAL			newRange;
	sVector		temp;
	sREAL			t;
	sPoint		targetPosition;
	sAttitude	attitudeToTarget;
	sVector		directionToTarget;
	sVector		targetHeading;
	sVector		targetVelocity;
	sPoint		ourPosition;
	sAttitude	ourAttitude;

	flightModel->GetPositionAndAttitude(ourPosition,ourAttitude);
	sTarget::GetTargetInfo(TARGET_IDX,targetPosition,targetVelocity,
						 targetHeading);
	GetAttitude(targetPosition,directionToTarget,attitudeToTarget);
	TARGET_VELOCITY = targetVelocity;
	TARGET_POSITION = targetPosition;
	TARGET_HEADING = targetHeading;
	TARGET_ATTITUDE = attitudeToTarget;
	newRange = directionToTarget.Magnitude();
	TARGET_DRANGE = (newRange - TARGET_RANGE) / timeFrame;
	TARGET_RANGE = newRange;

	/* Get gun lead information */
	t = TARGET_RANGE / weaponLimits.initialVelocity;
	temp = targetVelocity;
	/* get velocity vector for that time */
	temp *= t;
	/* add it to target's position to get lead */
	LEAD_POINT = targetPosition + temp;
	LEAD_POINT.z += 16.0 * t * t;
	/* calc attitude for lead */
	GetAttitude(LEAD_POINT,LEAD_VECTOR,LEAD_ATTITUDE);
	GUN_LEAD_RANGE_SQ = sDistanceSquared(ourPosition,LEAD_POINT);
}






