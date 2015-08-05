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
 * File   : swaypnt.h                            *
 * Date   : April, 1998                          *
 * Author : Dan Hammer                           *
 *************************************************/
#ifndef SWAYPNT_H
#define SWAYPNT_H

typedef struct tag_swaypoint_info
{
	float		to_x;
	float		to_y;
	float		to_z;
	float		speed;
	float		altitude;
	int		task;
	int		i_task_modifier[3];
	float		d_task_modifier[3];
	char		c_task_modifier[32];
	int		formation_type;
	double	time;
} swaypoint_info;

enum	{	
			swpNAVIGATE, swpCAP, swpINTERCEPT, swpENGAGE_PLAYER, 
			swpENGAGE_FLITE, swpSTRAFE_ATTACK, swpROCKET_ATTACK, 
			swpLEVEL_BOMB, swpDIVE_BOMB, swpTORPEDO_BOMB,
			swpAIRSHOW, swpTARGETS, swpTAKEOFF, swpLAND, swpMANEUVER  
		};

enum	{
			swpTARGET_LEVEL0, swpTARGET_LEVEL1, swpTARGET_LEVEL2 
		};

#ifdef __cplusplus
#include <string.h>

#include "sobject.h"
#include "smath.h"

inline void swaypoint_info_copy(swaypoint_info *wp0,
							   swaypoint_info *wp1)
{
	wp0->to_x = wp1->to_x;
	wp0->to_y = wp1->to_y;
	wp0->to_z = wp1->to_z;
	wp0->speed = wp1->speed;
	wp0->altitude = wp1->altitude;
	wp0->task = wp1->task;
	wp0->time = wp1->time;
	wp0->formation_type = wp1->formation_type;
	wp0->i_task_modifier[0] = wp1->i_task_modifier[0];
	wp0->i_task_modifier[1] = wp1->i_task_modifier[1];
	wp0->i_task_modifier[2] = wp1->i_task_modifier[2];
	wp0->d_task_modifier[0] = wp1->d_task_modifier[0];
	wp0->d_task_modifier[1] = wp1->d_task_modifier[1];
	wp0->d_task_modifier[2] = wp1->d_task_modifier[2];
	memcpy(wp0->c_task_modifier,wp1->c_task_modifier,
		sizeof(wp0->c_task_modifier));
}

/* Waypoint definition -- used by flite leader */
class sWaypoint : public sObject
{
public:
	sPoint		to;
	sREAL			speed;
	sREAL			altitude;
	int			task;
	int         iTaskModifier[3];
	sREAL			dTaskModifier[3];
   char			cTaskModifier[32];
	int			idx;
	int			formationType;
	double		time;
	sWaypoint   *next;

	sWaypoint()
	{
		next = NULL;
		idx = 0;
		formationType = 0;
	}

	void Copy(const sWaypoint &wp)
	{
		to = wp.to;
		speed = wp.speed;
		altitude = wp.altitude;
		task = wp.task;
		time = wp.time;
		iTaskModifier[0] = wp.iTaskModifier[0];
		iTaskModifier[1] = wp.iTaskModifier[1];
		iTaskModifier[2] = wp.iTaskModifier[2];
		dTaskModifier[0] = wp.dTaskModifier[0];
		dTaskModifier[1] = wp.dTaskModifier[1];
		dTaskModifier[2] = wp.dTaskModifier[2];
		memcpy(cTaskModifier,wp.cTaskModifier,sizeof(cTaskModifier));
		next = wp.next;
		idx = wp.idx;
		formationType = wp.formationType;
	}

	sWaypoint(const sWaypoint &wp)
	{
		Copy(wp);
	}

	sWaypoint &operator =(const sWaypoint &wp)
	{
		Copy(wp);
		return (*this);
	}

	void Copy(const swaypoint_info &inf)
	{
		to = sPoint(inf.to_x,inf.to_y,inf.to_z);
		speed = inf.speed;
		altitude = inf.altitude;
		task = inf.task;
		time = inf.time;
		iTaskModifier[0] = inf.i_task_modifier[0];
		iTaskModifier[1] = inf.i_task_modifier[1];
		iTaskModifier[2] = inf.i_task_modifier[2];
		dTaskModifier[0] = inf.d_task_modifier[0];
		dTaskModifier[1] = inf.d_task_modifier[1];
		dTaskModifier[2] = inf.d_task_modifier[2];
		memcpy(cTaskModifier,inf.c_task_modifier,sizeof(cTaskModifier));
		formationType = inf.formation_type;

	}

	sWaypoint(const swaypoint_info &inf)
	{
		Copy(inf);
	}

	sWaypoint &operator =(const swaypoint_info &inf)
	{
		Copy(inf);
		return (*this);
	}

	const char *GetFieldId()
	{
		return cTaskModifier;
	}

	int GetRunwayNo()
	{
		return iTaskModifier[0];
	}
	int GetManeuverType()
	{
		return iTaskModifier[0];
	}
	int GetManeuverFlags()
	{
		return iTaskModifier[1];
	}
	sREAL GetManeuverData0()
	{
		return dTaskModifier[0];
	}
	sREAL GetManeuverData1()
	{
		return dTaskModifier[1];
	}
	sREAL GetManeuverData2()
	{
		return dTaskModifier[2];
	}
	virtual ~sWaypoint();
};

/*
 *  Navigation info
 */
class sNavInfo
{
public:
	sPoint		navPoint;			/* location in world */
	sVector		worldDirection;	/* direction from us in world coords */
	sAttitude	relAttitude;		/* pitch, yaw relative to us (body coords) */
	sAttitude	attitude;			/* pitch, yaw in world coords */
	sREAL			range;				/* current distance */
	sREAL			dRange;				/* approach rate */
	sWaypoint	waypoint;			/* waypoint info */
	sNavInfo()
	{
		range = dRange = 0.0;
	}
	virtual ~sNavInfo();
};

#endif /* __cplusplus */
#endif
