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
 * File   : aibase.h                             *
 * Date   : November, 1998                       *
 * Author : Dan Hammer                           *
 * Base class for AI classes                     *
 * Defines skill levels, maneuver stack and      *
 * attacker list behavior.                       *
 *************************************************/
#ifndef __aibase_h
#define __aibase_h

#ifdef __cplusplus
#include "sobject.h"
#include "sarray.h"
#include "smnvrst.h"
#include "sattkr.h"

#define aiB_MNVR_MAX					16
#define aiB_SKILL_SLOTS				16
#define aiB_HANDLE_SIZE				16
#define aiB_DEBUG_MAX					512

class aiBase : public sObject
{
public:
	aiBase();
	virtual ~aiBase();

	virtual void Init();
	virtual void Update(double timeFrame);
	virtual void Destroy();

	int GetAffiliation()
	{
		return affiliation;
	}
	void SetAffiliation(int affiliation)
	{
		this->affiliation = affiliation;
	}
	char *GetHandle()
	{
		return handle;
	}
	void SetHandle(const char *handle);
	int GetTask()
	{
		return (task);
	}
	void SetTask(int task)
	{
		this->task = task;
	}
	unsigned long GetIdx()
	{
		return idx;
	}
	void SetIdx(unsigned long idx)
	{
		this->idx = idx;
	}
	int GetIndex()
	{
		return index;
	}
	void SetIndex(int index)
	{
		this->index = index;
	}
   int GetAttackerCount()
   {
		return attackList.GetCount();
   }
   void AddAttacker(unsigned long attackerIdx);
   void DeleteAttacker(unsigned long attackerIdx);
	int HasAttackers()
	{
		return attackList.GetCount() > 0;
	}
	void KillAttackList();
	virtual int IsActive()	
	{ 
		return active;
	}
	virtual int GetMaxSkillSlots()
	{
		return aiB_SKILL_SLOTS;
	}
	void SetSkillLevel(int level, int slot);
	int GetSkillLevel(int slot);
	virtual void SetManeuver(int mnvr, unsigned long flags = 0, sREAL d0 = 0.0,
						 sREAL d1 = 0.0, sREAL d2 = 0.0);
	virtual void SetManeuverX(int mnvr, unsigned long flags = 0, 
								  sREAL d0 = 0.0, sREAL d1 = 0.0, sREAL d2 = 0.0,
								  sREAL d3 = 0.0, sREAL d4 = 0.0, sREAL d5 = 0.0);

	int OpenLog(char *path = NULL);
	void CloseLog();
	void __cdecl LOG(const char *, ...);
	sREAL GetTimeFrame()
	{
		return timeFrame;
	}
	sAttacker *GetFirstAttacker() 
	{ 
		return attackList.GetFirstAttacker(); 
	}

	sAttacker *GetNextAttacker()	
	{ 
		return attackList.GetNextAttacker();	
	}

	const sManeuverState &GetManeuverStackTop();
	const sManeuverState &GetManeuverStackBottom();

protected:
	/**************************************************************************
    * protected members                                                      *
    **************************************************************************/
   unsigned long  idx;                       /* unique id                    */
   int            index;                     /* ordinal value                */
   char           debug_str[aiB_DEBUG_MAX];  /* debugging string             */
   int            affiliation;               /* whose side we're on          */
   int            active;                    /* active status                */
   char           handle[aiB_HANDLE_SIZE];   /* call sign                    */
   char           *dbg;                      /* debug string                 */
   int            task;                      /* current task                 */

   sREAL          timeFrame;                 /* time in secs from last update*/
   sREAL          oneOverTimeFrame;
   sREAL          elapsedTime;               /* total time elapsed           */
   int            logFlag;                   /* true if logging              */
   FILE           *logFile;                  /* logging file                 */

   sManeuverState  mnvrStack[aiB_MNVR_MAX];  /* the maneuver stack           */
   int             mnvrStackPtr;             /* maneuver stack pointer       */
   sManeuverState *curManeuver;              /* current maneuver             */

	/* attacker list */
	sAttackerList	attackList;
	sREAL				attackerCheckInterval;   /* time interval for checking attacker list */ 

	int				skillLevels[aiB_SKILL_SLOTS];

	void					ExecManeuverStack(void);
	virtual void		UpdateManeuver(sManeuverState &im);
	sManeuverState		*PushManeuver(int mnvr, unsigned long flags = 0, sREAL d0 = 0.0,
												sREAL d1 = 0.0, sREAL d2 = 0.0);
	sManeuverState		*PushManeuverX(int mnvr, unsigned long flags = 0,
												sREAL d0 = 0.0, sREAL d1 = 0.0, sREAL d2 = 0.0,
												sREAL d3 = 0.0, sREAL d4 = 0.0, sREAL d5 = 0.0);
	sManeuverState		*PopManeuver(void);
	void					ClearManeuverStack(void);
	void					ClearManeuverStackTo(int);
	char					*BuildManeuverStr();
	void					ShowManeuverStack(void);

};
#endif /* __cplusplus */

#endif
