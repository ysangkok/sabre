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
 * File   : scntrl.h                             *
 * Date   : November, 1998                       *
 * Author : Dan Hammer                           *
 * AI flight model control class                 *
 *************************************************/
#ifndef __scntrl_h
#define __scntrl_h

#ifdef __cplusplus /* don't let "C" compiler see this */

#include "smath.h"

class sControlState
{
public:
  int   on;
  sREAL setPoint;
  sREAL controlStep;
  sREAL	dVel;
  int flag;

  sControlState()
    {
      on = 0;
      setPoint = 0.0;
      flag = 0;
      controlStep = static_cast<sREAL>(0.1);
    }

  friend void SETCONTROL(sControlState &cs,
			 sREAL setPoin)
    {
      cs.setPoint = setPoin;
      cs.controlStep = 1.0;
      cs.on = 1;
      cs.flag = 0;
    }

  friend void SETPIDCONTROL(sControlState &cs,
			    sREAL setPoin,
			    sREAL dVl = 0.0,
			    sREAL controlStp = 1.0
                            )
    {
      cs.setPoint = setPoin;
      cs.dVel = dVl;
      cs.controlStep = controlStp;
      cs.on = 1;
      cs.flag = 0;
    }

  friend void SETCONTROLX(sControlState &cs,
			  sREAL setPoin,
			  sREAL controlStp = static_cast<sREAL>(0.01),
			  sREAL = 0.0
			  )
    {
      cs.setPoint  = setPoin;
      cs.controlStep = controlStp;
      cs.on = 1;
      cs.flag = 0;
      cs.dVel = 0.0;
    }

  int Done()
    {
      return (flag);
    }

  bool IsOn()
    {
      return (on);
    }

  void Activate(int sw = 1)
    {
      on = sw;
    }

  void Off()
    {
      on = 0;
      flag = 0;
    }

  void On()
    {
      on = 1;
    }
};


#endif /* __cplusplus */
#endif
