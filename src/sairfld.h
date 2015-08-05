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
 * File   : sairfld.h                            *
 * Date   : April, 1998                          *
 * Author : Dan Hammer                           *
 *************************************************/
#ifndef SAIRFLD_H
#define SAIRFLD_H

#include "srunway.h"
#include "sarray.h"

#define sAIRFIELD_MAX_RUNWAYS			4
#define sAIRFIELD_MAX_AIRFIELDS		256

typedef struct sairfield_info_struct
{
	char		id[32];
	int		nrunways;
	double	x;
	double	y;
	double	z;
	srunway_info runways[sAIRFIELD_MAX_RUNWAYS];
} sairfield_info;

#ifdef __cplusplus
extern "C" {
#endif	
	void add_srunway(sairfield_info *airfield, srunway_info *runwayinf);
	void clear_airfield_info(sairfield_info *airfield_info);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus


class sAirfield : public sObject
{
public:
	sAirfield()
	{}

	virtual ~sAirfield();

	sAirfield(const sairfield_info &);

	sRunway	*GetRunway(int which);		
	
	const sPoint &GetPosition()
	{
		return position;
	}

	static void AddsAirfield(sAirfield *);
	static void AddsAirfield(const sairfield_info &);
	static sAirfield *GetsAirfield(const char *airfieldId);
	static void FlushsAirfields();

protected:
	char						id[32];
	sObjectArray			runways;
	sPoint					position;

	static sObjectArray airfields;
};

#endif

#endif
