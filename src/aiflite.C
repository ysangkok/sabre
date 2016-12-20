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
 * File   : aiflite.cpp                          *
 * Date   : April, 1998                          *
 * Author : Dan Hammer                           *
 *************************************************/
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include "sairfld.h"
#include "sfrmtn.h"
#include "stime.h"
#include "aiflite.h" 

#define aiF_DEFAULT_VE_RADIUS 22000.0
#define aiF_STRIKE_APPROACH_DISTANCE 24000.0

//#define INIT_FORMATION_LEN 250

extern "C" void sim_printf(char *, ...);

aiFlite::aiFlite()
  :pilots(3,0),
   waypoints(1,0),
   freePilots(3,0)
{
  max = 3;
  n = 0;
  viewPilot = 0;
  isPlayerFlite = 0;
  playerPilotWingPos = -1;
  idx = static_cast<uint32_t>(-1);
  affiliation = -1;
  id[0] = 0;
  activeCount = 1;
  leaderIndex = 0;
  mode = aiF_NAVIGATE;
  visualEngagementRadius = aiF_DEFAULT_VE_RADIUS;
  damageFlag = 0;
  newTargetDelayTime = 0.0;
  playerCommandMode = 0;
}

aiFlite::aiFlite(int mx, bool owns, const char *i)
  :pilots(mx,owns),
   waypoints(1,1),
   freePilots(mx,0)
{
  this->max = mx;
  n = 0;
  viewPilot = 0;
  isPlayerFlite = 0;
  playerPilotWingPos = -1;
  idx = static_cast<uint32_t>(-1);
  affiliation = -1;
  SetId(i);
  activeCount = 1;
  leaderIndex = 0;
  mode = aiF_NAVIGATE;
  curTask = swpNAVIGATE;
  visualEngagementRadius = aiF_DEFAULT_VE_RADIUS *
    aiF_DEFAULT_VE_RADIUS;
  attackerCount = 0;
  damageFlag = 0;
  newTargetDelayTime = 0.0;
  curWaypoint = NULL;
  playerCommandMode = 0;
}

void aiFlite::Init()
{
  int i;
  uint32_t formationIndex;
  sPoint	 leaderPosition;
  sAttitude leaderAttitude;

  activeCount = 0;
  leaderIndex = 0;
  damageFlag = 0;

  if (n == 0)
    return;

  for (i=0;i<n;i++)
    {
      aiPilot *pil = GetPilot(i);
      if (pil->IsPlayer())
	{
	  SetAsPlayerFlite(1);
	  playerPilotWingPos = i;
	}
      if (pil->IsActive())
	activeCount++;
      pil->SetFliteId(GetIdx());
      pil->markedAsFree = 0;
    }

  leader = GetLeader();
  leader->LOG("Assigned leader");
  SetAffiliation(leader->GetAffiliation());
  leader->GetPositionAndAttitude(leaderPosition,leaderAttitude);
  leaderAttitude.roll = 0.0;
  if (NWaypoints() > 0)
    {
      wpidx = 0;
      curWaypoint = GetWaypoint(wpidx);
      /*
       * start the pilots out in their
       proper formation positions
       */
      formationIndex = leader->GetIdx();
      /*
       *	if a takeoff waypoint, get airfield & runway
       * and put us there
       */
      if (curWaypoint->task == swpTAKEOFF)
	{
	  const char *fieldId = curWaypoint->GetFieldId();
	  int runwayNo = curWaypoint->GetRunwayNo();
	  sAirfield *airfield = sAirfield::GetsAirfield(fieldId);
	  if (airfield)
	    {
	      sRunway *runway = airfield->GetRunway(runwayNo);
	      if (runway)
		{
		  sPoint		positio;
		  sAttitude	attitude;
		  runway->GetTakeoffPositionAndAttitude(positio,attitude);
		  leader->SetPositionAndAttitude(positio,attitude);
		}
	    }
	}
      else
	{
	  sVector v;
	  sPoint p = leaderPosition;
	  p.z = curWaypoint->to.z;
	  v = curWaypoint->to - p;
	  sVector2sAttitude(v,leaderAttitude);
	  leader->SetPositionAndAttitude(leaderPosition,leaderAttitude);
	}
      int wPos = 1;
      for (i=0;i<n;i++)
	{
	  sPoint	 pos;
	  sAttitude att;
	  sPoint    formPoint;
	  sVector   formVect;

	  aiPilot *pil = GetPilot(i);
	  if (pil == leader)
	    continue;
	  pil->GetPositionAndAttitude(pos,att);
	  att.roll = 0.0;
	  sFormationData::GetWingmanVector(curWaypoint->formationType,
					   wPos++,
					   formVect);
	  formVect *= pil->GetFormationWingLen();
	  aiPilot::BodyPoint2WorldPoint(static_cast<int>(formationIndex),static_cast<const sPoint &>(formVect), // first cast can't fail on x64 cause it (uint32_t) is cast into a wider type (int)
					formPoint);
	  pil->SetPositionAndAttitude(formPoint,leaderAttitude);
	}
      LOG("aiFlite::Initing Formation");
      InitFormation(curWaypoint);
    }
}

void aiFlite::InitFormation(sWaypoint *wp)
{

  switch (wp->task)
    {
    case swpTAKEOFF:
      {
	for (int i=0;i<GetCount();i++)
	  {
	    aiPilot *pil = GetPilot(i);
	    if (pil->IsActive())
	      pil->SetTakeOff(wp);
	  }
	mode = aiF_TAKEOFF;
	nextPilot = 0;
      }
    break;

    case swpLAND:
      {
	for (int i=0;i<GetCount();i++)
	  {
	    aiPilot *pil = GetPilot(i);
	    if (pil->IsActive())
	      pil->SetLand(wp);
	  }
	mode = aiF_LAND;
	nextPilot = 0;
      }
    break;

    case swpMANEUVER:
      {
	for (int i=0;i<GetCount();i++)
	  {
	    aiPilot *pil = GetPilot(i);
	    if (pil->IsActive())
	      pil->SetManeuver(wp->GetManeuverType(),
			       static_cast<uint32_t>(wp->GetManeuverFlags()),
			       wp->GetManeuverData0(),
			       wp->GetManeuverData1(),
			       wp->GetManeuverData2());
	  }
	mode = aiF_MANEUVER;
      }
    break;

    default:
      {
	int wingPos = 1;
	uint32_t targetIdx;

	targetIdx = leader->GetIdx();
	if (wp->task == swpAIRSHOW)
	  leader->SetAirShowPoint(wp);
	else
	  leader->SetNavigatePoint(wp);
	leader->markedAsFree = 0;
	for (int i=0;i<GetCount();i++)
	  {
	    if (i != leaderIndex)
	      {
		aiPilot *pil = GetPilot(i);
		if (pil->IsActive())
		  {
		    pil->SetFormationTarget(targetIdx,wingPos++,
					    wp->formationType);
		    pil->markedAsFree = 0;
		  }
	      }
	  }
	mode = aiF_NAVIGATE;
	curTask = wp->task;
	if (curTask == swpTARGETS)
	  SetTargetMode(wp->iTaskModifier[0]);
      }
    break;
    }
  freePilots.Flush();
}

int aiFlite::AssignEngageTarget(aiPilot *pil, aiFlite *targetFlite)
{
  int i;
  int result = 0;

  for (i=0;i<targetFlite->GetCount();i++)
    {
      aiPilot *ppil = targetFlite->GetPilot(i);
      if (ppil->IsActive())
	{
	  pil->SetEngageTarget(ppil->GetIdx());
	  if (ppil->IsPlayer())
	    IssuePlayerAttackWarning(targetFlite,pil,ppil);
	  result = 1;
	  break;
	}
    }
  return result;
}

void aiFlite::KillAttackers()
{
  for (int i=0;i<GetCount();i++)
    {
      aiPilot *pil = GetPilot(i);
      pil->KillAttackList();
    }
}

void aiFlite::Update(double)
{
  int i;
  sAttitude att;
  aiPilot *newLeader;
  int foundPlayer = 0;

  attackerCount = 0;
  activeCount = 0;
  for (i=0;i<GetCount();i++)
    {
      aiPilot *pil = GetPilot(i);
      if (pil->IsActive())
	activeCount++;
      if (pil->HasAttackers())
	attackerCount++;
      if (pil->IsPlayer())
	{
	  foundPlayer = 1;
	  playerPilotWingPos = i;
	}
    }

  if (!IsActive())
    return;

  SetAsPlayerFlite(foundPlayer);

  newLeader = GetLeader();
  if (newLeader != leader)
    {
      leader = newLeader;
      leader->LOG("Assigned leader");
      if (mode != aiF_ENGAGE && mode != aiF_TARGETS)
	{
	  mode = aiF_NAVIGATE;
	  InitFormation(curWaypoint);
	}
    }

  leader->GetPositionAndAttitude(position,att);

  if (mode != aiF_NAVIGATE)
    {
      switch (mode)
	{
	case aiF_INTERCEPT:
	  DoInterceptUpdate();
	  break;

	case aiF_ENGAGE:
	  DoEngageUpdate();
	  break;

	case aiF_ENGAGE_PLAYER:
	  DoEngagePlayerUpdate();
	  break;

	case aiF_STRIKE:
	  DoStrikeUpdate();
	  break;

	case aiF_MANEUVER:
	  break;

	case aiF_TARGETS:
	  DoTargetsUpdate();
	  break;

	case aiF_TAKEOFF:
	  DoTakeOffUpdate();
	  break;

	case aiF_LAND:
	  DoLandUpdate();
	  break;
	}
    }
  else
    {
      switch (curTask)
	{
	case swpNAVIGATE:
	case swpAIRSHOW:
	  DoNavigateWaypoint();
	  break;

	case swpCAP:
	  DoCapWaypoint();
	  break;

	case swpINTERCEPT:
	  DoInterceptWaypoint();
	  break;

	case swpENGAGE_PLAYER:
	  DoEngagePlayerWaypoint();
	  break;

	case swpSTRAFE_ATTACK:
	case swpROCKET_ATTACK:
	case swpDIVE_BOMB:
	case swpTORPEDO_BOMB:            
	case swpLEVEL_BOMB:
	  DoStrikeWaypoint();
	  break;

	case swpTARGETS:
	  DoTargetsWaypoint();
	  break;
	}
    }
  if (HasAttackers() && mode == aiF_NAVIGATE && leader->HasGunners())
    DistributeAttackers();
  damageFlag = 0;
}

void aiFlite::SetAsPlayerFlite(bool isPlayer)
{
  isPlayerFlite = isPlayer;
  if (isPlayerFlite)
    playerFlite = this;
  else
    {
      if (playerFlite == this)
	playerFlite = NULL;
    }
}

void aiFlite::DistributeAttackers()
{
  int i;

  freePilots.Flush();
  for (i=0;i<GetCount();i++)
    {
      aiPilot *pil = GetPilot(i);
      if (pil->IsActive() && !pil->HasAttackers())
	freePilots.Add(pil);
    }

  if (attackerCount > 0 && freePilots.Count() > 0)
    {
      int j = 0;
      for (i=0;i<GetCount();i++)
	{
	  if (j >= freePilots.Count())
	    break;
	  aiPilot *pil = GetPilot(i);
	  if (pil->IsActive() && pil->HasAttackers())
	    {
	      aiPilot *pil0 = static_cast<aiPilot *>(freePilots[j++]);
	      sAttacker *attkr = pil->GetFirstAttacker();
	      while (attkr)
		{
		  pil0->SetGunnersTarget(attkr);
		  attkr = pil->GetNextAttacker();
		}
	    }
	}
    }
}

void aiFlite::Destroy()
{
  int i;
  KillAttackers();
  for (i=0;i<GetFliteCount();i++)
    {
      aiFlite *flite = GetFliteByIndex(i);
      if (flite && flite != this)
	flite->KillEngageFlite(this);
    }
  if (pilots.Owns())
    {
      for (i=0;i<n;i++)
	{
	  aiPilot *pil = GetPilot(i);
	  if (pil)
	    {
	      aiPilot::RemoveaiPilot(pil);
	      pil->Destroy();
	    }
	}
    }
  pilots.Flush();
}


void aiFlite::DoEngageUpdate()
{
  aiFlite *engageFlite;
  engageFlite = GetaiFlite(engageFliteIdx);
  if (engageFlite != NULL && engageFlite->IsActive())
    {
      if (newTargetDelayTime <= 0.0)
	{
	  for (int i=0;i<GetCount();i++)
	    {
	      aiPilot *pil = GetPilot(i);
	      if (pil->IsActive() &&
		  pil->GetEngageStatus() == 0)
		AssignEngageTarget(pil,engageFlite);
	    }
	}
      else
	HandleDelayedTargets(engageFlite);
    }
  else
    {
      mode = aiF_NAVIGATE;
      for (int i=0;i<GetCount();i++)
	{
	  aiPilot *pil = GetPilot(i);
	  pil->Disengage();
	}
      InitFormation(curWaypoint);		
    }
}

void aiFlite::HandleDelayedTargets(aiFlite *engageFlite)
{
  int i;

  for (i=0;i<GetCount();i++)
    {
      aiPilot *pil = GetPilot(i);
      if (!pil->markedAsFree && pil->IsActive() && pil->GetEngageStatus() == 0)
	{
	  pil->timerExtra = -pil->GetTimeFrame();	/* 'cause itll be incr in next loop */
	  pil->markedAsFree = 1;
	  freePilots.Add(pil);
	  /*
	    if (pil == leader)
	    pil->SetNavigatePoint(curWaypoint);
	    else
	    {
	    int leaderIdx;
	    leaderIdx = leader->GetIdx();
	    pil->SetFormationTarget(leaderIdx,pil->GetWingPosition(),curWaypoint->formationType);
	    }
	    */
	}
    }

  for (i=0;i<freePilots.Count();i++)
    {
      aiPilot *pil = static_cast<aiPilot *>(freePilots[i]);
      if (pil != NULL)
	{
	  pil->timerExtra += pil->GetTimeFrame();
	  if (pil->timerExtra >= newTargetDelayTime || pil->IsPlayer())
	    {
	      AssignEngageTarget(pil,engageFlite);
	      freePilots.Remove(i);
	      pil->markedAsFree = 0;
	    }
	}
    }
}

void aiFlite::DoTargetsUpdate()
{
  if (targetMode == aiF_TARGETMODE_NOACTION)
    {
      if (!leader->IsPlayer())
	{
	  sREAL dist = leader->GetNavigationDistance();
	  if (dist <= 1000.0)
	    {
	      curWaypoint = GetNextWaypoint();
	      leader->LOG("new waypoint");    
	      InitFormation(curWaypoint);
	    }
	}
    }
}

void aiFlite::KillEngageFlite(aiFlite *engageFlite)
{
  if (engageFlite->GetIdx() == engageFliteIdx)
    {
      mode = aiF_NAVIGATE;
      for (int i=0;i<GetCount();i++)
	{
	  aiPilot *pil = GetPilot(i);
	  pil->Disengage();
	}
      InitFormation(curWaypoint);		
    }
}

void aiFlite::DoInterceptUpdate()
{
  aiFlite *interceptFlite = GetaiFlite(interceptFliteIdx);
  if (interceptFlite && interceptFlite->IsActive())
    {
      if (!leader->GetInterceptStatus())
	{
	  if (!leader->GetTargetActive())
	    {
	      aiPilot *oppLeader = interceptFlite->leader;
	      leader->SetInterceptTarget(oppLeader->GetIdx());
	    }
	  else
	    {
	      EngageFlite(interceptFlite);
	      mode = aiF_ENGAGE;
	    }
	}
    }
  else
    {
      mode = aiF_NAVIGATE;
      InitFormation(curWaypoint);
    }
}

void aiFlite::DoEngagePlayerUpdate()
{
  
}

void aiFlite::DoStrikeUpdate()
{
  if (curTask == swpLEVEL_BOMB)
    {
      sAttitude att;
      sVector   v;
      sREAL     d;
      if (leader->IsBombing())
	{
	  for (int i=0;i<GetCount();i++)
	    {
	      aiPilot *pil = GetPilot(i);
	      if (pil != leader && pil->IsActive())
		pil->DropBomb();
	    }    
	}
      leader->GetAttitude(curWaypoint->to,v,att);
      d = v.Magnitude();
      if ((d >= 6000.0) && (fabs(att.yaw) > (Pi - Pi_4)))
	{
	  curWaypoint = GetNextWaypoint();
	  InitFormation(curWaypoint);
	}
    }
}


void aiFlite::DoTakeOffUpdate()
{
  if (curWaypoint != NULL &&	sGetElapsedTime() < curWaypoint->time)
    {
      for (int i=0;i<GetCount();i++)
	GetPilot(i)->SetClearForTakeoff(0);
      return;
    }
  else if (nextPilot < GetCount())
    {
      aiPilot *pil = GetPilot(nextPilot);
      pil->SetClearForTakeoff(1);
      if (pil->GetAirSpeedFPS() > sMPH2FPS(30.0))
	nextPilot++;
    }

  if (nextPilot >= GetCount())
    {
      int nextWaypoint = 1;
      for (int i=0;i<GetCount();i++)
	{
	  aiPilot *pil = GetPilot(i);
	  if (pil->IsActive() && pil->GetAltitudeFPS() < curWaypoint->altitude - 200.0)
	    {
	      nextWaypoint = 0;
	      break;
	    }
	}
      if (nextWaypoint)
	{	
	  curWaypoint = GetNextWaypoint();
	  InitFormation(curWaypoint);
	}
    }
}

void aiFlite::DoLandUpdate()
{
  if (curWaypoint != NULL &&	sGetElapsedTime() < curWaypoint->time)
    {
      for (int i=0;i<GetCount();i++)
	GetPilot(i)->SetClearForLanding(0);
      return;
    }
  else if (nextPilot < GetCount())
    {
      aiPilot *pil = GetPilot(nextPilot);
      pil->SetClearForLanding(1);
      if (pil->GetHeightAGL() <= 500.0)
	nextPilot++;
    }
}

void aiFlite::DoNavigateWaypoint()
{
  if (!leader->IsPlayer())
    {
      sREAL dist = leader->GetNavigationDistance();
      if (dist <= 1000.0)
	{
	  curWaypoint = GetNextWaypoint();
	  leader->LOG("new waypoint");    
	  InitFormation(curWaypoint);
	}
    }
}

void aiFlite::DoTargetsWaypoint()
{

  if (!leader->IsPlayer())
    {
      sREAL dist = leader->GetNavigationDistance();
      if (dist <= 1000.0)
	{
	  curWaypoint = GetNextWaypoint();
	  leader->LOG("new waypoint");    
	  InitFormation(curWaypoint);
	  return;
	}
    }

  if (damageFlag != 0)
    {
      aiFlite *damagerFlite = NULL;
      aiPilot *damagerPilot = NULL;
      aiPilot *broadcastPilot = NULL;

      damagerPilot = aiPilot::GetaiPilot(damagerIdx);
      if (damagerPilot)
	damagerFlite = GetaiFlite(damagerPilot->GetFliteId());
	
      if (damagerPilot && !damagerPilot->IsPlayer())
	broadcastPilot = damagerPilot;
      else if (damagerFlite)
	{
	  for (int i=0;i<damagerFlite->GetCount();i++)
	    {
	      aiPilot *pil = damagerFlite->GetPilot(i);
	      if (pil && !pil->IsPlayer() && pil->IsActive())
		{
		  broadcastPilot = pil;
		  break;
		}	
	    }
	}
		
      if (broadcastPilot)
	{
	  switch (targetMode)
	    {
	    case aiF_TARGETMODE_NOACTION:
	      broadcastPilot->Broadcast(commWINGMAN_GUYS_ASLEEP);
	      break;

	    case aiF_TARGETMODE_EVADE:
	      broadcastPilot->Broadcast(commWINGMAN_GUYS_NO_AMMO);
	      break;

	    case aiF_TARGETMODE_ENGAGE:
	      broadcastPilot->Broadcast(commWINGMAN_GUYS_FIGHTING);
	      break;
	    }
	}

      switch(targetMode)
	{
	case aiF_TARGETMODE_NOACTION:
	  mode = aiF_TARGETS;
	  break;
	
	case aiF_TARGETMODE_EVADE:
	  {
	    mode = aiF_TARGETS;
	    for (int i =0;i<GetCount();i++)
	      {
		aiPilot *pil = GetPilot(i);
		if (pil->IsActive())
		  pil->SetEvadeTarget(damagerIdx);
	      }
	    break;
	  }

	case aiF_TARGETMODE_ENGAGE:
	  if (damagerFlite)
	    EngageFlite(damagerFlite);
	  break;
	}

    }
}

void aiFlite::DoCapWaypoint()
{
  int count = 0;
  int choice;
  aiFlite *engageCandidates[aiMAX_FLITES];
  //sREAL  engageRanges[aiMAX_FLITES];


  /****************************************************
   * if player is in charge, don't automatically
   *	engage
   ****************************************************
   if (leader->IsPlayer() && GetPlayerCommandMode())
   return;
   */

  int i;
  for (i=0;i<GetFliteCount();i++)
    {
      aiFlite *flite = GetFliteByIndex(i);
      if (flite != NULL && flite->GetAffiliation() != GetAffiliation() 
	  && flite->IsActive())
	{
	  sPoint ourPos;
	  sPoint itsPos;

	  ourPos = GetPosition();
	  itsPos = flite->GetPosition();
	  ourPos.z = itsPos.z = 0.0;
	  sREAL d = sDistanceSquared(ourPos,itsPos);
	  if (d <= visualEngagementRadius)
	    {
	      engageCandidates[count] = flite;
	      //engageRanges[count] = d;
	      count++;
	    }
	}
    }

  if (count > 0)
    {
      if (count == 1)
	EngageFlite(engageCandidates[0]);
      else
	{
	  choice = sRand(0,count-1);
	  EngageFlite(engageCandidates[choice]);
	}
    }
}

void aiFlite::DoInterceptWaypoint()
{
  /*
   *  waypoint should contain id of flite
   *  to intercept
   */
  aiFlite *interceptFlite = GetaiFlite(curWaypoint->cTaskModifier);
  if (interceptFlite && interceptFlite->IsActive())
    {
      interceptFliteIdx = interceptFlite->GetIdx();
      uint32_t targetIdx;
      aiPilot *oppLeader = interceptFlite->leader;
      targetIdx = oppLeader->GetIdx();
      leader->SetInterceptTarget(targetIdx);
      mode = aiF_INTERCEPT;
    }
}

void aiFlite::DoStrikeWaypoint()
{
  sREAL dist;

  dist = leader->GetNavigationDistance();
  if (dist <= aiF_STRIKE_APPROACH_DISTANCE)
    {
      mode = aiF_STRIKE;
      switch (curTask)
	{
        case swpLEVEL_BOMB:
        default:
	  leader->SetSurfaceTarget(static_cast<uint32_t>(curWaypoint->iTaskModifier[0]),
				   aiPilot::LEVEL_BOMB,
				   &curWaypoint->to);
	  break;
	}
    }
}

void aiFlite::DoEngagePlayerWaypoint()
{
  sTargetGeometry tg;
  if (aiPilot::playerPilot != NULL)
    {
      leader->GetQuickTargetGeometry(aiPilot::playerPilot->GetIdx(),tg);
      if (tg.range * tg.range <= visualEngagementRadius)
	{
	  mode = aiF_ENGAGE_PLAYER;
	  for (int i=0;i<GetCount();i++)
	    {
	      aiPilot *pil = GetPilot(i);
	      if (pil->IsActive())
		pil->SetEngageTarget(aiPilot::playerPilot->GetIdx());
	    }
	}
    }
}

aiPilot *aiFlite::GetPilot(int idx)
{
  aiPilot *result = NULL;
  if (idx >= 0 && idx < n)
    {
      result = static_cast<aiPilot *>(pilots[idx]);
    }
  return (result);
}

aiPilot *aiFlite::GetPlayerWingman()
{
  aiPilot *result = NULL;
  if (IsPlayerFlite())
    {
      for (int i=0;i<GetCount();i++)
	{
	  aiPilot *pil = GetPilot(i);
	  if (!pil->IsPlayer() && pil->IsActive())
	    {
	      result = pil;
	      break;
	    }
	}	
    }
  return (result);
}

int aiFlite::Add(aiPilot *newPilot)
{
  if (n < max)
    {   
      pilots.Add(newPilot);
      n++;
    }
  return (n);
}

/* Add a pilot which stands in for the player */
void aiFlite::AddPlayerPilot(aiPilot *playerPilot)
{
  /* should only be called once */
  if (n < max)
    {
      playerPilot->SetAsPlayer(1);
      playerPilot->SetAffiliation(affiliation);
      aiPilot::AddaiPilot(playerPilot);
      playerPilot->Init();
      isPlayerFlite = 1;
      pilots.Add(playerPilot);
      playerPilotWingPos = n;
      n++;
    }
}

void aiFlite::SetWaypoints(const sWaypoint *wps, int nn)
{
  waypoints.Destroy();
  waypoints.Create(nn,1);
  for (int i=0;i<nn;i++)
    {
      sWaypoint *wp = new sWaypoint(wps[i]);
      waypoints.Add(wp);
      wp->next = NULL;
      if (i > 0 && i < nn - 1)
	GetWaypoint(i-1)->next = wp;
    }
}

void aiFlite::SetWaypoints(const swaypoint_info *winfo, int nn)
{
  waypoints.Destroy();
  waypoints.Create(nn,1);
  for (int i=0;i<nn;i++)
    {
      sWaypoint *wp = new sWaypoint(winfo[i]);
      waypoints.Add(wp);
      wp->next = NULL;
      if (i > 0 && i < nn - 1)
	GetWaypoint(i-1)->next = wp;
    }
}

sWaypoint *aiFlite::GetWaypoint(int idx)
{
  if (idx >= 0 && idx < waypoints.Count())
    return (static_cast<sWaypoint *>(waypoints[idx]));
  else
    return (NULL);
}

sWaypoint *aiFlite::GetNextWaypoint()
{
  wpidx++;
  if (wpidx >= waypoints.Count())
    wpidx = 0;
  return (GetWaypoint(wpidx));  
}

sWaypoint *aiFlite::GetPreviousWaypoint()
{
  wpidx--;
  if (wpidx < 0)
    wpidx = 0;
  return (GetWaypoint(wpidx));
}

int aiFlite::IncViewPilot()
{	
  if (!IsActive())
    return (0);
  viewPilot++;
  if (viewPilot >= n)
    viewPilot = 0;
  return (viewPilot);
}

void aiFlite::SetManeuver(int maneuver, uint32_t flags, sREAL d0,
			  sREAL d1, sREAL d2)
{
  for (int i=0;i<GetCount();i++)
    {
      aiPilot *pil = GetPilot(i);
      pil->SetManeuver(maneuver,flags,d0,d1,d2);
    }

  mode = aiF_MANEUVER;
}

void aiFlite::SetFormationWaypoint(sWaypoint *wp, int leaderIdx)
{
  uint32_t targetIdx;
  SetWaypoints(wp,1);
  if (leaderIndex < 0)
    leaderIndex = 0;
  if (leaderIndex > GetCount() - 1)
    leaderIndex = GetCount() - 1;
  this->leaderIndex = leaderIdx;
  aiPilot *ldr = GetPilot(leaderIndex);
  targetIdx = ldr->GetIdx();
  ldr->SetNavigatePoint(GetWaypoint(0));
  int wingPos = 1;
  for (int i=0;i<GetCount();i++)
    {
      if (i != leaderIndex)
	{
	  aiPilot *pil = GetPilot(i);
	  pil->SetFormationTarget(targetIdx,wingPos++,
				  wp->formationType);
	}
    }
}

aiPilot *aiFlite::GetLeader()
{
  if (!IsActive())
    return (NULL);
  else
    {
      leaderIndex = 0;
      return GetNextActivePilot(leaderIndex);
    }
}

aiPilot *aiFlite::GetNextActivePilot(int  & start)
{
  aiPilot *result = NULL;
  if (start > GetCount() - 1)
    start = 0;
  if (IsActive())
    {
      /* avoid endless loop */
      int check = 0;
      aiPilot *pil = GetPilot(start);
      while (!pil->IsActive())
	{
	  start++;
	  check++;
	  if (check > GetCount())
	    {
	      pil = NULL;
	      break;
	    }
	  if (start > GetCount() - 1)
	    start = 0;
	  pil = GetPilot(start);
	}
      result = pil;
    }
  return (result);
}

void aiFlite::EngageFlite(aiFlite *flite)
{
  if (!flite->IsActive())
    return;
  engageFliteIdx = flite->GetIdx();
  mode = aiF_ENGAGE;
  int ii = 0;
  for (int i = 0;i<GetCount();i++)
    {
      aiPilot *pil = GetPilot(i);
      if (pil->IsActive() && !pil->IsPlayer())
	{
	  aiPilot *ppil = flite->GetNextActivePilot(ii);
	  if (ppil)
	    {	
	      pil->SetEngageTarget(ppil->GetIdx());
	      if (ppil->IsPlayer())
		IssuePlayerAttackWarning(flite,pil,ppil);
	    }
	  ii++;
	}
    }
}

/*****************************************************************
 * player issued commands
 *****************************************************************/
void aiFlite::EngageNearestFlite()
{
  uint32_t	idx = 0;	
  sREAL	minDistance = visualEngagementRadius + static_cast<sREAL>(100.0);
  int foundOne = 0;

  for (int i=0;i<GetFliteCount();i++)
    {
      aiFlite *flite = GetFliteByIndex(i);
      if (flite != NULL && flite->GetAffiliation() != GetAffiliation() 
	  && flite->IsActive())
	{
	  sPoint ourPos;
	  sPoint itsPos;

	  ourPos = GetPosition();
	  itsPos = flite->GetPosition();
	  ourPos.z = itsPos.z = 0.0;
	  sREAL d = sDistanceSquared(ourPos,itsPos);
	  if (d <= visualEngagementRadius)
	    {
	      foundOne = 1;
	      if (d < minDistance)
		{
		  minDistance = d;
		  idx = flite->GetIdx();
		}
	    }
	}
    }

  if (foundOne)
    {
      aiFlite *flite = GetaiFlite(idx);
      if (flite)
	EngageFlite(flite);
    }

}

void aiFlite::FormUp()
{
  for (int i=0;i<GetCount();i++)
    {
      aiPilot *pil = GetPilot(i);
      pil->Disengage();
    }
  InitFormation(curWaypoint);
}

void aiFlite::ProtectPlayer()
{
  if (!IsActive())
    return;
  if (aiPilot::playerPilot != NULL)
    {
      if (IsPlayerFlite())
	{
	  aiPilot *wingmanPilot = GetPlayerWingman();
	  if (!wingmanPilot)
	    {
	      aiPilot::TextMessageToPlayer("No Wingmen");
	      return;
	    }
	  int pilIndex = 0;
	  sAttacker *attkr = aiPilot::playerPilot->GetFirstAttacker();
	  if (!attkr)
	    wingmanPilot->Broadcast(commWINGMAN_NOBODY_ATTACK);
	  while (attkr)
	    {
	      aiPilot *pil = GetPilot(pilIndex++);
	      if (pil != aiPilot::playerPilot)
		{
		  pil->Disengage();
		  pil->SetEngageTarget(attkr->GetIdx());
		  attkr = aiPilot::playerPilot->GetNextAttacker();
		}
	      if (pilIndex >= GetCount())
		break;
	    }
	}
      else
	{
	  int pilIndex = 0;
	  sAttacker *attkr = aiPilot::playerPilot->GetFirstAttacker();
	  while (attkr)
	    {
	      aiPilot *pil = GetPilot(pilIndex++);
	      if (pil != aiPilot::playerPilot)
		{
		  pil->Disengage();
		  pil->SetEngageTarget(attkr->GetIdx());
		  attkr = aiPilot::playerPilot->GetNextAttacker();
		}
	      if (pilIndex >= GetCount())
		break;
	    }
	}
    }
}

void aiFlite::NextWaypoint()
{
  curWaypoint = GetNextWaypoint();
  InitFormation(curWaypoint);
}

void aiFlite::PreviousWaypoint()
{
  curWaypoint = GetPreviousWaypoint();
  InitFormation(curWaypoint);
}

static const char *oClocks[] = 
{
  "12:00",
  "1:00",
  "2:00",
  "3:00",
  "4:00",
  "5:00",
  "6:00",
  "7:00",
  "8:00",
  "9:00",
  "10:00",
  "11:00"
};
void aiFlite::IssuePlayerAttackWarning(aiFlite *plyrFlit, aiPilot *attackerPilot,
				       aiPilot *playerPilot)
{
  sTargetGeometry geometry;
  sPoint			playerPosition;
  sAttitude		playerAttitude;
  int				oClock;
  sREAL				convYaw;
  const char				*hiLo;
  char				distance[32];
  sREAL				altDiff;
  static char    warningStr[256];

  /*
   * shouldn't be but God knows ...
   */
  if (attackerPilot->IsPlayer())
    return;
	
  aiPilot *wingmanPilot = plyrFlit->GetPlayerWingman();
  if (wingmanPilot)
    {
      playerPilot->GetQuickTargetGeometry(attackerPilot->GetIdx(),geometry);
      playerPilot->GetPositionAndAttitude(playerPosition,playerAttitude);
      sprintf(distance,"%5.1f",geometry.range / 300.0);
      altDiff = geometry.worldPosition.z - playerPosition.z;
      if (altDiff > 0 && fabs(altDiff) > 400.0)
	hiLo = ", high";
      else if (altDiff < 0 && fabs(altDiff) > 400.0)
	hiLo = ", low";
      else
	hiLo = "";
      convYaw = ConvertAngle(geometry.relAttitude.yaw);
      oClock = static_cast<int>((convYaw * 12.0) / _2Pi);
      if (oClock > 11)
	oClock = 11;
      sprintf(warningStr,"Bandit at %s, range %s %s",
	      oClocks[oClock],
	      distance,
	      hiLo);
      wingmanPilot->Broadcast(commWINGMAN_BANDIT,
			      commCHANNEL_AFFILIATION,
			      commPRIORITY_HIGH,
			      warningStr);
    }
}

/*******************************************
 * static aiFlite members                   *
 *******************************************/
sObjectArray aiFlite::aiFlites(aiMAX_FLITES,aiFLITE_DEFAULT_OWNERSHIP);
uint32_t aiFlite::nextIdx = 0L;
aiFlite *aiFlite::playerFlite = NULL;

void aiFlite::AddaiFlite(aiFlite *flite)
{
  aiFlites.Add(flite);
  flite->SetIdx(nextIdx++);
  if (flite->IsPlayerFlite())
    playerFlite = flite;
}

aiFlite *aiFlite::GetaiFlite(uint32_t idx)
{
  aiFlite *result = NULL;
  for (int i=0;i<aiFlites.Count();i++)
    {
      aiFlite *flite = static_cast<aiFlite *>(aiFlites[i]);
      if (flite != NULL && flite->GetIdx() == idx)
	{
	  result = flite;
	  break;
	}
    }
  return result;
}

aiFlite *aiFlite::GetaiFlite(char *id)
{
  aiFlite *result = NULL;
  for (int i=0;i<GetFliteCount();i++)
    {
      aiFlite *fl = GetFliteByIndex(i);
      if (fl && !strcmp(id,fl->GetId()))
	{
	  result = fl;
	  break;
	}
    }
  return result;
}

aiFlite *aiFlite::GetFliteByIndex(int i)
{
  if (i >= 0 && i < aiFlites.Count())
    return static_cast<aiFlite *>(aiFlites[i]);
  else
    return NULL;
}

aiFlite *aiFlite::GetPlayerFlite()
{
  return playerFlite;
}

int aiFlite::GetFliteCount()
{
  return aiFlites.Count();
}

void aiFlite::FlushaiFlites()
{
  aiFlites.Flush();
  playerFlite = NULL;
}

void aiFlite::RemoveaiFlite(aiFlite *flite)
{
  for (int i=0;i<aiFlites.Count();i++)
    {
      aiFlite *fflite = static_cast<aiFlite *>(aiFlites[i]);
      if (fflite != NULL && fflite == flite)
	aiFlites.Remove(i);
    }
}

void aiFlite::aiFlitesUpdate(double timeFrame)
{
  for (int i=0;i<aiFlites.Count();i++)
    {
      aiFlite *flite = static_cast<aiFlite *>(aiFlites[i]);
      if (flite != NULL)
	flite->Update(timeFrame);
    }
}

