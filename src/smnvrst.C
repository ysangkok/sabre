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
 * File   : smnvrst.cpp                          *
 * Date   : April, 1998                          *
 * Author : Dan Hammer                           *
 *************************************************/
#include "smnvrst.h"

const char *sManeuver::maneuver2string[] =
{
	"FIRST",
	"STRAIGHT_AND_LEVEL",
	"STANDARD_TURN",
	"CLIMB",
	"DESCEND",
	"BREAK_TURN",
	"HARD_TURN",
	"IMMELMAN",
	"SPLIT_S",
	"INVERT",
	"EXTEND",
	"JINK",
	"ENGAGE",
	"ALIGN",
	"RQ_EVADE",
	"RQ_ENGAGE",
	"FQ_ENGAGE",
	"PURSUIT",
	"GUN_ATTACK",
	"NAVIGATE",
	"FORMATION",
	"PULLUP",
	"LEVEL_ROLL",
	"PITCHED_ROLL",
	"WINGOVER",
	"LEVEL_BOMB",
	"INTERCEPT",
	"CLIMBING_TURN",
	"DESCENDING_TURN",
	"SNAP_ROLL",
	"CHANDELLE",
	"AIRSHOW",
	"AILERON_ROLL",
	"EVADE",
	"ZOOM",
	"BARREL_ROLL",
	"SPIRAL_DIVE",
	"TAKEOFF",
	"LAND",
	"TAXI",
	"LAST"
};

const char *sManeuver::Maneuver2String(unsigned int mn)
{
	if (mn > LAST)
		mn = LAST;
	return (maneuver2string[mn]);
}

int sManeuverState::GetManeuverDirection()
{
	if (IMNVR_DIR(flags))
		return (sManeuver::RIGHT);
	else
		return (sManeuver::LEFT);
}
