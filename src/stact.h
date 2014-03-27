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
 * File   : stact.h                              *
 * Date   : April, 1998                          *
 * Author : Dan Hammer                           *
 * Tactic definitions                            *
 *************************************************/
#ifndef __stact_h
#define __stact_h

#define SPT_RQ_CONE             Pi_4
#define SPT_FQ_CONE             Pi_4
#define SPT_ASPECT_HEADON_CONE  Pi_4  
#define SPT_ASPECT_REAR_CONE    Pi_4      
#define SPT_ANGLEOFF_CONE       (_degree * 5.0)

#define SPT_RTHREAT_DISTANCE    1800
#define SPT_RTHREAT_CONE        Pi_4
#define SPT_CANCEL_RTHREAT      0.8

#define SPT_PURSUIT_DISTANCE        6200
#define SPT_PURSUIT_CONE            (_degree * 60.0)
#define SPT_CANCEL_PURSUIT          0.8
#define SPT_WEAPON_THREAT_DISTANCE  2000 
#define SPT_TURN_THRESHOLD          (_degree * 4.0)
#define SPT_ALT_THRESHOLD           600
#define SPT_RANGE_THRESHOLD         10
#define SPT_HARDTURNALT_THRESHOLD   -180
#define SPT_HARDTURNSPEED_THRESHOLD -100
#define SPT_ENGAGE_DISTANCE         24000.0
#define SPT_PLAYER_THREAT_DISTANCE  3000.0
#define SPT_PLAYER_THREAT_ASPECT    (_degree * 12.0)
#define SPT_ATTACKER_CHECK_INTERVAL 12.0

#ifdef __cplusplus

#include "sobject.h"
#include "sarray.h"
#include "smath.h"
#include "splncaps.h"
#include "spilcaps.h"

#define SPT_NTACTICS   8
 
class sPlaneSpecs;
class sPilotCaps;

class sPilotTactics
{
public:
  enum { WEENIE_FIGHT, ANGLES_FIGHT, ENERGY_FIGHT, STRAFE, ROCKET, DIVE_BOMB, TORPEDO_BOMB,
			LEVEL_BOMB };

  enum { LAG_PURSUIT, LEAD_PURSUIT, PURE_PURSUIT };

  int   curTactic;                       /* one of WEENIE_FIGHT ... HEAVY_BOMB                          */
  int   planeTacticalCaps[SPT_NTACTICS]; /* flags indicating which tactics we are capable of            */
  int   pursuitType;                     /* one of LAG_PURSUIT ... PURE_PURSUIT                         */
  sREAL	targetRQCone;                    /* what constitutes a rear-quarter target                      */
  sREAL targetFQCone;	                 /* what constitutes a target "on the nose"                     */
								
  sREAL aspectHeadOnCone;                /* what constitutes a headon aspect to target                  */
  sREAL	aspectRearCone;                  /* what constitutes a rear aspect to target                    */
  sREAL	angleOffCone;                    /* what constitutes high/low angle off                         */

  sREAL	rearThreatDistance;              /* what constitutes a rear threat                              */	
  sREAL	rearThreatCone;     
  sREAL	cancelRearThreat;

  sREAL engageDistance;                  /* max distance for engage                                     */

  sREAL pursuitDistance;                 /* pursuit stage parameters                                    */
  sREAL pursuitCone;                              
  sREAL cancelPursuit;

  sREAL	turnThreshold;                   /* minimum turn rate to consitute a "turning" target            */
  sREAL	altThreshold;                    /* minimum altitude difference to constitute a high/low target  */
  sREAL altThresholdVery;                /* as above, for a "very" high/low target                       */
  sREAL	rangeThreshold;			 /* minimum range difference to constitute closing/opening range */

  sREAL playerThreatDistance;            /* max distance for considering player a threat                 */
  sREAL playerThreatAspect;              /* aspect cone for considering player a threat                  */
  sREAL attackerCheckInterval;           /* how often to check for attackers                             */

  sPilotTactics();
  int TacticAvail(int tactic);
  void SetPlaneCaps(const sPlaneCaps &); /* get tactical stuff from plane caps                         */
  void SetPilotCaps(const sPilotCaps &); /* get tactical stuff from pilot caps                           */
};


#endif /* __cplusplus */
#endif
