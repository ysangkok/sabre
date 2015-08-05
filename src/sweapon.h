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
 * File   : sweapon.h                            *
 * Date   : April, 1998                          *
 * Author : Dan Hammer                           *
 *************************************************/
#ifndef SWEAPON_H
#define SWEAPON_H

#define swDEFAULT_CONE_PITCH        (static_cast<sREAL>(_degree * 1.0))
#define swDEFAULT_CONE_YAW          (static_cast<sREAL>(_degree * 1.0))
#define swDEFAULT_MAX_RANGE         (static_cast<sREAL>(1360.0))
#define swDEFAULT_INITIAL_VELOCITY  (static_cast<sREAL>(2502.0))
#define swDEFAULT_BURST_TIME        (static_cast<sREAL>(0.5))
#define swDEFAULT_BURST_PAUSE       (static_cast<sREAL>(0.4))
#define swDEFAULT_CONVERGANCE       (static_cast<sREAL>(900.0))
#define swMAX_RANGE_FACTOR          (static_cast<sREAL>(0.6))

#ifdef __cplusplus

#include "sobject.h"
#include "smath.h"
#include "starget.h"

class sConeOfFire
{
public:
    sAttitude    min;
    sAttitude   max;
    sConeOfFire()
    {}
};

class sWeaponLimits
{
public:

    sREAL            maxRange;
    sREAL            minRange;
    sAttitude    cone;
    sVector        jiggle;
    sREAL            initialVelocity;
    sREAL            fireInterval;
    sREAL            fireTimeAccumulator;
    sConeOfFire    coneOfFire;
    enum { BURST_INIT, BURST_BURST, BURST_PAUSE };
    int            burstState;
    sREAL            burstTime;
    sREAL            burstPauseTime;
    sREAL            burstTimeAccumulator;
    sREAL            convergance;

    sWeaponLimits()
    {
        maxRange = swDEFAULT_MAX_RANGE;
        minRange = 20.0;
        coneOfFire.min.yaw = -swDEFAULT_CONE_YAW / 2.0;
        coneOfFire.min.pitch = -swDEFAULT_CONE_PITCH / 2.0;
        coneOfFire.max.yaw = swDEFAULT_CONE_YAW / 2.0;
        coneOfFire.max.pitch = swDEFAULT_CONE_PITCH / 2.0;
        cone.pitch = swDEFAULT_CONE_PITCH;
        cone.yaw = swDEFAULT_CONE_YAW;
        jiggle.x = jiggle.y = jiggle.z = 0.0;
        initialVelocity = swDEFAULT_INITIAL_VELOCITY;
        fireInterval = static_cast<sREAL>(0.06);
        fireTimeAccumulator = 0.0;
        burstTimeAccumulator = 0.0;
        burstState = BURST_INIT;
        burstTime = swDEFAULT_BURST_TIME;
        burstPauseTime = swDEFAULT_BURST_PAUSE;
        convergance = swDEFAULT_CONVERGANCE;
    }

    void Init()
    {
      fireTimeAccumulator = 0.0;
    }

    void Update(sREAL timeFrame)
    {
      burstTimeAccumulator += timeFrame;
      fireTimeAccumulator += timeFrame;
    }

    int UpdateBurst(int inFireParams);
    
};
#endif /* __cplusplus */

#endif

