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
 * File   : aipilot.h                            *
 * Date   : April, 1998                          *
 * Author : Dan Hammer                           *
 * aiPilot class definition                      *
 *************************************************/
#ifndef __aipilot_h
#define __aipilot_h

/* enum for skills */
enum	{ 
			aiPILOT_SKILL_MANEUVER, aiPILOT_SKILL_SHOOT, aiPILOT_SKILL_ENERGY, 
			aiPILOT_SKILL_SA, aiPILOT_SKILL_BOMB
		}; 	

#ifdef __cplusplus
#include "sobject.h"
#include "sarray.h"
#include "sfltmdl.h"
#include "scntrl.h"
#include "starget.h"
#include "sweapon.h"
#include "smnvrst.h"
#include "stact.h"
#include "splncaps.h"
#include "spilcaps.h"
#include "swaypnt.h"
#include "sattkr.h"
#include "spid.h"
#include "aigunner.h"
#include "aibase.h"
#include "aicomm.h"

#define aiPILOT_MAX_PILOTS       256
#define aiPILOT_SKILL_SLOTS        5
#define aiPILOT_AVERAGING_N       16
#define aiPILOT_MAX_GUNNERS       20
#define aiPILOT_BROADCAST_ALL    255
#define aiPILOT_DEFAULT_OWNERSHIP  0

class aiPilot : public aiBase
{
public:
	/* enums for task */
	enum { 
           NOTHING, CAP, ENGAGE_TARGET, FORMATION_FLY, NAVIGATE, INTERCEPT_TARGET, 
           STRAFE, ROCKET, LEVEL_BOMB, DIVE_BOMB, TORPEDO_BOMB, AIRSHOW, EVADE_TARGET,
			  TAKEOFF, LAND 
         };

	aiPilot(sFlightModel *flightModel);

	virtual ~aiPilot();

	virtual bool IsA() const
	{
		return (sAI_PILOT_T);
	}

	virtual void Init();
	virtual void Update(double timeFrame);
	virtual void Destroy();

	void UpdateCallback(int sw);
    /*
     *  Tasks
     */
   void Nothing();
   void InterceptTarget();
   void EngageTarget();
   void FlyFormation();
   void Navigate();
   void Strafe();
   void Rocket();
   void LevelBomb();
   void DiveBomb();
   void TorpedoBomb();
   void AirShow();
	void EvadeTarget();
	void TakeOff();
	void Land();

	/*
	 * Set a task
	 */
	void SetInterceptTarget(uint32_t targetIndex);
	void SetEngageTarget(uint32_t targetIndex);
	void SetFormationTarget(uint32_t targetIndex, int wingPosition = 1, int formationType = 0);
	void SetNavigatePoint(sWaypoint *wp);
	void SetSurfaceTarget(uint32_t surfaceTargetIndex, int attackMethod, sPoint *worldPoint = NULL);
	void SetAirShowPoint(sWaypoint *wp);
	void SetEvadeTarget(uint32_t targetIndex);
	void SetTakeOff(sWaypoint *wp);
	void SetLand(sWaypoint *wp);

   int GetInterceptStatus();
	int	GetEngageStatus();
   uint32_t GetTargetIdx();
	void Disengage();

   void AddAttacker(aiPilot *pil);
   void DeleteAttacker(aiPilot *pil);
     

	virtual void GetRemoteControlInputs() = 0;

	virtual void SetFlightModel(sFlightModel *flt_model);

	sFlightModel *GetFlightModel()
	{
		return (flightModel);
	}

	uint32_t GetFliteId()
	{
		return (fliteId);
	}
	void SetFliteId(uint32_t fltId)
	{
		this->fliteId = fltId;
	}
	int GetRemoteControl()
	{
		return (remoteControl);
	}

	bool IsDead()
	{
		return (dead);
	}

	virtual void SetDeath(bool dead);

	bool IsEjected()
	{
		return (ejected);
	}
	void SetEjected(int ejected);

	int HasGunners()
	{
		return (hasGunners);
	}
	unsigned int HasNGunners()
	{
		return (gunners.Count());
	}
	bool HasAttackers()
	{
		return (attackList.GetCount() > 0);
	}
	/* 
	 * to allow us to generically include the player as part of
	 * flights
	 */
	bool isPlayerFlag;
	virtual bool IsPlayer()
	{
		return (isPlayerFlag);
	}
	void SetAsPlayer(int);

	virtual const char *GetModel();
	virtual sREAL GetAltitudeFPS();
	virtual void GetPositionAndAttitude(sPoint &position, 
		sAttitude &attitude);
	virtual void SetPositionAndAttitude(const sPoint &position,
													const sAttitude &attitude);
	virtual void GetDirectionNormal(sVector &);
	sREAL GetFormationWingLen() 
	{ 
		return formationWingLen;
	}
	sREAL GetFormationOffset()
	{
		return formationOffset;
	}
	virtual bool IsActive()	
	{ 
		return !(IsDead() || IsEjected());
	}
  
	int GetTargetActive();

	void SetRemoteControl(int remoteControl);

	virtual void DoHUD()
	{}


	const char *GetCapsId()	{ return (capsId); }
	void SetCapsId(const char *);

	sREAL GetNavigationDistance();
	sREAL GetSurfaceTargetDistance();
	bool   IsBombing()	
	{ 
		return isBombing; 
	}

	sREAL	timerExtra; /* for convenience of iFlite class */
	int markedAsFree;
	int GetWingPosition()
	{
		return (wingPos);
	}
	void EnableGroundCollision(int enable)
	{
		groundCollisionOn = enable;
	}
	int GetGroundCollisionOn()
	{
		return (groundCollisionOn);
	}
	sREAL GetHeightAGL()
	{
		return (flightModel->GetHeightAGL());
	}
	sREAL GetAirSpeedFPS()
	{
		return (flightModel->GetAirSpeedFPS());
	}
	int GetClearForTakeoff()
	{
		return (clearForTakeoff);
	}
	void SetClearForTakeoff(int clear);
	int GetClearForLanding()
	{
		return (clearForLanding);
	}
	void SetClearForLanding(int clear);

	void SetGunnersTarget(uint32_t targetIdx, sREAL threatValue = ITARGET_MIN_THREAT_VALUE);
	void SetGunnersTarget(sAttacker *attkr);
	void SetGunnerTarget(unsigned int whichGunner, uint32_t targetIdx, sREAL threatValue = ITARGET_MIN_THREAT_VALUE);
	void ClearGunnersTarget();

	void BodyVector2WorldVector(const sVector &body, sVector &world)
	{
		if (flightModel)
			flightModel->BodyVector2WorldVector(body,world);
	}
	void WorldVector2BodyVector(const sVector &world, sVector &body)
	{
		if (flightModel)
			flightModel->WorldVector2BodyVector(world,body);
	}
	void BodyPoint2WorldPoint(const sPoint &body, sPoint &world)
	{
		if (flightModel)
			flightModel->BodyPoint2WorldPoint(body,world);
	}
	void WorldPoint2BodyPoint(const sPoint &world, sPoint &body)
	{
		if (flightModel)
			flightModel->WorldPoint2BodyPoint(world,body);
	}

	const sTarget &GetEngageTarget()
	{
		return engageTarget;
	}
	const sTarget &GetFormationTarget()
	{
		return formationTarget;
	}
	const sTarget &GetInterceptTarget()
	{
		return interceptTarget;
	}
	const sTarget &getEvadeTarget()
	{
		return evadeTarget;
	}

	void SetFlightModelUpdateFlag(int flag)
	{
		flightModelUpdateFlag = flag;
	}
	int GetFlightModelUpdateFlag()
	{
		return flightModelUpdateFlag;
	}
	const sNavInfo &GetNavInfo()
	{
		return navInfo;
	}

	/* static functions */
	static sObjectArray aiPilots;
	static uint32_t	nextIdx;
	static aiPilot *playerPilot; /* represents player */
	static aiPilot *cashedPilot;	/* to avoid too many array lookups */
	static void AddaiPilot(aiPilot *pilot);
	static aiPilot *GetaiPilot(uint32_t idx);
	static aiPilot *GetaiPilot(const char *handle);
	static unsigned int GetPilotCount();
	static void aiPilotUpdateCallback(int, void *);
	static void FlushaiPilots();
	static void RemoveaiPilot(aiPilot *pil);
	static aiPilot *GetPilotByIndex(unsigned int i);
	static void __cdecl TextMessageToPlayer(const char *, ...);
	static void BodyVector2WorldVector(unsigned int idx, const sVector &body, sVector &world);
	static void WorldVector2BodyVector(unsigned int idx, const sVector &world, sVector &body);
	static void BodyPoint2WorldPoint(unsigned int idx, const sPoint &body, sPoint &world);
	static void WorldPoint2BodyPoint(unsigned int idx, const sPoint &world, sPoint &body);
	
	static void SetOwnership(int owns)
	{
		aiPilots.SetOwnership(owns);
	}

	/* friendly stuff */
	friend class aiFlite;

protected:
    /**********************************************************************
     * protected members                                                  *
     **********************************************************************/
	sFlightModel   *flightModel;        /* pointer to flight model */
	sTarget        engageTarget;        /* air combat target info */
	sTarget        formationTarget;		/* formation flying target info */
	sTarget        interceptTarget;		/* intercept target */
	sTarget        evadeTarget;			/* evasion only target */

	int            wingPos;             /* wing position (0 = leader) */
	int            formationType;       /* formation type */
	sREAL          formationWingLen;		/* length for formation flying spacing */
	sREAL          engagementMinSpeed;	/* minimum speed to maintain during dogfight */
	sREAL          formationOffset;		/* align offset for formation flying */

   sTarget        *curTarget;          /* pointer to current target */
   sNavInfo       navInfo;             /* navigation information */
   sSurfaceTarget surfaceTarget;       /* surface target info       */
   sWeaponLimits  weaponLimits;        /* current weapon information */
   sPilotTactics  tactics;             /* tactics info */
   sAttitude      levelAttitude;       /* pitch, yaw rel to horizon */

	/*
	 * The flight model can be controlled indirectly, via roll,pitch, yaw &
	 * throttle inputs, or by directly setting the velocities & angular rates
	 */
   sControlState     rollCtl;             /* controls absolute roll indirectly */              
   sControlState     rollCtlDirect;       /* direct control of absolute roll */
   sControlState     rollRateCtl;         /* indirect control of roll rate */
   sControlState     rollRateCtlDirect;   /* direct control of roll rate */
   sREAL             rollMult;            /* multiplier for above */

   sControlState     pitchRateCtl;        /* controls pitch rate indirectly */
   sControlState     pitchRateCtlDirect;  /* controls pitch rate directly */
   sControlState     pitchCtl;            /* indirect pitch position control */
   sControlState     pitchCtlDirect;      /* direct pitch position control */

   sControlState     yawRateCtl;          /* controls yaw rate indirectly */
   sControlState     yawRateCtlDirect;    /* direct yaw rate control */
   sControlState     yawCtl;              /* indirect yaw control */
   sControlState     speedCtl;            /* airspeed, indirect */

   sControlState     gCtl;                /* gs, indirect */
   sControlState     velCtl;              /* direct control of airspeed */
   int               rollLock;            /* if set, keep roll rate = 0.0 */
   int               pitchLock;           /*    "         pitch "    */
   int               yawLock;             /*    "         yaw   "    */
   sREAL             maxRollRate;          
   sREAL             minRollRate;
    
   int               dead;                      /* living or dead */
   int               ejected;                   /* in the silk */
   uint32_t     fliteId;                         /* id of flite we're part of */              

   sREAL           timers[8];                  /* multi-purpose timers */
   int             remoteControl;              /* if true, allow player to fly us */

    /***************************************************************
     * dogfighting/maneuvering modifiers                           *
     ***************************************************************/
   sREAL       posGLimit;              /* max pos gs we can pull */
   sREAL       posStructGLimit;        /* mas gs the plane can take */
   sREAL       negGLimit;              /* mas neg gs we can pull */
   sREAL       negStructGLimit;        /* max neg gs the plane can take */
   sREAL       rollLimit;              /* angular roll rate limit */
   sREAL       extendAngle;            /* default extend pitch angle (degrees */
   sREAL       extAngle;               /* working copy of above */
   sREAL       gunAttackDistance;      /* working distance to enter gun attack mode */
   sREAL       gunAttackDistanceMul;   /*  times weapon max range gives us above */
   sREAL       engageMaxSpeed;         /* working variable for air speed max */
   sREAL       engageMaxSpeedMul;      /*  times flight model max speed gives above */
   /* PIDS */
   sPID        pitchPid;               /* pitch pid */
   sPID        yawPid;                 /* yaw pid */
   sPID        rollPid;                /* roll pid */
   sPID        gPid;                   /* g-force pid */
   sPID        airSpeedPid;            /* air-speed pid */
   sPID        targetClosurePid;       /* closure to target pid */

   sREAL       collisionAvoidSecs;           /* secs look-ahead to detect collision with target */
   sREAL       immelmanGs;                   /* g's to pull for immelman maneuver */
   sREAL       immelmanAltMin;               /* min alt for immelman maneuver */
   sREAL       noseOnRollLimit;              /* limits roll when getting nose on target */
   sREAL       speedLossLimit;               /* limit airspeed loss during gun attack */
   sREAL       splitSGs;                     /* g's to pull for split_s maneuver */
   sREAL       splitSAltMin;                 /* min alt for split_s maneuver */
   sREAL       zoomAngle;                    /* angle for zoom climb (degrees) */
   sREAL       standardTurnSpeedMPH;         /* speed for standard turn */
   sREAL       finalApproachDistance;        /* distance for final approach */
   sREAL       finalApproachAGL;             /* AGL for final approach */

    /**********************************************************
     * additional flight data                                 *
     **********************************************************/
   sREAL       Es;                  /* specific energy */
   sREAL       Ps;                  /* specific excess power */
   sREAL       deltaAirSpeed;       /* air speed loss/gained */
   sREAL       dAirSpeed[aiPILOT_AVERAGING_N]; 
   int         dAirSpeedIdx;
   sREAL       lastAirSpeed;
   sREAL       deltaAltitude;       /* altitude loss/gained */
   sREAL       dAltitude[aiPILOT_AVERAGING_N];
   int         dAltitudeIdx;
   sREAL       lastAltitude;
   int         turnBits;            /* for jinking */
   /* ground collision avoidance */
   sREAL       grndColSecs;         /* secs look-ahead to det. ground coll */
   sREAL       grndColAGL;          /* min AGL for ground coll */
   sREAL       grndColRcvrAGL;      /* min AGL to recover */
   sREAL       grndColPullUpAngle;  /* angle to pull up */

   /* attacker list */
   int            newAttacker;               /* set if a new attacker has been added    */
   bool           playerInAttackList;        /* player has been added to attacker list  */
   sREAL          playerThreatDistance;      /* distance at which player becomes threat */
   sREAL          playerThreatAspect;        /* aspect at which player becomes threat   */
   sREAL          playerThreatValue;         /* additional threat value of player       */
   uint32_t  currentHiThreatAttkrIdx;   /* idx of current highest-threat attacker  */
   int            currentAttkrPrime;         /* priming flag for above                  */

   char            capsId[32];              /* id for pilot caps lookup                 */

   /*****************************************************
    * gunners                                           *
    *****************************************************/
   bool           hasGunners;             /* if true, gunners aboard */
   sObjectArray   gunners;                /* gunners                 */
   sREAL          gunnersPitchRate;       /* how fast gunners can swing gun up, down */
   sREAL          gunnersYawRate;         /*          "                    left, right */
   sAttitude      gunnersJiggle;          /* jiggling factor for gunners */        
   sREAL          gunnersBulletRadius;    /* marksmanship factor for gunners */
protected:
    /* bullet radius -- major effect on marksmanship skill                    */
   sREAL          bulletRadiusMin;        /* minimum bullet radius            */
   sREAL          bulletRadiusMax;        /* maximum bullet radius            */
   sREAL          bulletRadius;           /* calc'd from above w/ skill level */          

   bool           isBombing;              /* if true, droppin' iron           */
   bool           groundCollisionOn;      /* detect ground collision          */
   bool           clearForTakeoff;        /* takeoff flag                     */
   bool           clearForLanding;        /* landing flag                     */
    /**********************************************************************
     * protected member functions                                         *
     **********************************************************************/
    /*
     *   Maneuver routines
     */
   void        DoStraightAndLevel(sManeuverState &ms);
   void        DoStandardTurn(sManeuverState &ms);
   void        DoBreakTurn(sManeuverState &ms);
   void        DoHardTurn(sManeuverState &ms);
   void        DoClimb(sManeuverState &ms);
   void        DoDescend(sManeuverState &ms);
   void        DoImmelman(sManeuverState &ms);
   void        DoSplitS(sManeuverState &ms);
   void        DoInvert(sManeuverState &ms);
   void        DoExtend(sManeuverState &ms);
   void        DoEngage(sManeuverState &ms);
   void        DoAlign(sManeuverState &ms);
   void        DoJink(sManeuverState &ms);
   void        DoRQEvade(sManeuverState &ms);
   void        DoRQEngage(sManeuverState &ms);
   void        DoFQEngage(sManeuverState &ms);
   void        DoPursuit(sManeuverState &ms);
   void        DoGunAttack(sManeuverState &ms);
   void        DoNavigate(sManeuverState &ms);
   void        DoFormation(sManeuverState &ms);
   void        DoPullUp(sManeuverState &ms);
   void        DoLevelRoll(sManeuverState &ms);
   void        DoPitchedRoll(sManeuverState &ms);
   void        DoWingOver(sManeuverState &ms);
   void        DoLevelBomb(sManeuverState &ms);
   void        DoIntercept(sManeuverState &ms);
   void        DoClimbingTurn(sManeuverState &ms);
   void        DoDescendingTurn(sManeuverState &ms);
   void        DoSnapRoll(sManeuverState &ms);
   void        DoChandelle(sManeuverState &ms);
   void        DoAirShow(sManeuverState &ms);
   void        DoAileronRoll(sManeuverState &ms);
   void        DoEvade(sManeuverState &ms);
   void        DoZoom(sManeuverState &ms);
   void        DoBarrelRoll(sManeuverState &ms);
   void        DoSpiralDive(sManeuverState &ms);
   void        DoTakeOff(sManeuverState &ms);
   void        DoLand(sManeuverState &ms);
   void        DoTaxi(sManeuverState &ms);
   /*
    *  Flight model control routines
    */
   void        UpdateRollCtl(sControlState &);
   void        UpdateRollCtlDirect(sControlState &);
   void        UpdateRollRateCtl(sControlState &);
   void        UpdateRollRateCtlDirect(sControlState &);

   void        UpdatePitchRateCtl(sControlState &);
   void        UpdatePitchRateCtlDirect(sControlState &);
   void        UpdatePitchCtl(sControlState &);
   void        UpdatePitchCtlDirect(sControlState &);

   void        UpdateYawRateCtl(sControlState &);
   void        UpdateYawRateCtlDirect(sControlState &);
   void        UpdateYawCtl(sControlState &);

   void        UpdateSpeedCtl(sControlState &);
   void        UpdateGCtl(sControlState &);
   void        UpdateVelCtl(sControlState &);
   void        ControlsOff();

   void        UpdateManeuver(sManeuverState &ms);

   int         flightModelUpdateFlag;     /* if true, update flight model */
    /*
     * Decide whether or not to shoot
     */
   virtual int InShootParams();
   virtual void Shoot() = 0;
    /*
     * Drop an egg
     */
    virtual void DropBomb() = 0;

    /*
     * Get hurting percentage
     */
    virtual sREAL GetDamagePer();

    /*
     *  Target info 
     */
   void GetAttitude(const sPoint &position, 
                        sVector &direction, 
                        sAttitude &attitude)
   {
      flightModel->GetAttitudeToPoint(position,direction,attitude);
   }
   void GetTargetFlags(sTarget &);
   virtual void GetNavigationGeometry(sNavInfo &ni);
   virtual void GetTargetGeometry(uint32_t targetIdx, sTargetGeometry &targetGeometry);
   virtual void CalcGunLeadPoint(sTargetGeometry &targetGeometry);
   void GetQuickTargetGeometry(uint32_t targetIdx, sTargetGeometry &tg);
   virtual void GetTargetInfo(uint32_t idx,   sTargetInfo &targetInfo);
   virtual void GetTargetFlags(sTargetFlags &, sTargetGeometry &);
   virtual void GetAttitudeToTarget(uint32_t targetIdx,
                                    sPoint &targetPosition,
                                    sAttitude &attitudeToTarget,
                                    sVector &directionToTarget,
                                    sVector &targetHeading,
                                    sVector &targetVelocity);
   virtual sREAL GetTargetTurnRate(uint32_t targetIdx);
   virtual sREAL GetTargetLoad(uint32_t targetIdx);
   virtual sREAL GetTargetAirSpeed(uint32_t targetIdx);
   virtual int GetTargetActive(uint32_t targetIdx);
   void GetSurfaceTargetGeometry(sSurfaceTarget &st);
   void CalcAlignPoint(sSurfaceTarget &st);
   virtual void GetWeaponLimits(sWeaponLimits &weaponLimits) = 0;
   void GetFormationPoint(int wingPos, int formationType, unsigned int leaderIdx, sPoint &offset);
   void ClearTargetPilot();
   void SetTargetPilot(uint32_t targetIdx);
   int EvalThreatFromPlayer();
   void SelectEngagementFightType(uint32_t targetIdx);
   void SetNoseOn(sREAL, sREAL, sREAL = Pi_2);
   void SetNoseOn(bool flag, const sPoint &, sREAL pitch, sREAL yaw);

   void SetNoseOnX(sREAL, sREAL, sREAL = 0.5, sREAL = 0.0, sREAL = static_cast<sREAL>(Pi_2));
   sREAL Point2Roll(const sPoint &rollPosition);
   sREAL CalcClosureSpeed(const sTargetGeometry &, int flg = 0);
   /* communication */
   virtual void Broadcast(unsigned int idx, int channel = commCHANNEL_AFFILIATION,
                          int priority = commPRIORITY_STANDARD,
                          void *extraInfo = NULL) = 0;

   /* get plane capabilities / control parameters */
   void CheckPlaneCaps(const char *model);
   /* get pilot capabilities */
   void CheckPilotCaps(const char *capsId);
  
   /*
    * Get yaw, pitch relative to horizon
    */
   void GetLevelAttitude(sAttitude &, sREAL = 0.0);

   /*
    *  Gunners
    */
   virtual void BuildGunners() =0;
   void AddGunner(aiGunner *newGunner);
   aiGunner *GetGunner(unsigned int whichGunner);
   void UpdateGunners();

   /* attacker list */
   sAttacker *CheckAttackerList();
   sAttacker *EvalAttackerList();
   void DistributeAttackers();
   sREAL EvalThreat(uint32_t idx, aiPilot **pil);

   void SETROLLPID(sREAL goal, sREAL dVel = 0.0, sREAL controlStep = 1.0)
   {
      rollCtl.on = 1;
      rollCtl.setPoint = goal;
      rollCtl.dVel = dVel;
      rollCtl.controlStep = controlStep;
      rollCtl.flag = 0;
   }
   void SETPITCHPID(sREAL goal, sREAL dVel = 0.0, sREAL controlStep = 1.0)
   {
      pitchCtl.on = 1;
      pitchCtl.setPoint = goal;
      pitchCtl.dVel = dVel;
      pitchCtl.controlStep = controlStep;
      pitchCtl.flag = 0;
   }
   void SETYAWPID(sREAL goal, sREAL dVel = 0.0, sREAL controlStep = 1.0)
   {
      yawCtl.on = 1;
      yawCtl.setPoint = goal;
      yawCtl.dVel = dVel;
      yawCtl.controlStep = controlStep;
      yawCtl.flag = 0;
   }
};
#endif
#endif
