/*
    Sabre Fighter Plane Simulator 
    Copyright (c) 1997 Dan Hammer
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
 *           Sabre Fighter Plane Simulator       *
 * File   : pilot.C                              *
 * Date   : March, 1997                          *
 * Author : Dan Hammer                           *
 * Artificial Dumbs                              *
 * Most of the dumbs are now in aipilot*.C. This *
 * class basically ties in that code to the      *
 * rest of the system.                           *
 *************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <values.h>
#include <stdarg.h>
#include "defs.h"
#include "sim.h"
#include "simfile.h"
#include "simsnd.h"
#include "vmath.h"
#include "port_3d.h"
#include "cpoly.h"
#include "copoly.h"
#include "flight.h"
#include "fltlite.h"
#include "weapons.h"
#include "unguided.h"
#include "aimath.h"
#include "pilot.h"
#include "aipildef.h"
#include "rtkey.h"

#define sbrPRIORITY_REGULAR 5
#define sbrPRIORITY_LOW     0
#define sbrPRIORITY_HIGH	10

#define FLTPORT flight->state.flight_port

/*
 *  Don't yack up the guns too much
 */
float Pilot::gunsGunsSoundTime = 0.0;
#define GUNSGUNSPAUSE 2.5

/*
 *  Update any static members
 */
void Pilot::UpdateStatics(void)
{
	gunsGunsSoundTime -= time_frame;
}

Pilot::Pilot(Flight *flt, Pilot_Params *params,
				Weapon_Instance *weapons, int n_weaps,
				char *hndl, Target *target_obj)
  : aiPilot(&sbrFlight),
    sbrFlight(flt)
{
	this->params = params;
	this->weapons = weapons;
	this->n_weaps = n_weaps;
	this->target_obj = target_obj;
	SetAffiliation(params->affiliation);
	SetHandle(hndl);
	SetTask(NOTHING);
	AddaiPilot(this);

	target_flight = NULL;
	target_pilot = NULL;
	target = NULL;
	flight = flt;
	sel_wpn = 0;
	sel_weapon = &weapons[0];
	message.time_limit = 3.0;
	new_damage = 0.0;

	if (!flight->controls.autopilot)
	{
		if (!IsPlayer())
			ClearManeuverStack();
		SetAsPlayer(1);
	}
	else
		SetAsPlayer(0);

	/*
	 * See if we are carrying
	 * stores which we can later
	 * drop dramatically when engaged
	 */
	nstores = 0;
	for (int i=0;i<n_weaps;i++)
	{
		if (weapons[i].getType() == fueltank_t)
		{
			if (nstores < sbrPIL_MAX_STORES)
			{
				stores[nstores] = i;
				nstores++;
			}
			else
				break;
		}
	}
}

void Pilot::start()
{
}

void Pilot::init()
{
	functioning = 1;
	new_damage = 0.0;
	time_to_target = 0.0;
	if (params->log)
		OpenLog(NULL);
	Init();
	pitchPid = flight->specs->pitchPid;
	rollPid = flight->specs->rollPid;
	yawPid = flight->specs->yawPid;
	gPid = flight->specs->gPid;
	airSpeedPid = flight->specs->airSpeedPid;
	targetClosurePid = flight->specs->targetClosurePid;
	SetTask(NOTHING);
	SetManeuver(sManeuver::STRAIGHT_AND_LEVEL,IMNVR_LOOPBIT);
	grndColSecs = 5.0;
	posGLimit = params->posG;
	negGLimit = params->negG;
	immelmanGs = posGLimit;
	splitSGs = posGLimit;
	tactics.altThreshold = 2000.0;
	formationWingLen = flight->specs->formationWingLen;
	formationOffset = flight->specs->formationOffset;
}

void Pilot::pause(void)
{

}

void Pilot::update(int aiActive)
{
	// Get elapsed time in microseconds
	t = time_frame;

	// aiActive == 0 means we're not on
	// "autopilot" so just update
	// weapons, navigation & communications info ..
	// we are the player
	if (!aiActive)
	{
		SetRemoteControl(1);
		SetAsPlayer(1);
	}
	else
	{
		SetRemoteControl(0);
		SetAsPlayer(0);
	}

	/*
	 * Select target for player if 
	 * he has not
	 */
	if (!aiActive && task == ENGAGE_TARGET && TARGET_ACTIVE && target_pilot == NULL)
	{
		aiPilot *pil = GetaiPilot(engageTarget.idx);
		if (pil)
			set_target((Pilot *)pil);
	}

	// Do some special tweaking based on task
	switch (task)
	{
	case FORMATION_FLY:
		{
			aiPilot *pil = GetaiPilot(formationTarget.idx);
			if (pil != NULL)
				set_target((Pilot *)pil);
			if (!aiActive)
				GetTargetGeometry(formationTarget.idx,formationTarget.geometry);
			flight->controls.armed_w = 0;
			break;
		}
	default:
		if (!aiActive)
		{
			if (target_pilot != NULL)
				GetTargetGeometry(target_pilot->GetIdx(),engageTarget.geometry);		
			else
				gun_point = sel_weapon->predict_path(*flight,1.0);
			GetWeaponLimits(weaponLimits);
		}
		break;
	}

	if (aiActive)
	{
		if (task == ENGAGE_TARGET && TARGET_ACTIVE)
		{
			selWeapon(0);
			/*
			 *  Dramatically drop stores if engaged
			 */
			if (nstores > 0)
			{
				flight->controls.bang_bang = 1;
				weapons[stores[nstores-1]].update(*flight,Unguided_Manager::the_umanager,
															 target_obj);
				if (weapons[stores[nstores-1]].rounds_remaining <= 0)
					nstores--;
				flight->controls.bang_bang = 0;
			}
			flight->controls.armed_w = 1;
			aiPilot *pil = GetaiPilot(engageTarget.idx);
			if (pil)
				set_target((Pilot *)pil);
		}
		else
			flight->controls.armed_w = 0;
		flight->controls.auto_coord = 1;
	}
	else
		flight->controls.auto_coord = 0;
	message.update();
	damage_check();

	Update(t);
}

/**************************************************************
 * Take a good hard look at how much punishment we've taken,  *
 * and get out if it's just too much to take!                 *
 **************************************************************/
int Pilot::damage_check()
{
	if (flight->mods.battle_damage > 0)
	{
		if (flight->mods.battle_damage >= flight->specs->max_damage)
		{
			if (functioning)
			{
				broadcast("Ejecting!",6,getAffiliation(),NULL);
				sound_off("takingfire");
				sound_off("takingflak");
				sound_off("heavydamage");
				sound_on("eject",getAffiliation());
				dbg = "EJECT";
				functioning = 0;
				set_target(NULL);
				SetDeath(1);
			}
			return (1);
		}
		else
		{
			if (flight->mods.battle_damage > new_damage)
			{
				new_damage = flight->mods.battle_damage;
				float d1 = ((float)new_damage) / ((float)flight->specs->max_damage);
				if (target_obj->who_got_me != NULL && target_obj->who_got_me->getType() == GROUND_UNIT_T)
				{
					brdcst(sbrPRIORITY_REGULAR,getAffiliation(),"Taking flak: %3.0f%%%%",d1 * 100.0);
					sound_on("takingflak",getAffiliation());
				}
				else
				{
					if (d1 >= 0.75)
					{
						brdcst(sbrPRIORITY_REGULAR,getAffiliation(),"Heavy damage: %3.0f%%%%!",d1 * 100.0);
						sound_on("heavydamage",getAffiliation());
					}
					else
					{
						brdcst(sbrPRIORITY_REGULAR,getAffiliation(),"Taking fire %3.0f%%%%",d1 * 100.0);
						sound_on("takingfire",getAffiliation());
					}
				}
			}
		}
	}
	return (0);
}

void Pilot::set_target(Pilot *pl)
{
	target_pilot = pl;
	if (target_pilot != NULL)
	{
		target = pl->get_target_obj();
		target_flight = pl->get_flight();
	}
	else
	{
		target = NULL;
		target_flight = NULL;
	}
}


int Pilot::broadcast(char *mss, int priority, int freq, char *who)
{
int result = 0;

	for (int i=0;i<npilots;i++)
	{
		Pilot *pl = pilots[i];
		if ((freq != -1) && (pl->getAffiliation() != freq))
			continue;
		if (pl == this && IsPlayer())
			continue;
		if (who != NULL && strcmp(pl->handle,who))
			continue;
		if (pl->message.set_message(mss,priority,this))
			result++;
	}
	return result;
}

int __cdecl Pilot::brdcst(int priority, int freq, char *mss, ...)
{
static char mssbuff[200];
	va_list ap;
	va_start(ap,mss);
	vsprintf(mssbuff,mss,ap);
	va_end(ap);
	MYCHECK(strlen(mssbuff) <= 127);
	return (broadcast(mssbuff,priority,freq));
}

void Pilot::selectNextWeapon(int dir)
{
  sel_wpn += dir;
  if (sel_wpn >= n_weaps)
    sel_wpn = 0;
  else if (sel_wpn < 0)
    sel_wpn = n_weaps - 1 ;
  sel_weapon = &weapons[sel_wpn];
}

void Pilot::selWeapon(int n)
{
	if (n >= 0 && n < n_weaps)
	{
		sel_wpn = n;
		sel_weapon = &weapons[sel_wpn];
	}
}


void Pilot::sound_on(const char *soundId, int affiliation)
{
	if ((affiliation >= 0 && (sound_get_affiliation() != affiliation)) ||
		IsPlayer())
		return;
	::sound_on(soundId,NORM);
}


const char *Pilot::buildSoundId(char *suffix, Flight *targetFlight)
{
	static char soundId[64];
	sprintf(soundId,"%s%s",targetFlight->specs->model,suffix);
	return soundId;
}



Pilot *Pilot::getPilot(char *hndl)
{
	Pilot *result = NULL;
	for (int i=0;i<npilots;i++)
	{
		if (!strcmp(hndl,pilots[i]->handle))
		{
			result = pilots[i];
			break;
		}
	}
	return (result);
}


void Pilot::BuildGunners(void)
{

}


void Pilot::GetWeaponLimits(sWeaponLimits &weaponLimits)
{
FlightLight_Specs *flspecs;

	if (!sel_weapon)
		return;

	flspecs = WPSPECS(sel_weapon)->getFltSpecs();
	weaponLimits.initialVelocity = flspecs->init_speed;
	weaponLimits.maxRange = flspecs->init_speed;
	gunAttackDistanceMul = 1.0;
	gunAttackDistance = WPSPECS(sel_weapon)->max_range;
	weaponLimits.jiggle.x = weaponLimits.jiggle.y = weaponLimits.jiggle.z = 0.0;
	weaponLimits.burstTime = 0.4 + sRandPer() * 0.4;
	weaponLimits.burstPauseTime = 0.1 + sRandPer() * 0.2;
	weaponLimits.cone.pitch = weaponLimits.cone.yaw = params->shootRadius;

	if (IsPlayer())
		sel_weapon->hitScaler = player_hit_scaler;
	else
		sel_weapon->hitScaler = params->hitScaler;
}

void Pilot::DropBomb()
{

}

void Pilot::GetRemoteControlInputs()
{

}

void Pilot::CalcGunLeadPoint(sTargetGeometry &tg)
{
REAL_TYPE t;
sVector   temp;
sPoint    leadPoint;
sPoint    ourPosition;
sPoint	 gunPoint;
sAttitude ourAtt;
sAttitude gunPointAtt;
sAttitude leadPointAtt;

	in_range = 0;
	GetPositionAndAttitude(ourPosition,ourAtt);
	if (!target)
		t = 1.0;
	else
	{
		time_to_target = sel_weapon->calc_hvtime(*flight,*target->get_position());
		if (tg.range <= gunAttackDistance)
		{
			in_range = 1;
			t = time_to_target;
		}
		else
			t = 1.0;
	}
	gun_point = sel_weapon->predict_path(*flight,t);
	R_3DPoint2sPoint(gun_point,gunPoint);
	GetAttitude(gunPoint,temp,gunPointAtt);

	sPoint2R_3DPoint(tg.worldPosition,sel_weapon->target_position);
	
	temp = tg.worldVelocity;
	temp *= t;
	leadPoint = tg.worldPosition + temp;
 	tg.gunLeadPoint = leadPoint;
	GetAttitude(leadPoint,temp,leadPointAtt);

	tg.gunLeadAttitude.pitch = leadPointAtt.pitch - gunPointAtt.pitch;
	tg.gunLeadAttitude.yaw = leadPointAtt.yaw - gunPointAtt.yaw;
	tg.gunLeadRangeSq = sDistanceSquared(tg.gunLeadPoint,ourPosition);
	tg.gunTOF = time_to_target;
	sPoint2R_3DPoint(tg.worldPosition,track_point);
	sPoint2R_3DPoint(tg.leadPoint,lead_track_point);
	sPoint2R_3DPoint(tg.gunLeadPoint,gun_track_point);
}


void Pilot::Shoot()
{
	selWeapon(0);
	if (InShootParams())
	{
		flight->controls.bang_bang = 1;
		broadcast("Guns! Guns!",1,getAffiliation(),NULL);
		/*
		 *  Don't yack up the "gunsguns" sound too much
		 */
		if (gunsGunsSoundTime <= 0.0)
		{
			sound_on("gunsguns",getAffiliation());
			gunsGunsSoundTime = sRandPer() * GUNSGUNSPAUSE + 0.2;
		}
	}
}

void Pilot::Broadcast(int idx, int channel, int priority, void *extraInfo)
{
int priorityValue;
int affil;

	switch (priority)
	{
	case commPRIORITY_STANDARD:
	default:
		priorityValue = sbrPRIORITY_REGULAR;
		break;

	case commPRIORITY_LOW:
		priorityValue = sbrPRIORITY_LOW;
		break;

	case commPRIORITY_HIGH:
		priorityValue = sbrPRIORITY_HIGH;
		break;
	}

	switch (channel)
	{
	case commCHANNEL_AFFILIATION:
	case commCHANNEL_SQUAD:
	default:
		affil = GetAffiliation();
		break;

	case commCHANNEL_ALL:
		affil = -1;
		break;
	}

	switch (idx)
	{

	case commWINGMAN_KILL:
		{
			aiPilot *targetPilot = (aiPilot *)extraInfo;
			if (targetPilot)
			{
				brdcst(priorityValue,affil,"Kill on %s @ Angels %d",
								targetPilot->GetModel(),
								(int) ANGELS(targetPilot));
				Flight *targetFlight = ((Pilot *)targetPilot)->get_flight();
				sound_on(buildSoundId("kill",targetFlight),affil);
			}
		}
		break;

	case commWINGMAN_ENGAGING:
		{
			aiPilot *targetPilot = (aiPilot *)extraInfo;
			if (targetPilot)
			{
				brdcst(priorityValue,affil,"Engaging %s @ Angels %d",
								targetPilot->GetModel(),
								(int) ANGELS(targetPilot));
				Flight *targetFlight = ((Pilot *)targetPilot)->get_flight();
				sound_on(buildSoundId("engage",targetFlight),affil);
			}
		}
		break;
	}
}

/*****************************************
 Note limit of 64 pilots                 *
 *****************************************/   
#define MAXPILOTS 64
Pilot *Pilot::pilots[MAXPILOTS];
int Pilot::npilots = 0;
int Pilot::maxpilots = MAXPILOTS;


/**********************************************************
 * initialize static members for Pilot class              *
 **********************************************************/
int Pilot::initPilot()
{
	// do nothing now
  return (0);
}

