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
 * File   : sairfld.cpp                          *
 * Date   : April, 1998                          *
 * Author : Dan Hammer                           *
 *************************************************/
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "sairfld.h"


void add_irunway(sairfield_info *airfield_info, srunway_info *runwayinf);
void add_irunway(sairfield_info *airfield_info, srunway_info *runwayinf)
{
	if (airfield_info->nrunways < sAIRFIELD_MAX_RUNWAYS)
	{
		airfield_info->nrunways++;
		airfield_info->runways[airfield_info->nrunways-1].x = runwayinf->x;
		airfield_info->runways[airfield_info->nrunways-1].y = runwayinf->y;
		airfield_info->runways[airfield_info->nrunways-1].z = runwayinf->z;
		airfield_info->runways[airfield_info->nrunways-1].heading = runwayinf->heading;
		airfield_info->runways[airfield_info->nrunways-1].length = runwayinf->length;
		airfield_info->runways[airfield_info->nrunways-1].width = runwayinf->width;
	}
}

void clear_airfield_info(sairfield_info *airfield_info)
{
	airfield_info->nrunways = 0;
	airfield_info->id[0] = 0;
	airfield_info->x = airfield_info->y = airfield_info->z = 0.0;
}

sAirfield::~sAirfield() = default;

sAirfield::sAirfield(const sairfield_info &airf_info)
							:runways(sAIRFIELD_MAX_RUNWAYS,1)
{
	strncpy(id,airf_info.id,32);
	id[31] = 0;

	position = sPoint(float(airf_info.x),float(airf_info.y),float(airf_info.z));
	int nRunways = airf_info.nrunways;
	if (nRunways > sAIRFIELD_MAX_RUNWAYS)
		nRunways = sAIRFIELD_MAX_RUNWAYS;
	for (int i=0;i<nRunways;i++)
	{
		sRunway *runway = new sRunway(airf_info.runways[i]);
		runways.Add(runway);
	}
}

sRunway *sAirfield::GetRunway(int which)
{
	if (which >= 0 && which < runways.Count())
		return (sRunway *) runways[which];
	else
		return NULL;
}

sObjectArray sAirfield::airfields(sAIRFIELD_MAX_AIRFIELDS,0);

void sAirfield::AddsAirfield(sAirfield *airfield)
{
	if (airfields.Count() < sAIRFIELD_MAX_RUNWAYS)
		airfields.Add(airfield);
}

void sAirfield::AddsAirfield(const sairfield_info &airfield_info)
{
	if (airfields.Count() < sAIRFIELD_MAX_RUNWAYS)
	{
		sAirfield *airfield = new sAirfield(airfield_info);
		airfields.Add(airfield);
	}
}

sAirfield *sAirfield::GetsAirfield(const char *airFieldId)
{
	sAirfield *result = NULL;

	for (int i=0;i<airfields.Count();i++)
	{
		sAirfield *airfield = (sAirfield *)airfields[i];
		if (airfield)
		{
			if (strcmp(airfield->id,airFieldId) == 0)
			{
				result = airfield;
				break;
			}
		}
	}
	return result;
}

void sAirfield::FlushsAirfields()
{
	airfields.Flush();
}





