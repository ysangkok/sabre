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
 * File   : spilcaps.cpp                         *
 * Date   : Apirl, 1998                          *
 * Author : Dan Hammer                           *
 *************************************************/
#include <stdio.h>
#include <string.h>

#include "spilcaps.h"

sObjectArray sPilotCaps::sPilotCapsArray(sMAXPILOTCAPS,1);

void sPilotCaps::AddsPilotCaps(sPilotCaps &pilotCaps)
{
	sPilotCapsArray.Add(&pilotCaps);
}

void sPilotCaps::AddsPilotCaps(spilot_caps *caps)
{
    if (sPilotCapsArray.Count() < sMAXPILOTCAPS)
    {
       sPilotCaps *pilotCaps = new sPilotCaps(*caps);
		 AddsPilotCaps(*pilotCaps);
    }
}

sPilotCaps *sPilotCaps::GetsPilotCaps(const char *capsId)
{
    sPilotCaps *result = NULL;
    for (int i=0;i<sPilotCapsArray.Count();i++)
    {
      sPilotCaps *pilotCaps = (sPilotCaps *)sPilotCapsArray[i];
      if (!strcmp(pilotCaps->capsId,capsId))
      {
        result = pilotCaps;
        break;
      }
    }
   return (result);
} 

void sPilotCaps::FlushsPilotCaps()
{
	sPilotCapsArray.Flush();
}

sPilotCaps::sPilotCaps(const spilot_caps &caps)
{
    strcpy(capsId,caps.capsId);

    targetRQCone =	caps.targetRQCone ;
    targetFQCone =	caps.targetFQCone ;
    aspectHeadOnCone =	caps.aspectHeadOnCone ;
    aspectRearCone =	caps.aspectRearCone ;
    angleOffCone =	caps.angleOffCone ;
    rearThreatDistance =	caps.rearThreatDistance ;
    rearThreatCone =	caps.rearThreatCone ;
    cancelRearThreat =	caps.cancelRearThreat ;
    pursuitDistance =	caps.pursuitDistance ;
    pursuitCone =	caps.pursuitCone ;
    cancelPursuit =	caps.cancelPursuit ;
    turnThreshold =	caps.turnThreshold ;
    altThreshold =	caps.altThreshold ;
    rangeThreshold =	caps.rangeThreshold ;
    grndColSecs        =	caps.grndColSecs        ;
    grndColAGL         =	caps.grndColAGL         ;
    grndColRcvrAGL     =	caps.grndColRcvrAGL     ;
    grndColPullUpAngle =	caps.grndColPullUpAngle ;
    posGLimit =	caps.posGLimit ;
    negGLimit =	caps.negGLimit ;
    playerThreatDistance = caps.playerThreatDistance;
    playerThreatAspect = caps.playerThreatAspect;
    attackerCheckInterval = caps.attackerCheckInterval;
    bulletRadiusMin = caps.bulletRadiusMin;
	 bulletRadiusMax = caps.bulletRadiusMax;

    gunnersPitchRate = caps.gunnersPitchRate;
    gunnersYawRate = caps.gunnersYawRate;
    gunnersJiggle.pitch = caps.gunnersJiggle.pitch;
    gunnersJiggle.yaw = caps.gunnersJiggle.yaw;
    gunnersBulletRadiusMin = caps.gunnersBulletRadiusMin;
	 gunnersBulletRadiusMax = caps.gunnersBulletRadiusMax;
}
