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
 * File   : sftlmdl.C                            *
 * Date   : Apirl, 1998                          *
 * Author : Dan Hammer                           *
 *************************************************/
#include "sfltmdl.h"
 
/*************************************************
 * Call back prior to final calc of vector forces*
 *************************************************/
void sFlightModel::DoVForcesCallback()
{
  if (
      (updateCallback != NULL ) &&
      ( callbackFlags & SFMCB_VFORCES_UPDATE) 
      )
    (*updateCallback)(VFORCES_UPDATE,callbackData);
}

/************************************************
 * Call back prior to final calc of rotational  *
 * forces                                       *
 ************************************************/
void sFlightModel::DoRForcesCallback()
{
  if (
      (updateCallback != NULL ) &&
      ( callbackFlags & SFMCB_RFORCES_UPDATE) 
      )
    (*updateCallback)(RFORCES_UPDATE,callbackData);
}

/**********************************************
 * Call back prior to calc of new position    *
 * based on current vector forces             *
 **********************************************/
void sFlightModel::DoPositionCallback()
{
  if (
      (updateCallback != NULL ) &&
      ( callbackFlags & SFMCB_POSITION_UPDATE) 
      )
    (*updateCallback)(POSITION_UPDATE,callbackData);
}

/**********************************************
 * Call back priot to calc of new orientation *
 * based on current rotational forces         *
 **********************************************/
void sFlightModel::DoOrientationCallback()
{
  if (
      (updateCallback != NULL ) &&
      ( callbackFlags & SFMCB_ORIENTATION_UPDATE) 
      )
    (*updateCallback)(ORIENTATION_UPDATE,callbackData);
}

/*********************************************
 * Call back for setting of flight controls  *
 *********************************************/
void sFlightModel::DoControlCallback()
{
  if (
      (updateCallback != NULL ) &&
      ( callbackFlags & SFMCB_CONTROL_UPDATE) 
      )
    (*updateCallback)(CONTROL_UPDATE,callbackData);
}

sFlightModel::~sFlightModel() = default;
