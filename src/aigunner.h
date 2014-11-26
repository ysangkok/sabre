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
 * File   : aigunner.h                           *
 * Date   : April, 1998                          *
 * Author : Dan Hammer                           *
 * aiGunner class definition                     *
 *************************************************/
#ifndef __aigunner_h
#define __aigunner_h

#define aiGUN_PITCH_RATE      (static_cast<sREAL>(_degree * 20.0))
#define aiGUN_YAW_RATE        (static_cast<sREAL>(_degree * 20.0))

#ifdef __cplusplus
#include "fstream"
#include "sobject.h"
#include "sarray.h"
#include "sfltmdl.h"
#include "scntrl.h"
#include "starget.h"
#include "sweapon.h"
#include "smnvrst.h"
#include "stact.h"
#include "swaypnt.h"
#include "sattkr.h"

class aiGunner: public sObject
{
protected:
	/********************************************************************
	* protected members                                                 *
	*********************************************************************/
	int				index;					/* index                        */
	uint32_t	ownerIdx;				/* owner's idx						  */
	int				affiliation;			/* whose side                   */
	sVector			offset;					/* offset relative to 0,0,0 in  */
													/*  plane's body coords         */
	sFlightModel	*flightModel;			/* flight model pointer         */
	sPoint			worldPosition;			/* where am I in world          */
	int				isDead;					/* 1 == dead                    */
	int				isEjected;				/* 1 == ejected                 */
	int				hasTarget;				/* 1 == has a target            */
	sTarget			target;					/* current target               */
	sWeaponLimits	weaponLimits;			/* current weapon information   */
	sREAL				bulletRadius;			/* marksmanship factor          */
	sREAL				timeFrame;				/* time between frames          */
	sAttitude		gunAttitude;			/* position of gun              */
	sREAL				gunPitchRate;			/* rate at which gun can change pitch */
	sREAL				gunYawRate;				/*   same for yaw               */
	sAttitude		jiggleAtt;				/* gun jiggling data            */

	virtual void	GetTargetGeometry();
	virtual int		Shoot();
	virtual void	GetAttitude(const sPoint &position, 
								sVector &direction, 
								sAttitude &attitude) = 0;
	virtual void	GetWeaponLimits(sWeaponLimits &weaponLimits) = 0;
	void				AdjustGun();
public:
	aiGunner();
	aiGunner(sFlightModel *fltModel, int index,
			uint32_t ownerIdx, int affiliation, sREAL bulletRadius,
			sREAL gunPitchRate = aiGUN_PITCH_RATE,
			sREAL gunYawRate = aiGUN_YAW_RATE,
			sAttitude *jiggleAtt = NULL );
	virtual ~aiGunner()
	{}
	virtual void Init();
	virtual void Update(double frame_time);
	virtual void Destroy();

	int HasTarget()
	{
		return (hasTarget);
	}
	void  SetTarget(uint32_t targetIdx);
	void  UnsetTarget()
	{
		hasTarget = 0;
	}
	uint32_t GetTargetIdx();
	int GetAffiliation()
	{
		return (affiliation);
	}
	void SetAffiliation(int affiliatio)
	{
		this->affiliation = affiliatio;
	}
	int GetIndex()
	{
		return (index);
	}
	void SetIndex(int idx)
	{
		this->index = idx;
	}
	uint32_t GetOwnerIdx()
	{
		return (ownerIdx);
	}
	void SetOwnerIdx(uint32_t ownIdx)
	{
		this->ownerIdx = ownIdx;
	}
	int IsDead()
	{
		return (isDead);
	}
	void SetDeath(int death)
	{
		isDead = death;
	}
	int IsEjected()
	{
		return (isEjected);
	}
	void SetEjected(int ejected)
	{
		this->isEjected = ejected;
	}
	int IsActive()
	{
		return (!(IsDead() || IsEjected()));
	}
	void SetBulletRadius(long bulletRadiu)
	{
		this->bulletRadius = bulletRadiu;
	}
	long GetBulletRadius()
	{
		return (static_cast<long>(bulletRadius));
	}
	void SetFlightModel(sFlightModel *flightMdl)
	{
		this->flightModel = flightMdl;
	}
	void SetOffset(const sVector &ofst)
	{
		this->offset = ofst;
	}
	void SetWorldPosition(const sPoint &worldPos)
	{
		this->worldPosition = worldPos;
	}
	virtual void DoHUD() = 0;
	sREAL GetTargetThreatValue()
	{
		return target.threatValue;	
	}
	void SetTargetThreatValue(sREAL threatValue)
	{
		target.threatValue = threatValue;
	}

	friend class aiPilot;
	friend class aiFlite;
};
#endif
#endif
