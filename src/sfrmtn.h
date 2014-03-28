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
 * File   : sfrmtnt.h                            *
 * Date   : April, 1998                          *
 * Author : Dan Hammer                           *
 * formation flying data                         *
 *************************************************/
#ifndef __sfrmnt_h
#define __sfrmnt_h

enum	{	
			sfWEDGE, sfVIC, sfECHELON_R, sfECHELON_L,
			sfTRAIL, sfLADDER, sfDIAMOND, sfFIGHTING_WING 
		};
#define sfMAXWINGMEN			3
#define sfFORMATION_MAX		(sfMAXWINGMEN+1)
#define sfMAXGROUPS			5
#define sfNFORMATION_TYPES	8

#ifdef __cplusplus

#include "sobject.h"
#include "smath.h"


struct FormationDat
{
	const char * name;
	sREAL woffsets[sfFORMATION_MAX][3];
	sREAL goffsets[sfMAXGROUPS][3];
};

class sFormationData : public sObject
{
public:
	char		name[32];
	sVector     wingmanOffsets[sfFORMATION_MAX];
	sVector		groupOffsets[sfMAXGROUPS];

	sFormationData();
	~sFormationData();
	void Set(const struct FormationDat &);
	static sFormationData formationData[sfNFORMATION_TYPES];
	static struct FormationDat formationDat[sfNFORMATION_TYPES];
	static void Init(void);
	static const sFormationData &GetFormationData(int formation);
	static const sFormationData &GetFormationData(const char *formation_name);
	static void GetWingmanVector(int formation,
								int wingpos,
								sVector &result);
	static void GetWingmanVector(const char *formation_name,
								int wingpos,
								sVector &result);
};




#endif /* __cplusplus */
#endif
