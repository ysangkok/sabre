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
 * File   : spilcaps.h                           *
 * Date   : April, 1998                          *
 * Author : Dan Hammer                           *
 * Pilot caps class                              *
 * Overrides default values for various AI pilot *
 * members.                                      *
 *************************************************/
#ifndef SPILCAPS_H
#define SPILCAPS_H

#include "smath.h"

#define sMAXPILOTCAPS 32

/*
 *  equivalent structure for "C"-only situations
 */
typedef struct spilot_caps_struct
{
  /*
   *   id
   */
  char capsId[32];

  /*
   *  tactics overrides
   */
  sREAL targetRQCone;
  sREAL	targetFQCone;
  sREAL	aspectHeadOnCone;
  sREAL	aspectRearCone;
  sREAL	angleOffCone;
  sREAL	rearThreatDistance;
  sREAL rearThreatCone;     
  sREAL cancelRearThreat;
  sREAL	pursuitDistance;
  sREAL	pursuitCone;         
  sREAL	cancelPursuit;
  sREAL	turnThreshold;
  sREAL	altThreshold;
  sREAL	rangeThreshold;
  sREAL playerThreatDistance;
  sREAL playerThreatAspect;
  sREAL attackerCheckInterval;

  /*
   *  ground collision overrides
   */
  sREAL grndColSecs;
  sREAL	grndColAGL;
  sREAL	grndColRcvrAGL;
  sREAL	grndColPullUpAngle;

  /*
   * g-limit overrides
   */
  sREAL posGLimit;
  sREAL negGLimit;

  /*
   * marksmanship overrides
   */
  sREAL	bulletRadiusMin;
  sREAL	bulletRadiusMax;

  /*
   *  gunners skill overrides
   */
  sREAL gunnersPitchRate;
  sREAL gunnersYawRate;
  s_attitude gunnersJiggle;
  sREAL gunnersBulletRadiusMin;
  sREAL gunnersBulletRadiusMax;

} spilot_caps;

#ifdef __cplusplus

#include "smath.h"
#include "sobject.h"
#include "sarray.h"

class sPilotCaps : public sObject
{
public:
  /*
   *   id
   */
  char capsId[32];

  /*
   *  tactics overrides
   */
  sREAL targetRQCone;
  sREAL	targetFQCone;
  sREAL	aspectHeadOnCone;
  sREAL	aspectRearCone;
  sREAL	angleOffCone;
  sREAL	rearThreatDistance;
  sREAL rearThreatCone;     
  sREAL cancelRearThreat;
  sREAL	pursuitDistance;
  sREAL	pursuitCone;         
  sREAL	cancelPursuit;
  sREAL	turnThreshold;
  sREAL	altThreshold;
  sREAL	rangeThreshold;
  sREAL playerThreatDistance;
  sREAL playerThreatAspect;
  sREAL attackerCheckInterval;

  /*
   *  ground collision overrides
   */
  sREAL grndColSecs;
  sREAL	grndColAGL;
  sREAL	grndColRcvrAGL;
  sREAL	grndColPullUpAngle;

  /*
   * g-limit overrides
   */
  sREAL posGLimit;
  sREAL negGLimit;

  /*
   * marksmanship overrides
   */
  sREAL	bulletRadiusMin;
  sREAL	bulletRadiusMax;

  /*
   *  gunners skill overrides
   */
  sREAL gunnersPitchRate;
  sREAL gunnersYawRate;
  sAttitude gunnersJiggle;
  sREAL gunnersBulletRadiusMin;
  sREAL gunnersBulletRadiusMax;

  sPilotCaps()
    {}
  virtual ~sPilotCaps();

  sPilotCaps(const spilot_caps &);

  static sObjectArray sPilotCapsArray;
  static void AddsPilotCaps(sPilotCaps &);
  static void AddsPilotCaps(spilot_caps *);
  static sPilotCaps *GetsPilotCaps(const char *capsId);
  static void FlushsPilotCaps();
};
#endif

#endif

