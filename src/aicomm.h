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
 * File   : aicomm.h                             *
 * Date   : November, 1998                       *
 * Author : Dan Hammer                           *
 * AI "communications"                           *
 *************************************************/
#ifndef AICOMM_H
#define AICOMM_H

// aiPilot broadcast enums

enum { commPRIORITY_STANDARD, commPRIORITY_LOW, commPRIORITY_HIGH };
enum { commCHANNEL_AFFILIATION, commCHANNEL_SQUAD,  commCHANNEL_ALL, commCHANNEL_PRIVATE };
enum { 
			commWINGMAN_ENGAGING, 
			commWINGMAN_TARGETING,
			commWINGMAN_KILL, 
			commWINGMAN_AUGERED_IN,                                                     
         commWINGMAN_GOODKILL,
			commWINGMAN_IMHIT,
			commWINGMAN_EJECTING,
			commWINGMAN_BANDIT,
			commWINGMAN_FORMING,
			commWINGMAN_RGR_TAXI,
			commWINGMAN_RGR_TAKEOFF_RUN,
			commWINGMAN_RGR_FINAL_APPROACH,
			commWINGMAN_RGR_DISENGAGE,
			commWINGMAN_GUYS_ASLEEP,
			commWINGMAN_GUYS_NO_AMMO,
			commWINGMAN_GUYS_FIGHTING,
			commWINGMAN_NOBODY_ATTACK,
			commWINGMAN_ENGAGING_SURFACE
};




#endif
