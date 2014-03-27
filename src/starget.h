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
 * File   : starget.h                            *
 * Date   : April, 1998                          *
 * Author : Dan Hammer                           *
 * Targeting thingees                            *
 *************************************************/
#ifndef __starget_h
#define __starget_h

#include "sobject.h"
#include "smath.h"
#include "stact.h"

#ifdef __cplusplus

#define T_AVERAGING_N				16
#define FLAG_AVERAGING_N			30
#define iNOTARGET_INDEX				-2
#define TRG_NO_TARGET				-16
#define ITARGET_MIN_THREAT_VALUE -99999.0

class sTargetGeometry 
{
public:
  enum { LEFT_ASPECT, RIGHT_ASPECT };
  sPoint    worldPosition;         /* target location in world */
  sVector   worldHeading;          /* target heading in world */	
  sVector   worldDirection;        /* target direction in world from us */
  sAttitude relAttitude;           /* pitch,roll,yaw relative to our heading */
  sREAL     angleOff;              /* angle between our heading and target heading */
  sREAL     aspect;                /* angle between target heading and direction from us */
  int       aspectDir;             /* left or right aspect */
  sREAL     range;                 /* range */
  sREAL     ranges[T_AVERAGING_N]; /* average range array */
  int       rng_idx;
  sREAL     dRange;                /* change in range (per sec) */
  sVector   worldVelocity;         /* velocity in world */
  sREAL	    airSpeed;              /* air speed (FPS) */
  sPoint    leadPoint;             /* lead pursuit point */
  sVector   leadVect;              /* vector to lead pursuit point */
  sAttitude leadAttitude;          /* attitude to lead pursuit point */
  sAttitude leadRate;		   /* normalized lead angular rates */
  sPoint    lagPoint;              /* lag pursuit point */
  sAttitude lagAttitude;           /* attitude to lag pursuit point */
  sPoint    gunLeadPoint;          /* gun lead point */
  sAttitude gunLeadAttitude;       /* attitude to gun lead point */
  sREAL     gunTOF;		   /* time-of-flight of gun projectile */
  sVector   gunLeadVector;         /* vector to gun lead pursuit point */
  sREAL     gunLeadRangeSq;        /* range to gun lead point */
  /* for formation flying */
  sPoint    formationPoint;
  sAttitude formationAttitude;
  sREAL     formationRange;
  sREAL		formationDRange;
  sREAL     turnRate;              /* current turning rate */
  sREAL     load;                  /* current load */

  sTargetGeometry();
};

/* flags indicating various target conditions   */
class sTargetFlags : public sObject
{
public:
  int rangeClose;   /* target is getting nearer   */
  int rangeOpen;    /* target is getting further  */
  int turning;      /* target is turning          */
  int turning_not;  /* not turning                */
  int below;        /* target below               */
  int above;        /* target above               */
  int low;          /* target is high             */
  int high;         /* target is low              */
  int veryLow;      /* target very low            */
  int veryHigh;     /* target very high           */
  int headOnAspect; /* target pointing towards us */
  int tailAspect;   /* target pointing away       */
  int forward;      /* target in forward quarter  */
  int rear;         /* target in rear quarter     */
  int onNose;       /* target on nose             */
  int onTail;       /* target on tail             */
  int highAOT;      /* high angle off             */
  int lowAOT;       /* low angle off              */

  void Reset()
    {
      rangeClose = 0;
      rangeOpen = 0;
      turning = 0;
      turning_not = 0;
      above = 0;
      below = 0;
      high = 0;
      low = 0;
      veryHigh = 0;
      veryLow = 0;
      headOnAspect= 0;
      tailAspect = 0;
      onNose = 0;
      onTail = 0;
      forward = 0;
      rear = 0;
      highAOT = 0;
      lowAOT = 0;
    }

  void Add(const sTargetFlags &fl);
  void Set(int max = 14);

  sTargetFlags()
    {
      Reset();
    }
};

class sTargetInfo : public sObject
{
public:
  char	model[32];
  int	tacticalCaps[SPT_NTACTICS];
  sREAL	weaponsRange;
  sREAL	weaponsEffect;
  sREAL	maxSpeed;
  sTargetInfo();
};

class sTarget : public sObject
{
public:
  sTargetInfo      info;
  sTargetGeometry  geometry;
  sTargetFlags     flags;
  sTargetFlags     tFlags[FLAG_AVERAGING_N];
  int              tFlagIndex;
  unsigned long    idx;
  int              active;
  sREAL            threatValue;
  sTarget()
    {
      tFlagIndex = 0;
      active = 1;
      idx = (unsigned long) TRG_NO_TARGET;
      threatValue = ITARGET_MIN_THREAT_VALUE;
    }
  static void GetTargetInfo(unsigned long targetIdx, sPoint &position, 
			    sVector &velocity, sVector &heading);
  static int GetTargetActive(unsigned long targetIdx);
};

/*
 * surface target info
 */
class sSurfaceTarget : public sObject
{
public:
  int        index;
  int        isStatic;
  sPoint     worldPoint;
  sPoint     alignPoint;
  sVector    worldDirection;
  sVector    worldVelocity;
  sAttitude  relAttitude;
  sREAL      range;
  sREAL      dRange;
  sREAL      vertDistance;
  sREAL      dVertDistance;
    
  sSurfaceTarget()
    {
      isStatic = 0;
      range = dRange = vertDistance = dVertDistance = 0.0;
    }  
};

#endif /* __cplusplus */
#endif
