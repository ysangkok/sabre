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
 * File   : smnvrst.h                            *
 * Date   : April, 1998                          *
 * Author : Dan Hammer                           *
 * Object for maneuver stack                     *
 *************************************************/
#ifndef __smnvrst_h
#define __smnvrst_h

typedef struct maneuver_st_type
{
  int type;
  unsigned long flags;
  float data0;
  float data1;
  float data2;
} maneuver_str;

#ifdef __cplusplus /* don't let "C" compiler see this */

#include "sobject.h"
#include "smath.h"

class sManeuver
{
public:
enum {
		STRAIGHT_AND_LEVEL = 1, // 1
		STANDARD_TURN,          // 2
		CLIMB,                  // 3
		DESCEND,                // 4
		BREAK_TURN,					// 5
		HARD_TURN,					// 6
		IMMELMAN,               // 7
		SPLIT_S,                // 8
		INVERT,                 // 9
		EXTEND,                 // 10
		JINK,                   // 11
		ENGAGE,                 // 12
		ALIGN,						// 13
		RQ_EVADE,					// 14
		RQ_ENGAGE,					// 15
		FQ_ENGAGE,					// 16
		PURSUIT,						// 17
		GUN_ATTACK,					// 18
		NAVIGATE,					// 19
		FORMATION,					// 20
		PULLUP,						// 21
		LEVEL_ROLL,					// 22
		PITCHED_ROLL,				// 23
		WINGOVER,					// 24
		LEVEL_BOMB,					// 25
		INTERCEPT,					// 26
		CLIMBING_TURN,				// 27
		DESCENDING_TURN,			// 28
		SNAP_ROLL,					// 29
		CHANDELLE,					// 30          
		AIRSHOW,						// 31
		AILERON_ROLL,				// 32
		EVADE,						// 33
		ZOOM,							// 34
		BARREL_ROLL,				// 35
		SPIRAL_DIVE,				//	36
		TAKEOFF,						// 37
		LAND,							// 38
		TAXI,							// 39
		LAST
	};

	enum { LEFT, RIGHT };

	static char *maneuver2string[];
	static char *Maneuver2String(int);
};

#define IMNVR_DIRBIT   0x01
#define IMNVR_LOOPBIT  0x02
#define IMNVR_FLAG0BIT 0x04
#define IMNVR_FLAG1BIT 0x08
#define IMNVR_FLAG2BIT 0x10
#define IMNVR_LEFT     0x00
#define IMNVR_RIGHT    0x01

#define IMNVR_DIR(a) ((int)((a) & IMNVR_DIRBIT))

class sManeuverState
{
public:
	int maneuver;
	int state;
	int done;
	int  stackLevel;
	unsigned long flags;
	sREAL data0;
	sREAL data1;
	sREAL data2;
	sREAL data3;
	sREAL data4;
	sREAL data5;

	sManeuverState()
	{
		maneuver = 0;
		state = 0;
		flags = 0;
		data0 = data1 = data2 = 0.0;
		data3 = data4 = data5 = 0.0;
		done = 0;
	}

	friend void SETMANEUVER(sManeuverState &ms,
								   int maneuver,
								   int stackLevel,
								   unsigned long flgs = 0,
								   sREAL d0 = 0.0,
								   sREAL d1 = 0.0,
								   sREAL d2 = 0.0)
	{
		ms.maneuver = maneuver;
		ms.stackLevel = stackLevel;
		ms.flags = flgs;
		ms.data0 = d0;
		ms.data1 = d1;
		ms.data2 = d2;
		ms.state = 0;
		ms.done = 0;
	}

	friend void SETMANEUVERX(sManeuverState &ms,
								   int maneuver,
								   int stackLevel,
								   unsigned long flgs = 0,
								   sREAL d0 = 0.0,
								   sREAL d1 = 0.0,
								   sREAL d2 = 0.0,
                           sREAL d3 = 0.0,
                           sREAL d4 = 0.0,
                           sREAL d5 = 0.0)
	{
		ms.maneuver = maneuver;
		ms.stackLevel = stackLevel;
		ms.flags = flgs;
		ms.data0 = d0;
		ms.data1 = d1;
		ms.data2 = d2;
		ms.data3 = d3;
		ms.data4 = d4;
		ms.data5 = d5;
		ms.done = 0;
	}

	int GetManeuverDirection();
	int IsLoop()
	{
		return (((int)(flags & IMNVR_LOOPBIT)) != 0);
	}
	char *GetManeuverString()
	{
		return (sManeuver::Maneuver2String(maneuver));
	}
};

#endif /* __cplusplus */
#endif
