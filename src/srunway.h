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
 * File   : srunway.h                            *
 * Date   : April, 1998                          *
 * Author : Dan Hammer                           *
 *************************************************/
#ifndef RUNWAY_H
#define RUNWAY_H

typedef struct srunway_info_struct
{
	double	x;
	double	y;
	double	z;
	double	heading;
	double	length;
	double	width;
} srunway_info;

#ifdef __cplusplus

#include "sslewer.h"

class sRunway : public sSlewer
{
public:

	sRunway()
	{}

	virtual ~sRunway();

	sRunway(const srunway_info &);

	unsigned int GetRunwayNumber()
	{
		return runwayNumber;
	}
	/*
	 * position is touchdown point for landing
	 */
	sPoint &GetPosition()
	{
		return pos;
	}
	sAttitude &GetAttitude()
	{
		return att;
	}
	sREAL GetLength()
	{
		return length;
	}
	sREAL GetWidth()
	{
		return width;
	}
	sREAL GetFinalApproachHeading()
	{
		return finalApproachHeading;
	}
	sREAL GetTakeoffHeading()
	{
		return takeoffHeading;
	}
	/* 
	 *  approach heading & takeoff heading
	 *  are relative to aircraft
	 */
	void SelectFinalApproachHeading()
	{
		att.yaw = finalApproachHeading;
		BuildMatrixes();
	}
	void SelectTakeoffHeading()
	{
		att.yaw = takeoffHeading;
		BuildMatrixes();
	}


	void SetPositionAndAttitude(const sPoint &position, 
										const sAttitude &attitude)
	{
		sAttitude tempAtt = attitude;
		tempAtt.pitch = tempAtt.roll = 0.0;
		finalApproachHeading = attitude.yaw;
		takeoffHeading = sIncrementAngle(attitude.yaw,Pi);
		sSlewer::SetPositionAndAttitude(position,tempAtt);
	}

	void GetLandingTurnPoint(sPoint &turnPoint, sREAL approachDistance, sREAL turnRadius,
									int clockWise);
	void GetTakeoffPositionAndAttitude(sPoint &, sAttitude &);

protected:
	unsigned int		runwayNumber;
	sREAL			length;
	sREAL			width;
	sREAL			finalApproachHeading;
	sREAL			takeoffHeading;
};

#endif

#endif
