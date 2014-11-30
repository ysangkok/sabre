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
 * File   : aipilot.cpp                          *
 * Date   : April, 1998                          *
 * Author : Dan Hammer                           *
 * aiPilot class definition                      *
 *************************************************/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include "aipilot.h"
#include "aipildef.h"

extern "C" void __cdecl sim_printf(char *, ...);

/*******************************************
 * aiPilot members                          *
 *******************************************/
/*
 * constructor with flight model instance
 */
aiPilot::aiPilot(sFlightModel *flightMdl)
						:gunners(aiPILOT_MAX_GUNNERS)
{
	isPlayerFlag = 0;
	timeFrame = elapsedTime = 0.0;
	SetFlightModel(flightMdl);
	task = NOTHING;
	remoteControl = 0;
	curTarget = &engageTarget;
	dead = ejected = 0;
	SetCapsId("DEFAULT");
	hasGunners = 0;
	flightModelUpdateFlag = 1;
}

aiPilot::~aiPilot()
{

}

/*
 * Initialization
 */
void aiPilot::Init()
{
	aiBase::Init();

	for (int i=0;i<8;i++)
		timers[i] = 0.0;
	posGLimit = posStructGLimit = aiPILOT_GLIMIT;
	negGLimit = negStructGLimit = aiPILOT_GLIMIT_NEG;
	rollLock = 0;
	pitchLock = 0;
	yawLock = 0;
	rollLimit = aiPILOT_ROLL_LIMIT;
	lastAirSpeed = lastAltitude = 0.0;
	for (int i=0;i<aiPILOT_AVERAGING_N;i++)
		dAirSpeed[i] = dAltitude[i] = 0.0;
	dAltitudeIdx = 0;
	dAirSpeedIdx = 0;
	formationWingLen = DEFLT_FORMATION_WINGLEN;
	engagementMinSpeed = sMPH2FPS(DEFLT_ENGAGEMENT_MIN_SPEED);
   SetDeath(0);
   SetEjected(0);

	grndColSecs        = aiPILOT_GCOL_SECS;
	grndColAGL         = aiPILOT_GCOL_AGL;
	grndColRcvrAGL     = aiPILOT_GCOL_RAGL;
	grndColPullUpAngle = aiPILOT_GCOL_ANGLE;

	collisionAvoidSecs	= aiPILOT_COLLISION_AVOID_SECS;
	immelmanGs				= aiPILOT_IMMELMAN_GS;
	immelmanAltMin			= aiPILOT_IMMELMAN_ALT_MIN;
	noseOnRollLimit		= aiPILOT_NOSEON_ROLL_LIMIT;
	speedLossLimit			= aiPILOT_SPEED_LOSS_LIMIT;
	splitSGs					= aiPILOT_SPLIT_S_GS;
	splitSAltMin			= aiPILOT_SPLIT_S_ALT_MIN;
	zoomAngle				= aiPILOT_ZOOM_ANGLE;
	
	extAngle = 8.0;
	formationOffset    = aiPILOT_FORMATION_OFFSET;
	rollMult = aiPILOT_ROLL_MULT;
	extendAngle = aiPILOT_EXTEND_ANGLE;
	gunAttackDistanceMul = aiPILOT_GUNATTACK_DISTANCE_MUL;
	gunAttackDistance = weaponLimits.maxRange * gunAttackDistanceMul;
	playerThreatDistance = aiPILOT_PLAYERTHREAT_DISTANCE;
	playerThreatAspect = aiPILOT_PLAYERTHREAT_ASPECT;
	playerThreatValue = aiPILOT_PLAYERTHREAT_VALUE;
	attackerCheckInterval = aiPILOT_ATTKRCHECK_INTERVAL;
	engageMaxSpeedMul = aiPILOT_ENGAGE_MAX_SPEED_MUL;
	maxRollRate = Pi_4;
	minRollRate = 0.0;
	bulletRadiusMin = aiPILOT_BULLET_RADIUS_MIN;
	bulletRadiusMax = aiPILOT_BULLET_RADIUS_MAX;

	standardTurnSpeedMPH = aiPILOT_CIRCLE_SPEED_MPH;
	finalApproachDistance = aiPILOT_FINAL_APPROACH_DISTANCE;
	finalApproachAGL = aiPILOT_FINAL_APPROACH_AGL;

	gunnersPitchRate = aiGUN_PITCH_RATE;
	gunnersYawRate = aiGUN_YAW_RATE;
	gunnersBulletRadius = aiPILOT_GUNNER_BULLET_RADIUS;
	gunnersJiggle.pitch = gunnersJiggle.yaw = aiPILOT_GUNNER_JIGGLE;

	airSpeedPid.p = aiPILOT_SPEED_P;
	airSpeedPid.i = aiPILOT_SPEED_I;
	airSpeedPid.d = aiPILOT_SPEED_D;

	targetClosurePid.p = aiPILOT_TC_P;
	targetClosurePid.i = aiPILOT_TC_I;
	targetClosurePid.d = aiPILOT_TC_D;

	CheckPlaneCaps(flightModel->GetModel());
	CheckPilotCaps(GetCapsId());

	/*
	 *	calc bullet radius
	 */
	bulletRadius = C(bulletRadiusMin + skillLevels[aiPILOT_SKILL_SHOOT] * 0.01 * bulletRadiusMax);
	playerInAttackList = 0;
	isBombing = 0;
	weaponLimits.Init();
	BuildGunners();
	currentAttkrPrime = 0;

	LOG("%s",flightModel->GetModel());
	LOG("Bullet radius: %2.1f",bulletRadius);
	LOG("Max Speed  : %5.1f",sFPS2MPH(flightModel->GetMaxSpeedFPS()));
	LOG("Stall Speed: %5.1f",sFPS2MPH(flightModel->GetStallSpeedFPS()));
	
}

// Update routine. 
void aiPilot::Update(double timeFrame)
{
	aiBase::Update(timeFrame);

	// If we're dead or ejected, just let the flight model
	// do it's thing
	if (IsDead() || IsEjected())
	{
		flightModel->SetUpdateCallback(0,NULL,NULL);
		flightModel->SetEngineControlPer(0.0);
		dbg = IsDead() ? "dead" : "eject" ;
		if (flightModelUpdateFlag)
			flightModel->Update(timeFrame);
		return;
	}

	sREAL curAirSpeed;
	sREAL curAlt;
	int i;

	for (i=0;i<8;i++)
		timers[i] += timeFrame;
	/*
	 *	calc some flight information
	 */
	Es = C(32.2 * flightModel->GetAltitudeFPS() + flightModel->GetAirSpeedFPS() * flightModel->GetAirSpeedFPS() / 2);
	Ps = C((flightModel->GetThrust() - flightModel->GetDrag()) / flightModel->GetWeight() * flightModel->GetAirSpeedFPS());

	/*
	 *	Keep track of change in altitude & airspeed
	 */
	curAlt = flightModel->GetAltitudeFPS();
	curAirSpeed = flightModel->GetAirSpeedFPS();
	dAltitude[dAltitudeIdx++] = (curAlt - lastAltitude) * oneOverTimeFrame;
	dAirSpeed[dAirSpeedIdx++] = (curAirSpeed - lastAirSpeed) * oneOverTimeFrame;
	lastAltitude = curAlt;
	lastAirSpeed = curAirSpeed;

	if (dAltitudeIdx >= aiPILOT_AVERAGING_N)
		dAltitudeIdx = 0;
	if (dAirSpeedIdx >= aiPILOT_AVERAGING_N)
		dAirSpeedIdx = 0;

	deltaAltitude = 0.0;
	deltaAirSpeed = 0.0;
	for (i=0;i<aiPILOT_AVERAGING_N;i++)
	{
		deltaAltitude += dAltitude[i];
		deltaAirSpeed += dAirSpeed[i];
	}
	deltaAltitude /= aiPILOT_AVERAGING_N;
	deltaAirSpeed /= aiPILOT_AVERAGING_N;
    /*
     *  Get pitch, yaw for horizon
     */
    GetLevelAttitude(levelAttitude);

    /*
     * Update weapon limits
     */
    weaponLimits.Update(C(timeFrame));

	/*
	 * If we have gunners, update them
	 */
	if (HasGunners())
		UpdateGunners();

	if (GetRemoteControl())
	{
		flightModel->SetUpdateCallback(0,NULL,NULL);
		if (flightModelUpdateFlag)
			flightModel->Update(timeFrame);
		return;
	}
	/*
	 * Allow player to remotely control us
	  
	if (GetRemoteControl())
	{
		GetRemoteControlInputs();
		flightModel->SetUpdateCallback(0,NULL,NULL);
		if (flightModel->GetSlewMode())
		{
			((sSlewer *)flightModel)->SetRoll(0.0);
			flightModel->IncPitch(flightModel->GetPitchControlPer() * slewRads);
			// flightModel->IncRoll(-flightModel->GetYawControlPer() * slewRads);
			flightModel->IncYaw(flightModel->GetRollControlPer() * slewRads );
			sREAL engPer = flightModel->GetEngineControlPer();
			if (engPer < 0.5)
				engPer *= 0.01;
			if (engPer < 0.001)
				engPer = 0.0;
			flightModel->IncY(engPer * slewFeet);
		}
		else
		{
			dbg = "Remote";
			if (task == ENGAGE_TARGET || task == FORMATION_FLY ||
				task == INTERCEPT_TARGET)
			{
				GetTargetGeometry(TARGET_IDX,TARGET_GEOMETRY);
				GetTargetFlags(*curTarget);
				Shoot();
			}
		}
		if (flightModelUpdateFlag)
			flightModel->Update(timeFrame);
	}
	else
	*/
	{
		/*
		 * Otherwise, go do our task
		 */

		/*
		 * Always eval threat from player
		 * and attackers
		 */
		if (timers[ATTACKER_CHECK_TIMER] >= attackerCheckInterval)
		{
			if (!(task == ENGAGE_TARGET && playerPilot &&
				 TARGET_IDX == playerPilot->GetIdx()))
				EvalThreatFromPlayer();
			timers[ATTACKER_CHECK_TIMER] = 0.0;
			sAttacker *nwAttckr = CheckAttackerList();
			if (nwAttckr)
			{
				if (HasGunners())
					SetGunnersTarget(nwAttckr);
				if ((CAN_ENERGY_FIGHT || CAN_ANGLES_FIGHT) &&
					 task == ENGAGE_TARGET && nwAttckr->GetIdx() != TARGET_IDX)
				{
					if (nwAttckr->GetThreatValue() > TARGET_THREAT_VALUE)
						SetTargetPilot(nwAttckr->GetIdx());
				}
			}
		}

		switch (task)
		{

		case NOTHING:
			Nothing();
			break;

		case INTERCEPT_TARGET:
			InterceptTarget();
			break;

		case ENGAGE_TARGET:
			EngageTarget();
			break;

		case EVADE_TARGET:
			EvadeTarget();
			break;

		case FORMATION_FLY:
			FlyFormation();
			break;

		case NAVIGATE:
			Navigate();
			break;

		case AIRSHOW:
			AirShow();
			break;

		case STRAFE:
			Strafe();
			break;

		case ROCKET:  
			Rocket();
			break;

		case LEVEL_BOMB:
			LevelBomb();
			break;

		case DIVE_BOMB:
			DiveBomb();
			break;

		case TORPEDO_BOMB:
			TorpedoBomb();
			break;

		case TAKEOFF:
			TakeOff();
			break;

		case LAND:
			Land();
			break;
		}

		dbg = BuildManeuverStr();
	}
}

void aiPilot::Destroy()
{
	aiBase::Destroy();
}

void aiPilot::SetAsPlayer(int flag)
{
	isPlayerFlag = flag;
	if (isPlayerFlag)
	{
		playerPilot = this;
		flightModel->SetUpdateCallback(0,NULL,NULL);
	}
	else
	{
		if (playerPilot == this)
			playerPilot = NULL;
	}
}

/* 
   If available, incorporate specific
   plane capabilities, control parameters 
*/ 
void aiPilot::CheckPlaneCaps(const char *model)
{
    sPlaneCaps *icp = sPlaneCaps::GetsPlaneCaps(model);
    if (icp)
    {
      maxRollRate = icp->maxRollRate;
      minRollRate = icp->minRollRate;
      pitchPid = icp->pitchPid;
      yawPid = icp->yawPid;
      rollPid = icp->rollPid;
		gPid = icp->gPid;

		collisionAvoidSecs = icp->collisionAvoidSecs;
		immelmanGs = icp->immelmanGs;
		immelmanAltMin = icp->immelmanAltMin;
		noseOnRollLimit = icp->noseOnRollLimit;
		speedLossLimit = icp->speedLossLimit;
		splitSGs = icp->splitSGs;
		splitSAltMin = icp->splitSAltMin;
		posStructGLimit = icp->posGLimit;
		negStructGLimit = icp->negGLimit;
		grndColSecs = icp->grndColSecs;
		grndColAGL = icp->grndColAGL;
		grndColRcvrAGL = icp->grndColRcvrAGL;
		grndColPullUpAngle = icp->grndColPullUpAngle;
		zoomAngle = icp->zoomAngle;
		standardTurnSpeedMPH = icp->standardTurnSpeedMPH;
		finalApproachDistance = icp->finalApproachDistance;
		finalApproachAGL = icp->finalApproachAGL;
      tactics.SetPlaneCaps(*icp);
    }
}

/*
   If available, incorporate specific pilot capabilities
 */
void aiPilot::CheckPilotCaps(const char *cpsId)
{
    sPilotCaps *icp = sPilotCaps::GetsPilotCaps(cpsId);
    if (icp)
    {
      tactics.SetPilotCaps(*icp);
      grndColSecs = icp->grndColSecs;
      grndColAGL = icp->grndColAGL;
      grndColRcvrAGL = icp->grndColRcvrAGL;
		grndColPullUpAngle = icp->grndColPullUpAngle;
		posGLimit = icp->posGLimit;
      negGLimit = icp->negGLimit;
      playerThreatDistance = icp->playerThreatDistance;
      playerThreatAspect = icp->playerThreatAspect;
      attackerCheckInterval = icp->attackerCheckInterval;
      bulletRadiusMin = icp->bulletRadiusMin;
		bulletRadiusMax = icp->bulletRadiusMax;

      gunnersPitchRate = icp->gunnersPitchRate;
      gunnersYawRate = icp->gunnersYawRate;
      gunnersJiggle = icp->gunnersJiggle;

    }
}

void aiPilot::SetCapsId(const char *cpsId)
{
  if (strlen(cpsId) < 31)
    strcpy(this->capsId,cpsId);
  else
    { 
      memcpy(this->capsId,cpsId,31);
      this->capsId[31] = 0;
    }
}

void aiPilot::SetFlightModel(sFlightModel *flightMdl)
{
	this->flightModel = flightMdl;
}

/*******************************************
 * Set surface target                      *
 *******************************************/
void aiPilot::SetSurfaceTarget(uint32_t surfaceTargetIndex, int attackMethod, 
                              sPoint *worldPoint)
{
	ClearManeuverStack();
	if (attackMethod < STRAFE)
		attackMethod = STRAFE;
	if (attackMethod > TORPEDO_BOMB)
		attackMethod = TORPEDO_BOMB;
	SetTask(attackMethod);
	surfaceTarget.index = surfaceTargetIndex;
	if (worldPoint != NULL)
	{
		surfaceTarget.worldPoint = *worldPoint;
		surfaceTarget.isStatic = 1;
	}
	else
		surfaceTarget.isStatic = 0;

	CalcAlignPoint(surfaceTarget);
	GetSurfaceTargetGeometry(surfaceTarget);

	LOG("Engaging surface target");
	/*
	Broadcast(GetAffiliation(),"engsurf","Engaging surface target @ %5.1f %5.1f",
			surfaceTarget.worldPoint.x,surfaceTarget.worldPoint.y);
	*/
	Broadcast(commWINGMAN_ENGAGING_SURFACE);
}

/********************************************
 * set intercept target
 ********************************************/
void aiPilot::SetInterceptTarget(uint32_t targetIdx)
{
	ClearTargetPilot();
	ClearManeuverStack();
	curTarget = &interceptTarget;
	TARGET_IDX = targetIdx;
	GetTargetInfo(TARGET_IDX,TARGET_INFO);
	SetTask(INTERCEPT_TARGET);
	SelectEngagementFightType(targetIdx);
	LOG("Intercept");
}

/*******************************************
 * set target for engagement               *
 *******************************************/
void aiPilot::SetEngageTarget(uint32_t targetIdx)
{
	ClearManeuverStack();
	SetTask(ENGAGE_TARGET);
	curTarget = &engageTarget;
	SetTargetPilot(targetIdx);
	TARGET_IDX = targetIdx;
	GetTargetGeometry(TARGET_IDX,TARGET_GEOMETRY);
	engageTarget.tFlagIndex = 0;
	GetTargetFlags(engageTarget);
	GetTargetInfo(TARGET_IDX,TARGET_INFO);
	SelectEngagementFightType(targetIdx);
	if (HasGunners())
		SetGunnersTarget(targetIdx,TARGET_THREAT_VALUE);
}


/******************************************
 * set target for evasive maneuvers only  *
 ******************************************/
void aiPilot::SetEvadeTarget(uint32_t targetIdx)
{
	ClearManeuverStack();
	SetTask(EVADE_TARGET);
	curTarget = &evadeTarget;
	TARGET_IDX = targetIdx;
	GetTargetInfo(TARGET_IDX,TARGET_INFO);
	SelectEngagementFightType(targetIdx);
}

/******************************************
 * Disengage from current target, if there
 *  is one
 ******************************************/
void aiPilot::Disengage()
{
	ClearTargetPilot();
}

/******************************************
 * set target for formation flying        *
 ******************************************/
void aiPilot::SetFormationTarget(uint32_t targetIdx, int wingPosition, int frmtionTp)
{
	ClearManeuverStack();
	curTarget = &formationTarget;
	wingPos = wingPosition;
	this->formationType = frmtionTp;
	TARGET_IDX = targetIdx;
	GetTargetInfo(TARGET_IDX,TARGET_INFO);
	SetTask(FORMATION_FLY);
   aiPilot *pil = GetaiPilot(TARGET_IDX);
	if (!pil)
	{
		LOG("SetFormationTarget:: invalid targetIdx %d",
			 targetIdx);
		SetTask(NOTHING);
	}
	else
//		Broadcast(affiliation,"forming","Forming with %s at %d feet",
//             pil->GetHandle(),(long)pil->GetAltitudeFPS() /1000 * 1000);
		Broadcast(commWINGMAN_FORMING);
}


/*****************************************
 * set navigation point                  *
 *****************************************/
void aiPilot::SetNavigatePoint(sWaypoint *wp)
{
	ClearManeuverStack();
	navInfo.waypoint = *wp;
	navInfo.navPoint = wp->to;
	navInfo.navPoint.z = wp->altitude;
	SetTask(NAVIGATE);
}

void aiPilot::SetTakeOff(sWaypoint *wp)
{
	ClearManeuverStack();
	navInfo.waypoint = *wp;
	navInfo.navPoint = wp->to;
	navInfo.navPoint.z = wp->altitude;
	SetTask(TAKEOFF);
	clearForTakeoff = 0;
}

void aiPilot::SetLand(sWaypoint *wp)
{
	ClearManeuverStack();
	navInfo.waypoint = *wp;
	navInfo.navPoint = wp->to;
	clearForLanding = 0;
	SetTask(LAND);
}

/*
 * Navigate to waypoint
 */
void aiPilot::Navigate()
{
	// Set flight model update callbacks
    flightModel->SetUpdateCallback(SFMCB_CONTROL_UPDATE |
  							     SFMCB_POSITION_UPDATE |
									SFMCB_ORIENTATION_UPDATE,
									aiPilotUpdateCallback,
									this);
	// Update the flight model
	if (flightModelUpdateFlag)
		flightModel->Update(timeFrame);
	// If needed, push navigate maneuver onto maneuver stack
	if (mnvrStackPtr < 0)
		SetManeuver(sManeuver::NAVIGATE,IMNVR_LOOPBIT);
	// Exec the stack
	ExecManeuverStack();
}

// Nothing ... useful for testing
// maneuvers
void aiPilot::Nothing()
{
	if (flightModelUpdateFlag)
		flightModel->Update(timeFrame);
	if (curManeuver != NULL && !GetRemoteControl())
	{
		ExecManeuverStack();
    	flightModel->SetUpdateCallback(SFMCB_CONTROL_UPDATE |
  							     SFMCB_POSITION_UPDATE |
								  SFMCB_ORIENTATION_UPDATE,
							   aiPilotUpdateCallback,
							   this);
	}
	else 
	{
		if (GetRemoteControl())
		{
			GetRemoteControlInputs();
			flightModel->SetUpdateCallback(0,NULL,NULL);
			dbg = "Remote";
		}
	}
}

/*********************************************
 * Intercept (get within engagement distance *
 *********************************************/
void aiPilot::InterceptTarget()
{
  curTarget = &engageTarget;
	// Set flight model update callbacks
	flightModel->SetUpdateCallback(SFMCB_CONTROL_UPDATE |
							SFMCB_ORIENTATION_UPDATE |
							SFMCB_POSITION_UPDATE,
						aiPilotUpdateCallback,
						this);
	// Update the flight model
	if (flightModelUpdateFlag)
		flightModel->Update(timeFrame);
  // If needed, push engage maneuver onto maneuver stack
  if (mnvrStackPtr < 0)
	SetManeuver(sManeuver::INTERCEPT,IMNVR_LOOPBIT);
  // Exec the stack
  ExecManeuverStack();
}

// Engage a target in air combat
void aiPilot::EngageTarget()
{
	curTarget = &engageTarget;
  // Set flight model update callbacks
   flightModel->SetUpdateCallback(SFMCB_CONTROL_UPDATE |
							   SFMCB_ORIENTATION_UPDATE |
							   SFMCB_POSITION_UPDATE,
						   aiPilotUpdateCallback,
						   this);
	// Update the flight model
	if (flightModelUpdateFlag)
		flightModel->Update(timeFrame);
	//	If needed, push engage maneuver onto maneuver stack
	if (mnvrStackPtr < 0)
		SetManeuver(sManeuver::ENGAGE,IMNVR_LOOPBIT);
    else if (mnvrStack[0].maneuver != sManeuver::ENGAGE)
    {
        LOG("problem here!");
		SetManeuver(sManeuver::ENGAGE,IMNVR_LOOPBIT);
    }
	// Exec the stack
	ExecManeuverStack();
}

// Evade a target
void aiPilot::EvadeTarget()
{
	curTarget = &evadeTarget;
	// Set flight model update callbacks
    flightModel->SetUpdateCallback(SFMCB_CONTROL_UPDATE |
							   SFMCB_ORIENTATION_UPDATE |
							   SFMCB_POSITION_UPDATE,
						   aiPilotUpdateCallback,
						   this);
	// Update the flight model
	if (flightModelUpdateFlag)
		flightModel->Update(timeFrame);
	// If needed, push engage maneuver onto maneuver stack
	if (mnvrStackPtr < 0)
		SetManeuver(sManeuver::EVADE,IMNVR_LOOPBIT);
    else if (mnvrStack[0].maneuver != sManeuver::EVADE)
    {
        LOG("problem here!");
		SetManeuver(sManeuver::EVADE,IMNVR_LOOPBIT);
    }
	// Exec the stack
	ExecManeuverStack();
}

/* fly formation */
void aiPilot::FlyFormation()
{
	flightModel->SetUpdateCallback(SFMCB_CONTROL_UPDATE |
						   SFMCB_ORIENTATION_UPDATE |
						   SFMCB_POSITION_UPDATE,
					   aiPilotUpdateCallback,
					   this);
	if (flightModelUpdateFlag)
		flightModel->Update(timeFrame);
	if (mnvrStackPtr < 0)
		SetManeuver(sManeuver::FORMATION,IMNVR_LOOPBIT);
	ExecManeuverStack();
}

/* TakeOff */
void aiPilot::TakeOff()
{
	flightModel->SetUpdateCallback(SFMCB_CONTROL_UPDATE |
							SFMCB_ORIENTATION_UPDATE |
							SFMCB_POSITION_UPDATE,
						aiPilotUpdateCallback,
						this);
	if (flightModelUpdateFlag)
		flightModel->Update(timeFrame);
	if (mnvrStackPtr < 0)
		SetManeuver(sManeuver::TAKEOFF,IMNVR_LOOPBIT,navInfo.waypoint.altitude);
	ExecManeuverStack();
}

/* land */
void aiPilot::Land()
{
	flightModel->SetUpdateCallback(SFMCB_CONTROL_UPDATE |
						   SFMCB_ORIENTATION_UPDATE |
						   SFMCB_POSITION_UPDATE,
					   aiPilotUpdateCallback,
					   this);
	if (flightModelUpdateFlag)
		flightModel->Update(timeFrame);
	if (mnvrStackPtr < 0)
		SetManeuver(sManeuver::LAND,IMNVR_LOOPBIT);
	ExecManeuverStack();
}

/*****************************************
 * set airshow waypoint                  *
 *****************************************/
void aiPilot::SetAirShowPoint(sWaypoint *wp)
{
	ClearManeuverStack();
	navInfo.waypoint = *wp;
	navInfo.navPoint = wp->to;
	SetTask(AIRSHOW);
}


/*
 * Do "Airshow"
 */
void aiPilot::AirShow()
{
	// Set flight model update callbacks
    flightModel->SetUpdateCallback(SFMCB_CONTROL_UPDATE |
  							     SFMCB_POSITION_UPDATE |
									SFMCB_ORIENTATION_UPDATE,
									aiPilotUpdateCallback,
									this);
	// Update the flight model
	if (flightModelUpdateFlag)
		flightModel->Update(timeFrame);
	// If needed, push navigate maneuver onto maneuver stack
	if (mnvrStackPtr < 0)
		SetManeuver(sManeuver::AIRSHOW,IMNVR_LOOPBIT);
	// Exec the stack
	ExecManeuverStack();
}

void aiPilot::LevelBomb()
{
  flightModel->SetUpdateCallback(SFMCB_CONTROL_UPDATE |
						   SFMCB_ORIENTATION_UPDATE |
						   SFMCB_POSITION_UPDATE,
					   aiPilotUpdateCallback,
					   this);
	if (flightModelUpdateFlag)
	  flightModel->Update(timeFrame);
  if (mnvrStackPtr < 0)
	SetManeuver(sManeuver::LEVEL_BOMB,0);
  ExecManeuverStack();
}

void aiPilot::Strafe()
{

}

void aiPilot::Rocket()
{

}

void aiPilot::DiveBomb()
{

}

void aiPilot::TorpedoBomb()
{

}

int aiPilot::GetTargetActive()
{
  return (TARGET_ACTIVE);
}

int aiPilot::GetInterceptStatus()
{
  return (TARGET_ACTIVE &&
          TARGET_RANGE > ENGAGE_DISTANCE);
}

/*
 *  return whether we are currently
 *  engaged with an active target
 */
int aiPilot::GetEngageStatus()
{
	if (task == ENGAGE_TARGET &&
		curTarget == &engageTarget &&
		TARGET_ACTIVE)
		return (1);
	else
		return (0);
}


/*
 * return index of current target
 */
uint32_t aiPilot::GetTargetIdx()
{
  return (TARGET_IDX);
}

// Call the appropriate update routine for the
// given maneuver
void aiPilot::UpdateManeuver(sManeuverState &ms)
{
	switch (ms.maneuver)
	{
		case sManeuver::STRAIGHT_AND_LEVEL:
			DoStraightAndLevel(ms);
			break;

		case sManeuver::STANDARD_TURN:
			DoStandardTurn(ms);
			break;
		
		case sManeuver::BREAK_TURN:
			DoBreakTurn(ms);
			break;

		case sManeuver::HARD_TURN:
			DoHardTurn(ms);
			break;

		case sManeuver::CLIMB:
			DoClimb(ms);
			break;

		case sManeuver::DESCEND:
			DoDescend(ms);
			break;

		case sManeuver::IMMELMAN:
			DoImmelman(ms);
			break;

		case sManeuver::SPLIT_S:
			DoSplitS(ms);
			break;
		
		case sManeuver::INVERT:
			DoInvert(ms);
			break;

		case sManeuver::EXTEND:
			DoExtend(ms);
			break;

		case sManeuver::ENGAGE:
			DoEngage(ms);
			break;

		case sManeuver::ALIGN:
			DoAlign(ms);
			break;

		case sManeuver::JINK:
			DoJink(ms);
			break;

		case sManeuver::RQ_EVADE:
			DoRQEvade(ms);
			break;

		case sManeuver::RQ_ENGAGE:
			DoRQEngage(ms);
			break;

		case sManeuver::FQ_ENGAGE:
			DoFQEngage(ms);
			break;

		case sManeuver::PURSUIT:
			DoPursuit(ms);
			break;

		case sManeuver::GUN_ATTACK:
			DoGunAttack(ms);
			break;

		case sManeuver::NAVIGATE:
			DoNavigate(ms);
			break;

		case sManeuver::FORMATION:
			DoFormation(ms);
			break;

		case sManeuver::PULLUP:
			DoPullUp(ms);
			break;

     case sManeuver::LEVEL_ROLL:
         DoLevelRoll(ms);
         break;

     case sManeuver::PITCHED_ROLL:
         DoPitchedRoll(ms);
         break;

     case sManeuver::WINGOVER:
         DoWingOver(ms);
         break;

     case sManeuver::LEVEL_BOMB:
         DoLevelBomb(ms);
         break;

     case sManeuver::INTERCEPT:
         DoIntercept(ms);
         break;

     case sManeuver::CLIMBING_TURN:
         DoClimbingTurn(ms);
         break;

     case sManeuver::DESCENDING_TURN:
         DoDescendingTurn(ms);
         break;

     case sManeuver::SNAP_ROLL:
         DoSnapRoll(ms);
         break;

     case sManeuver::CHANDELLE:
         DoChandelle(ms);
         break;

		case sManeuver::AIRSHOW:
			DoAirShow(ms);
			break;

		case sManeuver::AILERON_ROLL:
			DoAileronRoll(ms);
			break;

		case sManeuver::EVADE:
			DoEvade(ms);
			break;

		case sManeuver::ZOOM:
			DoZoom(ms);
			break;

		case sManeuver::BARREL_ROLL:
			DoBarrelRoll(ms);
			break;

		case sManeuver::SPIRAL_DIVE:
			DoSpiralDive(ms);
			break;

		case sManeuver::TAKEOFF:
			DoTakeOff(ms);
			break;

		case sManeuver::LAND:
			DoLand(ms);
			break;

		case sManeuver::TAXI:
			DoTaxi(ms);
			break;
	}
}


// Allow the user to control the flight model
void aiPilot::SetRemoteControl(int rmtCtrl)
{
	this->remoteControl = rmtCtrl;
//	ClearManeuverStack();
}

/**************************************************************
 * flight model update routines
 **************************************************************/
// Update called by the flight model. This allows us to 
// set control inputs as well as override various flight model
// states such as roll rate, pitch rate, velocity.
void aiPilot::UpdateCallback(int sw)
{
	switch (sw)
	{
		case sFlightModel::CONTROL_UPDATE:
		{
			/* 
			   Flight model is about to calculate rotational forces
			   Set the control inputs
			*/
			if (rollCtl.IsOn())
				UpdateRollCtl(rollCtl);
            else if (rollRateCtl.IsOn())
				UpdateRollRateCtl(rollRateCtl);
			
			if (pitchCtl.IsOn())
				UpdatePitchCtl(pitchCtl);
			else if (pitchRateCtl.IsOn())
				UpdatePitchRateCtl(pitchRateCtl);
			else if (gCtl.IsOn())
				UpdateGCtl(gCtl);

         if (yawCtl.IsOn())
            UpdateYawCtl(yawCtl);
         else if (yawRateCtl.IsOn())
				UpdateYawRateCtl(yawRateCtl);
         
			if (speedCtl.IsOn())
				UpdateSpeedCtl(speedCtl);

		}
		break;

		case sFlightModel::ORIENTATION_UPDATE:
		{
			/* 
			   Flight model is about to calculate new orientation.
			   Tweak rotational velocities as needed
			  */
			if (rollLock)
				flightModel->SetRollRate(0.0);
			else if (rollCtlDirect.IsOn())
				UpdateRollCtlDirect(rollCtlDirect);
			else if (rollRateCtlDirect.IsOn())
				UpdateRollRateCtlDirect(rollRateCtlDirect);
			
			if (pitchLock)
				flightModel->SetPitchRate(0.0);
			else if (pitchCtlDirect.IsOn())
				UpdatePitchCtlDirect(pitchCtlDirect);
			else if (pitchRateCtlDirect.IsOn())
				UpdatePitchRateCtlDirect(pitchRateCtlDirect);
		
            if (yawLock)
                flightModel->SetYawRate(0.0);
			else if (yawRateCtlDirect.IsOn())
				UpdateYawRateCtlDirect(yawRateCtlDirect);

			/* check indirect control flags */
			if (rollCtl.flag)
				flightModel->SetRollRate(0.0);
			if (rollRateCtl.flag)
				flightModel->SetRollRate(0.0);

			if (pitchCtl.flag)
				flightModel->SetPitchRate(0.0);
			if (pitchRateCtl.flag)
				flightModel->SetPitchRate(0.0);

         if (yawCtl.flag)
            flightModel->SetYawRate(0.0);
			if (yawRateCtl.flag)
				flightModel->SetYawRate(0.0);
		}
		break;

		case sFlightModel::POSITION_UPDATE:
		/*
		 * flight model is about to calculate position.
		 * tweak/override any vector velocities
		 */
		if (velCtl.IsOn())
			UpdateVelCtl(velCtl);
		break;
	}
}

void aiPilot::SetNoseOn(bool flag, const sPoint &point, sREAL pitch, sREAL yaw)
{
sREAL rollNeeded;

	flag = fabs(pitch) < (_degree * 3.0) && fabs(yaw) < (_degree * 3.0) ;
	ControlsOff();
	if (flag)
		rollNeeded = sATAN(flightModel->GetAirSpeedFPS() * yaw / 32.2);
	else
		rollNeeded = Point2Roll(point);
	SETROLLPID(rollNeeded);
	SETPITCHPID(pitch);
	SETYAWPID(yaw);
}

void aiPilot::SetNoseOn(sREAL pitch, sREAL yaw, sREAL rlLim)
{
sREAL rollNeeded;

   ControlsOff();
   rollNeeded = sATAN(flightModel->GetAirSpeedFPS() *
					 yaw / 32.2);
   if (fabs(rollNeeded) > rlLim)
   {
      if (rollNeeded < 0.0)
        rollNeeded = -rlLim;
      else
        rollNeeded = rlLim;
   }
	SETROLLPID(rollNeeded);
	SETPITCHPID(pitch);
	SETYAWPID(yaw);
}

void aiPilot::SetNoseOnX(sREAL pitch, sREAL yaw, sREAL mul, 
                              sREAL min, sREAL rlLim)
{
   sREAL rollNeeded;
   ControlsOff();

   rollNeeded = sATAN(flightModel->GetAirSpeedFPS() *
					 yaw / 32.2);
   if (fabs(rollNeeded) > rlLim)
   {
      if (rollNeeded < 0.0)
        rollNeeded = -rlLim;
      else
        rollNeeded = rlLim;
   }
   SETCONTROLX(rollCtl,rollNeeded,mul,min);
   SETCONTROLX(pitchRateCtl,pitch,mul,min);
   SETCONTROLX(yawRateCtl,yaw,mul,min);
}

/*
 *  Set all flight model controls off
 */
void aiPilot::ControlsOff()
{
	rollCtl.Off();
	rollCtlDirect.Off();
	rollRateCtl.Off();
	rollRateCtlDirect.Off();

	pitchRateCtl.Off();
	pitchRateCtlDirect.Off();
	pitchCtl.Off();
	pitchCtlDirect.Off();

	yawRateCtl.Off();
	yawRateCtlDirect.Off();
	yawCtl.Off();

	speedCtl.Off();
	gCtl.Off();
	velCtl.Off();

	rollLock = 0;
	pitchLock = 0;
	yawLock = 0;
}

/*
 *  Roll control - indirect
 */
void aiPilot::UpdateRollCtl(sControlState &ctl)
{

	sREAL distance;
	sREAL per;
	sREAL p,d,i;
	sREAL fact;
	sREAL curSpeed;

	if (flightModel->ControlsProportional())
	{
		curSpeed = flightModel->GetAirSpeedFPS() + eps;
		fact = rollPid.base / (curSpeed * curSpeed);
		p = rollPid.p * fact;
		i = rollPid.i * fact;
		d = rollPid.d * fact;
	}
	else
	{
		p = rollPid.p;
		i = rollPid.i;
		d = rollPid.d;
	}
		
   ctl.flag = 0;
   distance = sGetCircularDistance(ctl.setPoint,flightModel->GetRoll());
   per = ((distance * p) + (flightModel->GetRollRate() * d)) * i;
   flightModel->SetRollControlPer(per);
}

/* 
 * Direct setting of roll.
 * ctl.goal = roll desired
 */
void aiPilot::UpdateRollCtlDirect(sControlState &ctl)
{
	sREAL curRoll;
	//sREAL diff;
	sREAL rate;

	ctl.flag = 0;
	curRoll = flightModel->GetRoll();
	rate = sGetCircularDistance(curRoll,ctl.setPoint);
	//diff = fabs(rate);
	rate *= rollMult;
	rollLimit = Pi_4;
	/* GetMaxRollRate() doesn't work quite right yet */
	if (rollLimit > maxRollRate)
		rollLimit = maxRollRate;
	if (rollLimit < minRollRate)
		rollLimit = minRollRate;
	if (fabs(rate) > rollLimit)
	{
		if (rate < 0.0)
			rate = -rollLimit;
		else
			rate = rollLimit;
	}
	flightModel->SetRollRate(rate);
	flightModel->SetRollControlPer(0.0);
}

/* indirect control of roll rate
 * ctl.goal = roll rate desired
 */
void aiPilot::UpdateRollRateCtl(sControlState &ctl)
{
	sREAL d;
	sREAL curRate;
	sREAL stepPer;

	ctl.flag = 0;
	curRate = flightModel->GetRollRate();
	d = ctl.setPoint - curRate;
	stepPer = d / _2Pi;
	flightModel->IncRollControlPer(stepPer * ctl.controlStep);
}

/* 
 *  direct control of roll rate
 * ctl.goal = roll rate desired
 */
void aiPilot::UpdateRollRateCtlDirect(sControlState &ctl)
{
	ctl.flag = 0;
	flightModel->SetRollRate(ctl.setPoint);
}


/*
 *  Set pitch rate indirect
 *  ctl.goal = rate of pitch desired 
 */
void aiPilot::UpdatePitchRateCtl(sControlState &ctl)
{
	sREAL d;
	sREAL curRate;
	sREAL stepPer;

	ctl.flag = 0;
	curRate = flightModel->GetPitchRate();
	d = ctl.setPoint - curRate;
   stepPer = d / _2Pi;
	flightModel->IncPitchControlPer(stepPer * ctl.controlStep);
}

/* Direct setting of pitch rate */
/* ctl.goal = rate desired */
void aiPilot::UpdatePitchRateCtlDirect(sControlState &ctl)
{
	ctl.flag = 0;
	flightModel->SetPitchRate(ctl.setPoint);
}

/*
 * Set pitch control using PID loop
 * ctl.setPoint = distance
 */
void aiPilot::UpdatePitchCtl(sControlState &ctl)
{
	sREAL distance;
	sREAL per;
	sREAL p,d,i;
	sREAL fact;
	sREAL curSpeed;

	if (flightModel->ControlsProportional())
	{
		curSpeed = flightModel->GetAirSpeedFPS() + eps;
		fact = pitchPid.base / (curSpeed * curSpeed);
		p = pitchPid.p * fact;
		i = pitchPid.i * fact;
		d = pitchPid.d * fact;
	}
	else
	{
		p = pitchPid.p;
		i = pitchPid.i;
		d = pitchPid.d;
	}
   ctl.flag = 0;
   distance = ctl.setPoint;
   per = ((distance * p) - ((flightModel->GetPitchRate() - ctl.dVel) * d)) * i;
   flightModel->SetPitchControlPer(per);
}

/*
 *  Direct setting of pitch
 */
void aiPilot::UpdatePitchCtlDirect(sControlState &ctl)
{
	sREAL rate;
	sREAL curPitch;
	ctl.flag = 0;
	curPitch = flightModel->GetPitch();
	rate = sGetCircularDistance(curPitch,ctl.setPoint);
	flightModel->SetPitchRate(rate);
}

/*
 * Set yaw rate
 * ctl.goal = rate of yaw desired
 */
void aiPilot::UpdateYawRateCtl(sControlState &ctl)
{
	sREAL d;
	sREAL curRate;
	sREAL stepPer;

	ctl.flag = 0;
	curRate = flightModel->GetYawRate();
	d = ctl.setPoint - curRate;
    stepPer = d / _2Pi;
	flightModel->IncYawControlPer(stepPer * ctl.controlStep);
}

void aiPilot::UpdateYawRateCtlDirect(sControlState &ctl)
{
	ctl.flag = 0;
	flightModel->SetYawRate(ctl.setPoint);
}

// Set yaw control using PID loops
// ctl.setPoint = distance
void aiPilot::UpdateYawCtl(sControlState &ctl)
{
	sREAL distance;
	sREAL per;
	sREAL p,d,i;
	sREAL fact;
	sREAL curSpeed;

	if (flightModel->ControlsProportional())
	{
		curSpeed = flightModel->GetAirSpeedFPS() + eps;
		fact = yawPid.base / (curSpeed * curSpeed);
		p = yawPid.p * fact;
		i = yawPid.i * fact;
		d = yawPid.d * fact;
	}
	else
	{
		p = yawPid.p;
		i = yawPid.i;
		d = yawPid.d;
	}
   ctl.flag = 0;
   distance = ctl.setPoint;
   per = ((distance * p) - ((flightModel->GetYawRate() - ctl.dVel) * d))  * i;

   flightModel->SetYawControlPer(per);
}

/*
 *  Control g's being pulled
 */
void aiPilot::UpdateGCtl(sControlState &ctl)
{
	sREAL	distance;
	sREAL stepPer;
	sREAL curLoad;
	sREAL p,i,d;
	sREAL fact;
	sREAL curAirSpeed;

	curAirSpeed = flightModel->GetAirSpeedFPS();
	if (curAirSpeed <= 0.0)
		return;

	if (flightModel->ControlsProportional())
	{
		fact = gPid.base / (curAirSpeed * curAirSpeed);
		p = gPid.p * fact;
		i = gPid.i * fact;
		d = gPid.d * fact;
	}
	else
	{
		p = gPid.p;
		i = gPid.i;
		d = gPid.d;
	}
	ctl.flag = 0;
	curLoad = flightModel->GetLoad();
	distance = ctl.setPoint - curLoad;
	stepPer = (distance * p - flightModel->GetLoadRate() * d) * i;
	flightModel->IncPitchControlPer(stepPer);
}

/*
 *  Control speed
 */
void aiPilot::UpdateSpeedCtl(sControlState &ctl)
{
	sREAL curAirSpeed;
	sREAL	distance;
	sREAL stepPer;

	ctl.flag = 0;
	curAirSpeed = flightModel->GetAirSpeedFPS();
	distance = ctl.setPoint - curAirSpeed;
	if (fabs(distance) > 33.0 && distance < 0.0)
		flightModel->SetSpeedBreaks(1.0);
	else
		flightModel->SetSpeedBreaks(0.0);
	stepPer = (distance * airSpeedPid.p - deltaAirSpeed * airSpeedPid.d) * airSpeedPid.i;
	flightModel->IncEngineControlPer(stepPer);
}	

/*****************************************
 * directly set the y velocity component *
 *****************************************/
void aiPilot::UpdateVelCtl(sControlState &ctl)
{
	sVector curVel;
	ctl.flag = 0;
	flightModel->GetVelocity(curVel);
	curVel.y = ctl.setPoint;
	flightModel->SetVelocity(curVel);
}

/*
 *  Return whether or not we are in shoot params
 */
int aiPilot::InShootParams()
{
  int result = 0;
  GetWeaponLimits(weaponLimits);
  if (INRANGE(GUN_LEAD_RANGE_SQ,weaponLimits.minRange*weaponLimits.minRange,
									weaponLimits.maxRange*weaponLimits.maxRange))
  {
		sREAL pitchLimit;
		sREAL yawLimit;
		sREAL mul;

		mul = weaponLimits.maxRange / TARGET_RANGE;
		pitchLimit = weaponLimits.cone.pitch * mul;
		if (pitchLimit > Pi_4)
			pitchLimit = Pi_4;
		yawLimit = weaponLimits.cone.yaw * mul;
		if (yawLimit > Pi_4)
			yawLimit = Pi_4;
		result = INRANGE(GUN_LEAD_PITCH,-pitchLimit,pitchLimit) &&
				   INRANGE(GUN_LEAD_YAW,-yawLimit,yawLimit);
  }
 
  result = weaponLimits.UpdateBurst(result);
  return(result);
}

/*****************************************
 * determine target flag pairs 
 *****************************************/

/*
 *  Average out flag pairs
 */
void aiPilot::GetTargetFlags(sTarget &tg)
{
	tg.active = GetTargetActive(tg.idx);
	if (tg.active)
	{
		GetTargetFlags(tg.tFlags[tg.tFlagIndex++],tg.geometry);
		if (tg.tFlagIndex >= aiPILOT_AVERAGING_N)
			tg.tFlagIndex = 0;
		tg.flags.Reset();
		for (int i=0;i<aiPILOT_AVERAGING_N;i++)
			tg.flags.Add(tg.tFlags[i]);
		 tg.flags.Set(static_cast<int>(aiPILOT_AVERAGING_N / 2.0));
	}
}

/*
 *  Determine target flag pairs at given instance
 *  using target geometry
 */
void aiPilot::GetTargetFlags(sTargetFlags &tflags, sTargetGeometry &tGeom)
{
	sREAL d;

	tflags.Reset();

	/* range closing or opening? */
	if (fabs(tGeom.dRange) > tactics.rangeThreshold)
	{
		if (tGeom.dRange < 0.0)
			tflags.rangeOpen = 1;
		else
			tflags.rangeClose = 1;
	}

	/* target high or low? */
	d = tGeom.worldPosition.z - flightModel->GetAltitudeFPS();

	if (d > 0)
		tflags.above = 1;
	else
		tflags.below = 1;
	if (fabs(d) >= tactics.altThreshold)
	{
		if (d > 0)
			tflags.high = 1;
		else
			tflags.low = 1;
	}
	if (fabs(d) >= tactics.altThreshold * 1.5)
	{
		if (d > 0)
			tflags.veryHigh = 1;
		else 
			tflags.veryLow = 1;
	}

	/* target turning ? */
	if (fabs(tGeom.turnRate) > tactics.turnThreshold)
		tflags.turning = 1;
	else
		tflags.turning_not = 1;

	/* aspect head on or tail */
	if (tGeom.aspect < HEADON_ASPECT)
		tflags.tailAspect = 1;
	else if (tGeom.aspect > Pi - TAIL_ASPECT)
		tflags.headOnAspect = 1;

	/* angle off low or high */
	if (tGeom.angleOff < tactics.angleOffCone)
		tflags.lowAOT = 1;
	else if (tGeom.angleOff > Pi - tactics.angleOffCone)
		tflags.highAOT = 1;

	/* target in forward or rear quarter */
	if (fabs(tGeom.relAttitude.yaw) < tactics.targetFQCone)
		tflags.forward = 1;
	else if (fabs(tGeom.relAttitude.yaw) > (Pi - tactics.targetRQCone))
	   tflags.rear = 1;
    if (fabs(tGeom.relAttitude.yaw) < tactics.targetFQCone * 0.2)
        tflags.onNose = 1;
    else if (fabs(tGeom.relAttitude.yaw) > (Pi - tactics.targetRQCone * 0.3))
        tflags.onTail = 1;
}


/**************************************************************
 * calculate target geometry                                  *
 **************************************************************/
void aiPilot::GetTargetGeometry(uint32_t targetIdx, sTargetGeometry &tg)
{
sREAL			nRange;
sREAL			newRange;
sREAL			dotProd;
sVector		temp;
sPoint		targetPosition;
sAttitude	attitudeToTarget;
sVector		directionToTarget;
sVector		targetHeading;
sVector		targetVelocity;
sVector		ourHeading;
sPoint		ourPosition;
sAttitude	ourAttitude;
sPoint		leadPoint;
int			i;

	GetPositionAndAttitude(ourPosition,ourAttitude);
	GetAttitudeToTarget(targetIdx,targetPosition,attitudeToTarget,
						directionToTarget,targetHeading,targetVelocity);
	tg.worldVelocity = targetVelocity;
	GetDirectionNormal(ourHeading);		

	/* calc up the target info */
	tg.worldPosition = targetPosition;
	tg.worldHeading = targetHeading;
	tg.worldDirection = directionToTarget;
	tg.relAttitude = attitudeToTarget;
	newRange = directionToTarget.Magnitude();
	tg.ranges[tg.rng_idx++] = (tg.range - newRange) * oneOverTimeFrame;
	if (tg.rng_idx >= aiPILOT_AVERAGING_N)
		tg.rng_idx = 0;
	nRange = 0.0;
	for (i = 0;i<aiPILOT_AVERAGING_N;i++)
		nRange += tg.ranges[i];
	nRange /= aiPILOT_AVERAGING_N;
	tg.dRange = nRange;
	tg.range = newRange;
	
	/* calc aspect */
	directionToTarget.Normalize();
	targetHeading.Normalize();
	dotProd = dot(directionToTarget,targetHeading);
	tg.aspect = sACOS(dotProd);
	/* calc angle off */
	ourHeading.Normalize();
	dotProd = dot(ourHeading,targetHeading);
	tg.angleOff = sACOS(dotProd);

	/* Get gun lead information */
	CalcGunLeadPoint(tg);


	/* get lead pursuit information */
	leadPoint = targetPosition + targetVelocity;
	tg.leadPoint = leadPoint;
	GetAttitude(leadPoint,temp,tg.leadAttitude);

	/* get normalized lead angular rates */
	leadPoint = ourPosition + targetVelocity;
	GetAttitude(leadPoint,temp,tg.leadRate);

	/* get lag pursuit information */
	temp = targetVelocity;
	temp *= -1.0;
	leadPoint = targetPosition + temp;
	tg.lagPoint = leadPoint;
	GetAttitude(leadPoint,temp,tg.lagAttitude);

	/* if formation flying, get offset vector from leader */
	if (task == FORMATION_FLY && wingPos > 0)
	{
		sPoint   pos;
		sAttitude att;
		GetFormationPoint(wingPos,formationType,
							formationTarget.idx,
							tg.formationPoint);
		flightModel->GetPositionAndAttitude(pos,att);
		sVector v0 = tg.formationPoint - pos;
		tg.formationRange = v0.Magnitude() - formationOffset;
		tg.formationDRange = tg.dRange;
		GetAttitude(tg.formationPoint,temp,tg.formationAttitude);
	}

	/* more stuff */
	tg.turnRate = GetTargetTurnRate(targetIdx);
	tg.load     = GetTargetLoad(targetIdx);
	tg.airSpeed = GetTargetAirSpeed(targetIdx);
}

void aiPilot::CalcGunLeadPoint(sTargetGeometry &tg)
{
sREAL t;
sVector temp;
sPoint leadPoint;
sPoint ourPosition;
sAttitude ourAtt;

	GetPositionAndAttitude(ourPosition,ourAtt);
	/* calc time for bullet to reach target */
	t = tg.range / weaponLimits.initialVelocity;
	temp = tg.worldVelocity;
	/* get velocity vector for that time */
	temp *= t;
	/* add it to target's position to get gun lead */
	leadPoint = tg.worldPosition + temp;
	/* account for gravity drop */
   leadPoint.z += 16.0 * t * t;
 	tg.gunLeadPoint = leadPoint;
	/* calc attitude for lead */
	GetAttitude(leadPoint,temp,tg.gunLeadAttitude);
	tg.gunLeadRangeSq = sDistanceSquared(tg.gunLeadPoint,ourPosition);
}

/*****************************************
 * abbreviated version of above          *
 *****************************************/
void aiPilot::GetQuickTargetGeometry(uint32_t targetIdx, sTargetGeometry &tg)
{
	sREAL dotProd;
	sPoint    targetPosition;
	sAttitude attitudeToTarget;
	sVector   directionToTarget;
	sVector   targetHeading;
	sVector	  targetVelocity;
	sVector	  ourHeading;

	GetAttitudeToTarget(targetIdx,targetPosition,attitudeToTarget,
						directionToTarget,targetHeading,targetVelocity);
	tg.worldVelocity = targetVelocity;
	GetDirectionNormal(ourHeading);		
	/* calc up the target info */
	tg.worldPosition = targetPosition;
	tg.worldHeading = targetHeading;
	tg.worldDirection = directionToTarget;
	tg.relAttitude = attitudeToTarget;
	tg.range = directionToTarget.Magnitude();
	
	/* calc aspect */
	directionToTarget.Normalize();
	targetHeading.Normalize();
	dotProd = dot(directionToTarget,targetHeading);
	tg.aspect = sACOS(dotProd);
	/* calc angle off */
	ourHeading.Normalize();
	dotProd = dot(ourHeading,targetHeading);
	tg.angleOff = sACOS(dotProd);
}

void aiPilot::GetDirectionNormal(sVector &directionNormal)
{
	flightModel->GetDirectionNormal(directionNormal);
}

void aiPilot::GetAttitudeToTarget(uint32_t targetIdx,
											sPoint    &targetPosition,
											sAttitude &attitudeToTarget,
											sVector   &directionToTarget,
											sVector   &targetHeading,
											sVector   &targetVelocity
											)
{
	sAttitude dummy;
	aiPilot *tPilot = GetaiPilot(targetIdx);
	if (tPilot)
	{
		sFlightModel *flt = tPilot->GetFlightModel();
		flt->GetPositionAndAttitude(targetPosition,dummy);
		GetAttitude(targetPosition,directionToTarget,attitudeToTarget);
		flt->GetWorldVelocity(targetVelocity);
		flt->GetDirectionNormal(targetHeading);
		attitudeToTarget.roll = flt->GetRoll();
	}
}

/*
 *   Return turn rate for a given target index
 *   
 */
sREAL aiPilot::GetTargetTurnRate(uint32_t targetIdx)
{
	sREAL result;
	aiPilot *tPilot = GetaiPilot(targetIdx);
	if (tPilot)
	{
		sFlightModel *flt = tPilot->GetFlightModel();
		result = flt->GetTurnRate();
	}
	else
		result = 0.0;
	return (result);
}

/* 
 *  Return load for given target index 
 */
sREAL aiPilot::GetTargetLoad(uint32_t targetIdx)
{
	sREAL result;
	aiPilot *tPilot = GetaiPilot(targetIdx);
	if (tPilot)
	{
		sFlightModel *flt = tPilot->GetFlightModel();
		result = flt->GetLoad();
	}
	else
		result = 0.0;
	return (result);
}

/*
 *  Return airspeed for given target index
 */
sREAL aiPilot::GetTargetAirSpeed(uint32_t targetIdx)
{
	sREAL result;
	aiPilot *tPilot = GetaiPilot(targetIdx);
	if (tPilot)
	{
		sFlightModel *flt = tPilot->GetFlightModel();
		result = flt->GetAirSpeedFPS();
	}
	else
		result = 0.0;
	return (result);
}

void aiPilot::GetTargetInfo(uint32_t targetIdx, sTargetInfo &targetInfo)
{
	aiPilot *tPilot = GetaiPilot(targetIdx);
	if (tPilot)
	{
		sFlightModel *flightModl = tPilot->GetFlightModel();
		if (flightModl)
		{
			strncpy(targetInfo.model,flightModl->GetModel(),sizeof(targetInfo.model));
			sPlaneCaps *icp = sPlaneCaps::GetsPlaneCaps(flightModl->GetModel());
			if (icp != NULL)
			{
				targetInfo.tacticalCaps[sPilotTactics::ENERGY_FIGHT] = icp->canDoEnergyFight;
				targetInfo.tacticalCaps[sPilotTactics::ANGLES_FIGHT] = icp->canDoAnglesFight;
				targetInfo.tacticalCaps[sPilotTactics::STRAFE] = icp->canStrafe;
				targetInfo.tacticalCaps[sPilotTactics::ROCKET] = icp->canRocket;
				targetInfo.tacticalCaps[sPilotTactics::DIVE_BOMB] = icp->canDiveBomb;
				targetInfo.tacticalCaps[sPilotTactics::TORPEDO_BOMB] = icp->canTorpedoBomb;
				targetInfo.tacticalCaps[sPilotTactics::LEVEL_BOMB] = icp->canLightBomb;
				targetInfo.weaponsRange = icp->weaponsRange;
				targetInfo.weaponsEffect = icp->weaponsEffect;
			}
			targetInfo.maxSpeed = flightModl->GetMaxSpeedFPS();
		}
	}
}

/*
 * Get whether a given target is active
 */
int aiPilot::GetTargetActive(uint32_t targetIdx)
{
	int result = 0;
	aiPilot *tPilot = GetaiPilot(targetIdx);
	if (tPilot)
		result = !(tPilot->IsDead() || tPilot->IsEjected());
	return (result);
}

/*********************************************
 * calc up navigation geometry               *
 *********************************************/
void aiPilot::GetNavigationGeometry(sNavInfo &ni)
{
	sPoint			work;
	sAttitude      att;
	sREAL				newRange;

	GetPositionAndAttitude(work,att);
	work.x = ni.navPoint.x;
	work.y = ni.navPoint.y;
	GetAttitude(work,ni.worldDirection,ni.relAttitude);
	newRange = ni.worldDirection.Magnitude();
	sVector2sAttitude(ni.worldDirection,ni.attitude);    
	ni.dRange = (newRange - ni.range) * oneOverTimeFrame;
	ni.range = newRange;
}

sREAL aiPilot::GetNavigationDistance()
{
  return (NAV_RANGE);
}

/*
 * Determine guiding point for formation flying
 */
void aiPilot::GetFormationPoint(int wngPos, int frmationType, unsigned int leaderIndex, sPoint &pnt)
{
	sVector offset;
	sFormationData::GetWingmanVector(frmationType,wngPos,offset);
	offset *= formationWingLen;
	offset.y += formationOffset;
	BodyPoint2WorldPoint(leaderIndex,static_cast<const sPoint &>(offset),pnt);
}

/*
 *  Surface target
 */
void aiPilot::GetSurfaceTargetGeometry(sSurfaceTarget &st)
{
	sPoint      ourPos;
    sPoint      work0;
    sVector     wdir;
	sAttitude    att;
    sREAL     ourAlt;

	GetPositionAndAttitude(ourPos,att);
    ourAlt = ourPos.z;
    if (task == LEVEL_BOMB)
    {
       /* 
         for level bombing, use align point so we
         pass over the target
       */
        work0.x = st.alignPoint.x;
        work0.y = st.alignPoint.y;
        work0.z = ourAlt;
        GetAttitude(work0,wdir,st.relAttitude);
        work0.x = st.worldPoint.x;
        work0.y = st.worldPoint.y;
        st.worldDirection = work0 - ourPos;
    }
    else
        GetAttitude(st.worldPoint,st.worldDirection,st.relAttitude);

	sREAL newRange = st.worldDirection.Magnitude();
	st.dRange = (newRange - st.range) * oneOverTimeFrame;
	st.range = newRange;
    sREAL newV = ourAlt - st.worldPoint.z;
    st.dVertDistance = (newV - st.vertDistance) * oneOverTimeFrame;
    st.vertDistance = newV;
}

sREAL aiPilot::GetSurfaceTargetDistance()
{
    return (SURFACE_TARGET_RANGE);
}

/*
 * Calculate an aligning point which will
 * allow us to pass over the surface target ...
 * for level bombing run
 */
void aiPilot::CalcAlignPoint(sSurfaceTarget &st)
{
	sPoint      work;
	sAttitude    att;
	sPoint     work0;
	sVector        v;

	GetPositionAndAttitude(work,att);
	work0 = st.worldPoint;
	work0.z = 0.0;
	work.z = 0.0;
	v = work0 - work;
	v.Normalize();
	v *= 12000.0;
	st.alignPoint = work0 + v;
	st.worldPoint.z = flightModel->GetGroundHeight(st.worldPoint);
}

/**********************************************************
 * Attack list routines                                   *
 **********************************************************/
void aiPilot::AddAttacker(aiPilot *pil)
{
	/* 
		don't add as an attacker if we're engaged
	   with them
	*/
	if (task==ENGAGE_TARGET &&
		 TARGET_IDX == pil->GetIdx())
		 return;
	if (pil->IsPlayer())
		playerInAttackList = 1;
	aiBase::AddAttacker(pil->GetIdx());
	ATTACKER_CHECK;
}

void aiPilot::DeleteAttacker(aiPilot *pil)
{
	aiBase::DeleteAttacker(pil->GetIdx());
	if (pil->IsPlayer())
		playerInAttackList = 0;
	if (HasGunners())
	{
		for (unsigned int i=0;i<gunners.Count();i++)
		{
			aiGunner *gunner = GetGunner(i);
			if (gunner->HasTarget() &&
				 gunner->GetTargetIdx() == pil->GetIdx())
				gunner->UnsetTarget();
		}
	}
	ATTACKER_CHECK;
}

/*
 *	Evaluate threat from target
 */
sREAL aiPilot::EvalThreat(uint32_t idx, aiPilot **pil)
{
	sTargetGeometry tg;
	aiPilot *ppil;
	sREAL	 maxDistance = 30000.0 / 500.0;

	sREAL result = ITARGET_MIN_THREAT_VALUE;

	ppil = GetaiPilot(idx);
	if (ppil)
	{
		GetQuickTargetGeometry(idx,tg);
		sREAL distanceVal = C(tg.range / 500.0);
		if (distanceVal > maxDistance)
			distanceVal = maxDistance;
		result += maxDistance - distanceVal;
	}
	LOG("EvalThreat for %ld = %5.1f",
		idx,result);
	*pil = ppil;
	return result;
}

/*
 * determine threat value of attackers
 * and return highest attacker
 */
sAttacker *aiPilot::EvalAttackerList()
{
	sAttacker *attkr;
	aiPilot    *pil;
	sREAL     threatValue;
	sREAL     hiThreat = 0;
	int       primer;
	sAttacker *result = NULL;

	if (GetAttackerCount() == 0)
		return NULL;
	/* 
	derive a threat score for each
	attacker
	*/
	attkr = attackList.GetFirstAttacker();
	primer = 1;
	while (attkr)
	{
		threatValue = EvalThreat(attkr->GetIdx(),&pil);
		if (pil)
		{
			if (primer)
			{
				primer = 0;
				hiThreat = threatValue;
				result = attkr;
			}
			else if (threatValue > hiThreat)
			{
				hiThreat = threatValue;
				result = attkr;
			}
		}
		else
			/* invalid idx -- delete attacker */
			attackList.DeleteAttacker(attkr->GetIdx());
		attkr = attackList.GetNextAttacker();
	}
	return result;
}

sAttacker *aiPilot::CheckAttackerList()
{
	sAttacker *result = NULL;
	/*
	 *  Check attacker list
	 */
	if (GetAttackerCount() > 0)
	{
		sAttacker *attkr;
		attkr = EvalAttackerList();
		int nwAttckr = 0;
		if (attkr && currentAttkrPrime == 0)
		{
			nwAttckr = 1;
			currentAttkrPrime = 1;
		}
		else if (attkr && attkr->GetIdx() != currentHiThreatAttkrIdx)
			nwAttckr = 1;

		if (nwAttckr)
		{
			result = attkr;
			currentHiThreatAttkrIdx = attkr->GetIdx();
		}
	}
	else
		currentAttkrPrime = 0;

	return result;
}

/*
 *  consider threat from player,
 *  and add player to attacker list
 *  if not there already.
 */
int aiPilot::EvalThreatFromPlayer()
{
	sTargetGeometry tg;
	int result = 0;
	sREAL	threatDistance;
	/*
	*  playerPilot must have been set
	*/
	if (playerPilot == NULL)
		return(0);
	if (playerPilot->GetAffiliation() == GetAffiliation())
		return(0);
	threatDistance = playerThreatDistance;
	if (task !=	ENGAGE_TARGET)
		threatDistance *= 2.1;
	GetQuickTargetGeometry(playerPilot->GetIdx(),tg);
	if (tg.range <= playerThreatDistance)
	{
		if (!playerInAttackList)
		{
			result = 1;
			AddAttacker(playerPilot);
		}
	}
	else if (playerInAttackList)
		DeleteAttacker(playerPilot);
	return (result);
}

/*****************************************************
 * gunner stuff                                      *
 *****************************************************/
void aiPilot::AddGunner(aiGunner *newGunner)
{
	gunners.Add(newGunner);
	newGunner->Init();
	hasGunners = 1;
}

aiGunner *aiPilot::GetGunner(unsigned int whichGunner)
{
	if (hasGunners && whichGunner < gunners.Count())
		return static_cast<aiGunner *>(gunners[whichGunner]);
	else
		return NULL;
}

void aiPilot::UpdateGunners()
{
	if (HasGunners())
		for (unsigned int i=0;i<gunners.Count();i++)
			GetGunner(i)->Update(timeFrame);
}

void aiPilot::SetGunnersTarget(uint32_t targetIdx, sREAL threatValue)
{
	if (HasGunners())
		for (unsigned int i=0;i<gunners.Count();i++)
			SetGunnerTarget(i,targetIdx,threatValue);
}

void aiPilot::SetGunnersTarget(sAttacker *attkr)
{
	if (HasGunners())
		for (unsigned int i=0;i<gunners.Count();i++)
			SetGunnerTarget(i,attkr->GetIdx(),attkr->GetThreatValue());
}

void aiPilot::SetGunnerTarget(unsigned int whichGunner, uint32_t targetIdx, sREAL targetThreatValue)
{
	aiGunner *gunner = GetGunner(whichGunner);
	if (gunner->HasTarget() &&
		 gunner->GetTargetThreatValue() > targetThreatValue)
		return;
	gunner->SetTarget(targetIdx);
	gunner->SetTargetThreatValue(targetThreatValue);
}

void aiPilot::ClearGunnersTarget()
{
	if (HasGunners())
	{
		for (unsigned int i=0;i<gunners.Count();i++)
		{
			aiGunner *gunner = GetGunner(i);
			gunner->UnsetTarget();
		}
	}
}

void aiPilot::DistributeAttackers()
{
	if (!HasGunners())
		return;

	sAttacker *attkr;
	attkr = attackList.GetFirstAttacker();
	while (attkr)
	{
		for (unsigned int i=0;i<gunners.Count();i++)
		{
			aiGunner *gunner = GetGunner(i);
			gunner->SetTarget(attkr->GetIdx());
		}
		attkr = attackList.GetNextAttacker();
	}
}

/**********************************************
 * miscellaneous                              *
 **********************************************/

sREAL aiPilot::GetDamagePer()
{
  if (!IsActive())
    return (0);
  else
    return (1.0);
}

void aiPilot::SetDeath(bool ded)
{
	this->dead = ded;
    if (dead)
      ClearTargetPilot();
}

void aiPilot::SetEjected(int ejctd)
{
	this->ejected = ejctd;
    if (ejected)
      ClearTargetPilot();
}

void aiPilot::ClearTargetPilot()
{
	aiPilot *pil = GetaiPilot(TARGET_IDX);
	if (pil)
		pil->DeleteAttacker(this);
	engageTarget.active = 0;
}

void aiPilot::SetTargetPilot(uint32_t targetIdx)
{

	aiPilot *pil;

	ClearTargetPilot();
	TARGET_IDX = targetIdx;
	TARGET_THREAT_VALUE = EvalThreat(targetIdx,&pil);
	if (!pil)
	{
		TARGET_ACTIVE = 0;
		return;
	}

	//int broadcastChannel = GetAffiliation();
	//if (pil->IsPlayer())
	//	broadcastChannel = aiPILOT_BROADCAST_ALL;

	/*
	 * Inform target pilot only if
	 * we're engaging
	 */
	if (task == ENGAGE_TARGET)
		pil->AddAttacker(this);
	Broadcast(task == ENGAGE_TARGET ? commWINGMAN_ENGAGING : commWINGMAN_TARGETING,
				 commCHANNEL_AFFILIATION, commPRIORITY_STANDARD,
				 pil);
	 LOG("%s %s in %s",
	  task == ENGAGE_TARGET ? "Engaging" : "Targeting",
		  pil->GetHandle(),
		  pil->GetModel());
}

void aiPilot::GetPositionAndAttitude(sPoint &position, sAttitude &attitude)
{
	if (flightModel != NULL)
		flightModel->GetPositionAndAttitude(position,attitude);
}

void aiPilot::SetPositionAndAttitude(const sPoint &position,
												const sAttitude &attitude)
{
	if (flightModel != NULL)
		flightModel->SetPositionAndAttitude(position,attitude);
}

const char *aiPilot::GetModel()
{
  if (flightModel)
	return (flightModel->GetModel());
	else
	return ("Player");
}

sREAL aiPilot::GetAltitudeFPS()
{
  if (flightModel)
	return (flightModel->GetAltitudeFPS());
  else
	return (0.0);
}

/*
 * Get pitch, yaw relative to horizon
 * This takes into account roll
 * Also allow offset from horizon
 */
void aiPilot::GetLevelAttitude(sAttitude &att, sREAL offset)
{
  if (!flightModel)
    return;

  sVector   dirNormal;
  sPoint    pos;
  sPoint    horizonPoint;
  sAttitude ignore;    
  sVector   ignore2;
  /* attitude returned by this not what we want */
  GetPositionAndAttitude(pos,ignore);
  flightModel->GetDirectionNormal(dirNormal);
  dirNormal *= 1000.0;
  horizonPoint.x = pos.x + dirNormal.x;
  horizonPoint.y = pos.y + dirNormal.y;
  horizonPoint.z = pos.z + offset;
  /* Get relative pitch, yaw */
  GetAttitude(horizonPoint,ignore2,att); 
}

/*
 * Select the kind of fight we want to do
 */
void aiPilot::SelectEngagementFightType(uint32_t )
{
	tactics.curTactic = sPilotTactics::ANGLES_FIGHT;
	/*
  if (sFlip3SideCoin())
  {
    if (CAN_ENERGY_FIGHT)
      tactics.curTactic = sPilotTactics::ENERGY_FIGHT;
    else if (CAN_ANGLES_FIGHT)
      tactics.curTactic = sPilotTactics::ANGLES_FIGHT;
    else
      tactics.curTactic = sPilotTactics::WEENIE_FIGHT;
  }
  else
  {
    if (CAN_ANGLES_FIGHT)
      tactics.curTactic = sPilotTactics::ANGLES_FIGHT;
    else if (CAN_ENERGY_FIGHT)
      tactics.curTactic = sPilotTactics::ENERGY_FIGHT;
    else
      tactics.curTactic = sPilotTactics::WEENIE_FIGHT;
  }
*/
  if (IS_ENERGY_FIGHT)
    LOG("Energy Fight!");
  else if (IS_ANGLES_FIGHT)
    LOG("Angles Fight!");
  else
    LOG("Weenie Fight!");
}


void aiPilot::SetClearForTakeoff(int clear)
{
	int wasCleared = GetClearForTakeoff();
	clearForTakeoff = clear;
	if (!wasCleared && clear)
		Broadcast(commWINGMAN_RGR_TAKEOFF_RUN);
}

void aiPilot::SetClearForLanding(int clear)
{
	int wasCleared = GetClearForLanding();
	clearForLanding = clear;
	if (!wasCleared && clear)
		Broadcast(commWINGMAN_RGR_FINAL_APPROACH);
}

inline sREAL GET_ROLL_ANGLE(sREAL x, sREAL z)
{
sREAL result;
	z += eps;

	if (x > 0 && z > 0)
		result = (sATAN(x/z));
	else if (x > 0 && z < 0)
		result = C(Pi + atan(x/z));
	else if (x < 0 && z > 0)
		result = (sATAN(x/z));
	else
		result = C(-Pi + atan(x/z));

	return result;
}

/*
 *  Given a point in world coords, calc
 *  roll needed to align lift vector with it
 */
sREAL aiPilot::Point2Roll(const sPoint &rollPosition)
{
sREAL       result;
sVector     mat[3];
sVector     transMat[3];
sPoint      position;
sAttitude   attitude;
sVector     worldVect;
sVector     bodyVect;

	/* get our position and attitude */
	GetPositionAndAttitude(position,attitude);
	/* set roll to 0.0 */
	attitude.roll = 0.0;
	/* build matrix & transpose */
	sMatrix3Build(attitude,mat);
	sMatrix3Transpose(mat,transMat);
	
	worldVect = rollPosition - position;
	/* convert to body coords */
	sMatrix3Rotate(transMat,worldVect,bodyVect);    
	/* get roll needed to align with target */
	result = GET_ROLL_ANGLE(-bodyVect.x,bodyVect.z);
	return result;
}

/*
 *  calc speed needed to close with target
 */
sREAL aiPilot::CalcClosureSpeed(const sTargetGeometry &tg, int flg)
{
sREAL result;
sREAL targetDistance;
sREAL targetRate;
sREAL targetSpeed;
sREAL per;

	if (flg)
	{
		targetDistance = tg.formationRange;
		targetRate = tg.formationDRange;
	}
	else
	{
		targetDistance = tg.range;
		targetRate = tg.dRange;
	}

	targetSpeed = tg.airSpeed;

	per = C(1.0 + (targetClosurePid.p * targetDistance - targetClosurePid.d * targetRate) * targetClosurePid.i);
	result = targetSpeed * per;
	return result;
}

/*******************************************
 * static aiPilot members                   *
 *******************************************/
sObjectArray aiPilot::aiPilots(aiPILOT_MAX_PILOTS,aiPILOT_DEFAULT_OWNERSHIP);
aiPilot *aiPilot::playerPilot = NULL;
uint32_t aiPilot::nextIdx = 0L;
aiPilot *aiPilot::cashedPilot = NULL;

/*******************************************
 * static aiPilot functions                 *
 *******************************************/
/* Add a pilot to the global array */
void aiPilot::AddaiPilot(aiPilot *pilot)
{
	unsigned int index;
	index = aiPilots.Add(pilot);
	pilot->SetIdx(nextIdx++);
	pilot->SetIndex(index);
	if (pilot->IsPlayer())
		playerPilot = pilot;
}

/* 
   Retrieve a pilot from the global array 
   by unique id
*/
aiPilot *aiPilot::GetaiPilot(uint32_t idx)
{
	aiPilot *result = NULL;
	if (cashedPilot != NULL &&	cashedPilot->GetIdx() == idx)
		result = cashedPilot;
	else
	{
		unsigned int i = idx;
		if (i < GetPilotCount())
		{
			aiPilot *pil = static_cast<aiPilot *>(aiPilots[i]);
			if (pil != NULL && pil->GetIdx() == idx)
				result = pil;
		}
	}
	if (result == NULL)
	{
		for (unsigned int i=0;i<GetPilotCount();i++)
		{
			aiPilot *pil = static_cast<aiPilot *>(aiPilots[i]);
			if (pil != NULL && pil->GetIdx() == idx)
			{
				result = pil;
				break;
			}
		}
	}
	if (result != NULL)
		cashedPilot = result;
	return result;
}

/*
  Retrieve a pilot from the global array
  by handle
 */
aiPilot *aiPilot::GetaiPilot(const char *handle)
{
	aiPilot *result =NULL;
	for (unsigned int i=0;i<GetPilotCount();i++)
	{
		aiPilot *pil = GetPilotByIndex(i);
		if (pil && !strcmp(handle,pil->GetHandle()))
		{
			result = pil;
			break;
		}
	}
	return (result);
}

unsigned int aiPilot::GetPilotCount()
{
	return (aiPilots.Count());
}

void aiPilot::FlushaiPilots()
{
  aiPilots.Flush();
  playerPilot = NULL;
  cashedPilot = NULL;
  nextIdx = 0L;
}

void aiPilot::RemoveaiPilot(aiPilot *pil)
{
	for (unsigned int i=0;i<aiPilots.Count();i++)
	{
		aiPilot *ppil = static_cast<aiPilot *>(aiPilots[i]);
		if (ppil != NULL && ppil == pil)
			aiPilots.Remove(i);
	}
	if (pil == cashedPilot)
		cashedPilot = NULL;
}

aiPilot *aiPilot::GetPilotByIndex(unsigned int i)
{
	if (i < aiPilots.Count())
		return static_cast<aiPilot *>(aiPilots[i]);
	else
		return NULL;
}

void aiPilot::BodyVector2WorldVector(unsigned int idx, const sVector &body, sVector &world)
{
aiPilot *pilot = GetaiPilot(idx);

	if (pilot)
		pilot->BodyVector2WorldVector(body,world);
}

void aiPilot::WorldVector2BodyVector(unsigned int idx, const sVector &world, sVector &body)
{
aiPilot *pilot = GetaiPilot(idx);

	if (pilot)
		pilot->WorldVector2BodyVector(world,body);

}

void aiPilot::BodyPoint2WorldPoint(unsigned int idx, const sPoint &body, sPoint &world)
{
aiPilot *pilot = GetaiPilot(idx);

	if (pilot)
		pilot->BodyPoint2WorldPoint(body,world);

}

void aiPilot::WorldPoint2BodyPoint(unsigned int idx, const sPoint &world, sPoint &body)
{
aiPilot *pilot = GetaiPilot(idx);
	
	if (pilot)
		pilot->WorldPoint2BodyPoint(world,body);
}

/*
	static function which is called by sFlightModel.
	Control is directed to the pilot instance by
	casting the void * to an aiPilot *
 */
void aiPilot::aiPilotUpdateCallback(int sw, void *data)
{
	/* cast to an aiPilot */
	aiPilot *thePilot = static_cast<aiPilot *>(data);
	/* call the aiPilot instance callback function */
	if (thePilot)
		thePilot->UpdateCallback(sw);
}


