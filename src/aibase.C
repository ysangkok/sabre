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
 * File   : aibase.cpp                           *
 * Date   : April, 1998                          *
 * Author : Dan Hammer                           *
 *************************************************/
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include "aibase.h"

aiBase::aiBase()
{
	timeFrame = elapsedTime = 0.0;
	logFlag = 0;
	logFile = NULL;
	task = 0;
	dbg = "ctr";
	affiliation = 0;
	for (int i=0;i<aiB_SKILL_SLOTS;i++)
		skillLevels[i] = 100;
	strcpy(handle,"");
	active = 1;
}

aiBase::~aiBase()
{
	CloseLog();
}

void aiBase::Destroy()
{
	LOG("Destroy");
}

/*
 * Initialization
 */
void aiBase::Init()
{
	timeFrame = elapsedTime = 0.0;
	task = 0;
	active = 1;
	dbg = "NOTHING";
	attackList.DeleteAllAttackers();
	mnvrStackPtr = -1;
	curManeuver = NULL;
	LOG("----------%s------------",GetHandle());
}


// Update routine. 
void aiBase::Update(double timeFrm)
{
	timeFrame = (sREAL) timeFrm;
	oneOverTimeFrame = 1 / (timeFrame + eps);
	elapsedTime += timeFrame;
}

void aiBase::SetHandle(const char *hndl)
{
	if (hndl)
	{
		strncpy(this->handle,hndl,sizeof(this->handle));
		this->handle[sizeof(this->handle)-1] = 0;
	}
}

/*******************************************************
 * maneuver stack routines                             *
 *******************************************************/
// Execute maneuvers on the maneuver stack.
void aiBase::ExecManeuverStack(void)
{
int i;

	// Start at top
	i = mnvrStackPtr;
	while (i >= 0)
	{
		sManeuverState &ms = mnvrStack[i];
		UpdateManeuver(ms);
		// If this maneuver is done and not flagged as "looping",
		// pop it
		if (ms.done && !ms.IsLoop())
		{
			ClearManeuverStackTo(ms.stackLevel);
			PopManeuver();
		}
		i--;
	}
}

// Display maneuver stack
void aiBase::ShowManeuverStack(void)
{

	LOG("----   Maneuver Stack   ----");
	for (int i=0;i<=mnvrStackPtr;i++)
	{
		sManeuverState &ms = mnvrStack[i];
		LOG("%d: %s(%02d)  %lu  %5.3f %5.3f %5.3f %d",
			i,
			ms.GetManeuverString(),
			ms.state,
			ms.flags,
			ms.data0,
			ms.data1,
			ms.data2,
			ms.stackLevel);
	}
	LOG("---- End Maneuver Stack ----");
}

// Get top of stack
const sManeuverState & aiBase::GetManeuverStackTop()
{
	if (mnvrStackPtr >= 0)
		return mnvrStack[mnvrStackPtr];
	else
		return mnvrStack[0];
}
// Get bottom of stack
const sManeuverState & aiBase::GetManeuverStackBottom()
{
	return mnvrStack[0];
}

// Create maneuver description string
char *aiBase::BuildManeuverStr()
{
int i;
char nbuff[16];
*debug_str = 0;

	for (i=0;i<=mnvrStackPtr;i++)
	{
		sManeuverState &ms = mnvrStack[i];
		strcat(debug_str,ms.GetManeuverString());
		sprintf(nbuff,"(%02d)",ms.state);
		strcat(debug_str,nbuff);
		if (i<mnvrStackPtr)
			strcat(debug_str,":");
	}
	return debug_str;
}

// Push a maneuver on to the stack
sManeuverState *aiBase::PushManeuver(int mnvr, uint32_t flags, sREAL d0,
												sREAL d1, sREAL d2)
{
	if (mnvrStackPtr < aiB_MNVR_MAX - 1)
	{
		mnvrStackPtr++;
		SETMANEUVER(mnvrStack[mnvrStackPtr],
					mnvr,
					mnvrStackPtr,
					flags,
					d0,d1,d2);
		curManeuver = & mnvrStack[mnvrStackPtr];
		ShowManeuverStack();
		return (curManeuver);
	}
	else
		return (NULL);
}

sManeuverState *aiBase::PushManeuverX(int mnvr, uint32_t flags,
													sREAL d0, sREAL d1, sREAL d2,
													sREAL d3, sREAL d4, sREAL d5)
{
	if (mnvrStackPtr < aiB_MNVR_MAX - 1)
	{
		mnvrStackPtr++;
		SETMANEUVERX(mnvrStack[mnvrStackPtr],
					mnvr,
					mnvrStackPtr,
					flags,
					d0,d1,d2,
					d3,d4,d5);
		curManeuver = & mnvrStack[mnvrStackPtr];
		ShowManeuverStack();
		return (curManeuver);
	}
	else
		return (NULL);
}

// Pop a maneuver off of the stack
sManeuverState *aiBase::PopManeuver()
{
	if (mnvrStackPtr >= 0)
	{
		mnvrStackPtr--;
		if (mnvrStackPtr >= 0)
			curManeuver = & mnvrStack[mnvrStackPtr];
		else
			curManeuver = NULL;
	}
	else
		curManeuver = NULL;
	ShowManeuverStack();
	return (curManeuver);
}

// Clear the maneuver stack to a given level
void aiBase::ClearManeuverStackTo(int level)
{
	while (mnvrStackPtr > level)
		PopManeuver();
}

// Clear the entire maneuver stack
void aiBase::ClearManeuverStack()
{
	mnvrStackPtr = -1;
	curManeuver = NULL;
	LOG("Maneuver Stack Cleared");
}

// Set a maneuver. This clears the stack
// and makes the maneuver the current one
void aiBase::SetManeuver(int mnvr, uint32_t flags, 
									sREAL d0, sREAL d1, sREAL d2)
{
	if (mnvr >= 1 && mnvr < sManeuver::LAST)
	{
		ClearManeuverStack();
		PushManeuver(mnvr,flags,d0,d1,d2);
		LOG("Setting Maneuver To: %s",
			sManeuver::Maneuver2String(mnvr));
	}
	else
		LOG("Invalid Maneuver Passed: %d",
			mnvr);
}

void aiBase::SetManeuverX(int mnvr, uint32_t flags, 
									sREAL d0, sREAL d1, sREAL d2,
									sREAL d3, sREAL d4, sREAL d5)
{
	if (mnvr >= 1 && mnvr < sManeuver::LAST)
	{
		ClearManeuverStack();
		PushManeuverX(mnvr,flags,d0,d1,d2,d3,d4,d5);
		LOG("Setting Maneuver To: %s",
			sManeuver::Maneuver2String(mnvr));
	}
	else
		LOG("Invalid Maneuver Passed: %d",
			mnvr);
}

void aiBase::UpdateManeuver(sManeuverState &)
{

}

/**************************************************************
 * Log                                                        *
 **************************************************************/
int aiBase::OpenLog(const char *path)
{
	char logPath[BUFSIZ];

	if (path == NULL)
	{
		sprintf(logPath,"%s.log",
				GetHandle());
		path = logPath;
	}

	logFile = fopen(path,"w");
	if (logFile != NULL)
		logFlag = 1;
	return (logFlag);
}

void aiBase::CloseLog()
{
	if (logFile != NULL)
		fclose(logFile);
	logFile = NULL;
	logFlag = 0;
}

void __cdecl aiBase::LOG(const char *format, ...)
{
	if (logFlag && logFile)
	{
		char s[BUFSIZ];
		va_list ap;
		va_start(ap,format);
		vsprintf(s,format,ap);
		va_end(ap);
		fprintf(logFile,"%s\n",s);
		fflush(logFile);
	}
}

/**********************************************************
 * Attack list routines                                   *
 **********************************************************/
void aiBase::AddAttacker(uint32_t attackerIdx)
{
	attackList.AddAttacker(attackerIdx);
}

void aiBase::DeleteAttacker(uint32_t attackerIdx)
{
	attackList.DeleteAttacker(attackerIdx);
}

void aiBase::KillAttackList()
{
  attackList.DeleteAllAttackers();
}

/***********************************************************
 * skill levels                                            *
 ***********************************************************/
void aiBase::SetSkillLevel(int level, int slot)
{
	if (level <= 0)
		level = 1;
	if (level > 100)
		level = 100;
	if (slot >= 0 && slot < GetMaxSkillSlots())
		skillLevels[slot] = level;
}

int aiBase::GetSkillLevel(int slot)
{
	if (slot >= 0 && slot < GetMaxSkillSlots())
		return (skillLevels[slot]);
	else
		return (0);
}



