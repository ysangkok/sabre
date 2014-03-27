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
 * File   : stact.C                              *
 * Date   : April, 1998                          *
 * Author : Dan Hammer                           *
 *************************************************/
#include <stdio.h>
#include <string.h>
#include "smath.h"
#include "spilcaps.h"
#include "stact.h"

/************************************************
 * sPilotTactics methods                        *
 ************************************************/

sPilotTactics::sPilotTactics()
{
  curTactic = ANGLES_FIGHT;
  pursuitType = LEAD_PURSUIT;
  targetRQCone = SPT_RQ_CONE;
  targetFQCone = SPT_FQ_CONE;
  aspectHeadOnCone = SPT_ASPECT_HEADON_CONE;
  aspectRearCone = SPT_ASPECT_REAR_CONE;
  angleOffCone = SPT_ANGLEOFF_CONE;
  rearThreatDistance = SPT_RTHREAT_DISTANCE;
  rearThreatCone = SPT_RTHREAT_CONE;
  cancelRearThreat = SPT_CANCEL_RTHREAT;
  pursuitDistance = SPT_PURSUIT_DISTANCE;
  pursuitCone = SPT_PURSUIT_CONE;
  cancelPursuit = SPT_CANCEL_PURSUIT;
  turnThreshold = SPT_TURN_THRESHOLD;
  altThreshold = SPT_ALT_THRESHOLD;
  rangeThreshold = SPT_RANGE_THRESHOLD;
  engageDistance = SPT_ENGAGE_DISTANCE;
  playerThreatDistance = SPT_PLAYER_THREAT_DISTANCE;
  playerThreatAspect = SPT_PLAYER_THREAT_ASPECT;
  attackerCheckInterval = SPT_ATTACKER_CHECK_INTERVAL;
  
  planeTacticalCaps[WEENIE_FIGHT] = 1;
  planeTacticalCaps[ENERGY_FIGHT] = 1;
  planeTacticalCaps[ANGLES_FIGHT] = 1;
  planeTacticalCaps[STRAFE]  = 1;
  planeTacticalCaps[ROCKET] = 1;
  planeTacticalCaps[DIVE_BOMB] = 0;
  planeTacticalCaps[TORPEDO_BOMB] = 0;
  planeTacticalCaps[LEVEL_BOMB] = 0;
}

int sPilotTactics::TacticAvail(int tactic)
{
int result = 0;
  if (tactic >= 0 && tactic < SPT_NTACTICS)
    result = planeTacticalCaps[tactic] != 0;
  return (result);
}

void sPilotTactics::SetPlaneCaps(const sPlaneCaps &caps)
{
  planeTacticalCaps[ENERGY_FIGHT] = caps.canDoEnergyFight;
  planeTacticalCaps[ANGLES_FIGHT] = caps.canDoAnglesFight;
  planeTacticalCaps[STRAFE] = caps.canStrafe;
  planeTacticalCaps[ROCKET] = caps.canRocket;
  planeTacticalCaps[DIVE_BOMB] = caps.canDiveBomb;
  planeTacticalCaps[TORPEDO_BOMB] = caps.canTorpedoBomb;
  planeTacticalCaps[LEVEL_BOMB] = caps.canLightBomb || caps.canHeavyBomb;
}

void sPilotTactics::SetPilotCaps(const sPilotCaps &pcaps)
{
  targetRQCone = pcaps.targetRQCone;
  targetFQCone = pcaps.targetFQCone;
  aspectHeadOnCone = pcaps.aspectHeadOnCone;
  aspectRearCone = pcaps.aspectRearCone;
  angleOffCone = pcaps.angleOffCone;
  rearThreatDistance = pcaps.rearThreatDistance;
  rearThreatCone = pcaps.rearThreatCone;     
  cancelRearThreat = pcaps.cancelRearThreat;
  pursuitDistance = pcaps.pursuitDistance;
  pursuitCone = pcaps.pursuitCone;         
  cancelPursuit = pcaps.cancelPursuit;
  turnThreshold = pcaps.turnThreshold;
  altThreshold = pcaps.altThreshold;
  rangeThreshold = pcaps.rangeThreshold;
  playerThreatDistance = pcaps.playerThreatDistance;
  playerThreatAspect = pcaps.playerThreatAspect;
  attackerCheckInterval = pcaps.attackerCheckInterval;
}


