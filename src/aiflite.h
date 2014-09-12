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
 * File   : aiflite.h                            *
 * Date   : April, 1998                          *
 * Author : Dan Hammer                           *
 *************************************************/
#ifndef __aiflite_h
#define __aiflite_h

enum  { aiF_TARGETMODE_NOACTION, aiF_TARGETMODE_EVADE, aiF_TARGETMODE_ENGAGE };

#ifdef __cplusplus

#include "aibase.h"
#include "aipilot.h"
#include "swaypnt.h"

#define aiMAX_FLITES              256
#define aiFLITE_DEFAULT_OWNERSHIP   1

class aiFlite : public aiBase
{
public:

  enum {   aiF_NAVIGATE, aiF_INTERCEPT, aiF_ENGAGE, aiF_ENGAGE_PLAYER,
	   aiF_STRIKE, aiF_MANEUVER, aiF_TARGETS, aiF_TAKEOFF, aiF_LAND
  };
   
  aiFlite();
  aiFlite(int max, int owns = 1, const char *id = NULL);
   
  virtual ~aiFlite()
    {}

  int IsA() const
    {
      return (sAI_FLITE_T);
    }

  virtual void Init();
  virtual void Update(double time_frame);
  virtual void Destroy();

  int Add(aiPilot *);
  void AddPlayerPilot(aiPilot *);
  aiPilot *GetPilot(int idx = 0);
  void SetWaypoints(const sWaypoint *wps, int nn);
  void SetWaypoints(const swaypoint_info *winfo, int nn);
  sWaypoint *GetWaypoint(int idx);
  sWaypoint *GetNextWaypoint();
  sWaypoint *GetPreviousWaypoint();
  void InitFormation(sWaypoint *wp);
  void DoCapWaypoint();
  void DoInterceptWaypoint();
  void DoEngagePlayerWaypoint();
  void DoStrikeWaypoint();
  void DoNavigateWaypoint();
  void DoTargetsWaypoint();

  void DoEngageUpdate();
  void DoInterceptUpdate();
  void DoEngagePlayerUpdate();
  void DoStrikeUpdate();
  void DoTargetsUpdate();
  void DoTakeOffUpdate();
  void DoLandUpdate();

  int GetCount()
    {
      return (n);
    }
  int IsPlayerFlite()
    {
      return (isPlayerFlite);
    }
  int GetPlayerWingPos()
    {
      return (playerPilotWingPos);
    }
  int IncViewPilot();
  int GetViewIndex()
    {
      return (viewPilot);
    }

  void SetFormationWaypoint(sWaypoint *wp, int leadPilot = 0);
  void SetManeuver(int maneuver, uint32_t flags, sREAL d0, sREAL d1,
		   sREAL d2);
   
  void EngageFlite(aiFlite *);
  int AssignEngageTarget(aiPilot *pil, aiFlite *targetFlite);
  void DoWaypoints();
  aiPilot *GetLeader();
  aiPilot *GetNextActivePilot(int & start);
   
  const sPoint &GetPosition()
    {
      return (position);
    }
  void SetId(const char *i)
    {
      SetHandle(i);
    }
  const char *GetId()
    {
      return GetHandle();
    }
  int NWaypoints()
    {
      return waypoints.Count();
    }
  int GetActiveCount()
    {
      return activeCount;
    }
  int IsActive()
    {
      return (activeCount > 0);
    }
  int GetMode()
    {
      return (mode);
    }
  void SetVisualEngagementRadius(sREAL radius)
    {
      visualEngagementRadius = radius * radius;
    }
  void KillAttackers();
  void KillEngageFlite(aiFlite *engageFlite);

  void SetTargetMode(int mod)
    {
      if (mod >= aiF_TARGETMODE_NOACTION && mod <= aiF_TARGETMODE_ENGAGE)
	targetMode = mod;
      else
	targetMode = aiF_TARGETMODE_EVADE;
    }

  sREAL GetNewTargetDelayTime()
    {
      return (newTargetDelayTime);
    }
  void SetNewTargetDelayTime(sREAL newTargetDelayTim)
    {
      this->newTargetDelayTime = newTargetDelayTim;
    }

  void SetDamage(int, int shooterIndex)
    {
      damageFlag = 1;
      damagerIdx = shooterIndex;
    }

  aiPilot *GetPlayerWingman();

  /***********************************************
   * player-issued commands                      *
   ***********************************************/
  void SetPlayerCommandMode(int on)
    {
      playerCommandMode = on;
    }
  int GetPlayerCommandMode()
    {
      return (playerCommandMode);
    }
  void EngageNearestFlite();
  void FormUp();
  void NextWaypoint();
  void PreviousWaypoint();
  void ProtectPlayer();
   
  void SetAsPlayerFlite(int isPlayer);
protected:
  int            curTask;                /* current task                       */
  int            mode;                   /* current mode                       */
  sObjectArray   pilots;                 /* array of pilots                    */
  int            n;                      /* how many pilots/flight modls       */
  int            max;                    /* max pilots/flight models           */
  sObjectArray   waypoints;              /* array of waypoints                 */
  int            wpidx;                  /* index into                         */
  sWaypoint      *curWaypoint;           /* current waypoint                   */
  int            viewPilot;              /* pilot providing debug viewpoint    */
  int            isPlayerFlite;          /* true if player is in this flite    */
  int            playerPilotWingPos;     /* player's wing position             */
  uint32_t  engageFliteIdx;         /* idx of flite engaged with          */
  uint32_t  interceptFliteIdx;      /* idx of flite intercepting          */
  char           id[9];                  /* char id                            */
  aiPilot        *leader;                /* flite leader pilot                 */
  int            leaderIndex;            /* index of                           */
  int            activeCount;            /* how many pilots active             */
  sPoint         position;               /* current position                   */
  sREAL          visualEngagementRadius; /* max distance for visual engagement */
  sREAL          newTargetDelayTime;     /* delay for assigning pilots targets */
  int            attackerCount;          /* how many attackers                 */
  sObjectArray   freePilots;             /* working free pilot list            */
  int            targetMode;             /* mode for acting as target-practice */
  int            damageFlag;             /* true if any pilots hit by enemy fire*/
  uint32_t  damagerIdx;             /* idx of pilot causing above damage  */
  int            nextPilot;              /* next pilot to takeoff,land         */
  int            playerCommandMode;      /* if 1 & player is leader, player    */
  /*    commands flite                  */

  void           DistributeAttackers();
  void           HandleDelayedTargets(aiFlite *engageFlite);
  static void    IssuePlayerAttackWarning(aiFlite *playerFlite, aiPilot *attackerPilot,
					  aiPilot *playerPilot);
public:
  /******************
   * static members
   ******************/
  static sObjectArray aiFlites;
  static uint32_t nextIdx;
  static aiFlite *playerFlite;

  static void AddaiFlite(aiFlite *flite);
  static aiFlite *GetaiFlite(uint32_t idx);
  static aiFlite *GetaiFlite(char *id);
  static int GetFliteCount();
  static void FlushaiFlites();
  static void RemoveaiFlite(aiFlite *flite);
  static aiFlite *GetPlayerFlite();
  static aiFlite *GetFliteByIndex(int i);
  static void SetOwnership(int owns)
    {
      aiFlites.SetOwnership(owns);
    }
  static void aiFlitesUpdate(double timeFrame);
};

#endif /* __cplusplus */
#endif /* __ifilite_h */
