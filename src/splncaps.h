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
 * File   : splncaps.h                           *
 * Date   : April, 1998                          *
 * Author : Dan Hammer                           *
 * Plane capabilities                            *
 *************************************************/
#ifndef __splncaps_h
#define __splncaps_h

#include "sobject.h"
#include "sarray.h"
#include "smath.h"
#include "spid.h"

#define sMAXPLANECAPS 32

typedef struct splane_caps_struct
{
	char    model[32];
	int     canDoAnglesFight;
	int     canDoEnergyFight;
	int     canStrafe;
	int     canRocket;
	int     canDiveBomb;
	int     canTorpedoBomb;
	int     canLightBomb;
	int     canHeavyBomb;

	sREAL   maxSpeed;
	sREAL   stallSpeed;
	sREAL   cornerSpeed;
	sREAL   climbSpeed;

	sREAL   weaponsRange;
	sREAL   weaponsEffect;

	sREAL   maxRollRate;
	sREAL   minRollRate;
	sREAL   rollAccelFactor;
	sREAL   formationLen;
	sREAL   formationOffset;

	_sPID pitchPid;
	_sPID rollPid;
	_sPID yawPid;
	_sPID gPid;

	sREAL			collisionAvoidSecs;		/* secs look-ahead to detect collision with target */
	sREAL			immelmanGs;					/* g's to pull for immelman maneuver */
	sREAL			immelmanAltMin;			/* min alt for immelman maneuver */
	sREAL			noseOnRollLimit;			/* limits roll when getting nose on target */
	sREAL			speedLossLimit;			/* limit airspeed loss during gun attack */
	sREAL			leadAdjustMult;			/* mult to increase gun lead point */
	sREAL			splitSGs;					/* g's to pull for split_s maneuver */
	sREAL			splitSAltMin;				/* min alt for split_s maneuver */
	sREAL			posGLimit;
	sREAL			negGLimit;
	sREAL			grndColSecs;				/* secs look-ahead to det. ground coll */
	sREAL			grndColAGL;					/* min AGL for ground coll */
	sREAL			grndColRcvrAGL;			/* min AGL to recover */
	sREAL			grndColPullUpAngle;		/* angle to pull up */
	sREAL			zoomAngle;

	sREAL			standardTurnSpeedMPH;
	sREAL			finalApproachDistance;
	sREAL			finalApproachAGL;
} splane_caps;

#ifdef __cplusplus

class sPlaneCaps : public sObject
{
public:
	char    model[32];
	int     canDoAnglesFight;
	int     canDoEnergyFight;
	int     canStrafe;
	int     canRocket;
	int     canDiveBomb;
	int     canTorpedoBomb;
	int     canLightBomb;
	int     canHeavyBomb;

	sREAL   maxSpeed;
	sREAL   stallSpeed;
	sREAL   cornerSpeed;
	sREAL   climbSpeed;

	sREAL   weaponsRange;
	sREAL   weaponsEffect;

	sREAL   maxRollRate;
	sREAL   minRollRate;
	sREAL   rollAccelFactor;
	sREAL   formationLen;
	sREAL   formationOffset;

	sPID pitchPid;
	sPID rollPid;
	sPID yawPid;
	sPID gPid;

	sREAL			collisionAvoidSecs;		/* secs look-ahead to detect collision with target */
	sREAL			immelmanGs;					/* g's to pull for immelman maneuver */
	sREAL			immelmanAltMin;			/* min alt for immelman maneuver */
	sREAL			noseOnRollLimit;			/* limits roll when getting nose on target */
	sREAL			speedLossLimit;			/* limit airspeed loss during gun attack */
	sREAL			leadAdjustMult;			/* mult to increase gun lead point */
	sREAL			splitSGs;					/* g's to pull for split_s maneuver */
	sREAL			splitSAltMin;				/* min alt for split_s maneuver */
	sREAL			posGLimit;
	sREAL			negGLimit;
	sREAL			grndColSecs;				/* secs look-ahead to det. ground coll */
	sREAL			grndColAGL;					/* min AGL for ground coll */
	sREAL			grndColRcvrAGL;			/* min AGL to recover */
	sREAL			grndColPullUpAngle;		/* angle to pull up */
	sREAL			zoomAngle;

	sREAL			standardTurnSpeedMPH;
	sREAL			finalApproachDistance;
	sREAL			finalApproachAGL;

	sPlaneCaps()
	{}
	virtual ~sPlaneCaps();

	sPlaneCaps(const splane_caps &);

	static sObjectArray sPlaneCapsArray;
	static void AddsPlaneCaps(sPlaneCaps &);
	static void AddsPlaneCaps(splane_caps *);

	static sPlaneCaps *GetsPlaneCaps(const char *model);
	static void FlushsPlaneCaps();
};
#endif

#endif
