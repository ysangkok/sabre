#ifndef __aipildef_h
#define __aipildef_h

/* ground collision defaults */
#define aiPILOT_GCOL_SECS			       10.0
#define aiPILOT_GCOL_AGL			       2000.0
#define aiPILOT_GCOL_RAGL			       2000.0
#define aiPILOT_GCOL_ANGLE		       (_degree * 10.0)

/* collision avoidance default */
#define aiPILOT_COLLISION_AVOID_SECS		1.5

/* g limit default */
#define aiPILOT_GLIMIT                  6.0
#define aiPILOT_GLIMIT_NEG              -3.0

/* roll limit default */
#define aiPILOT_ROLL_LIMIT              0.8
/* roll multiplier default */
#define aiPILOT_ROLL_MULT               3.0
/* extend angle default*/
#define aiPILOT_EXTEND_ANGLE            10.0
/* gun attack params defaults */
#define aiPILOT_GUNATTACK_DISTANCE_MUL  1.2
/* player threat defaults */
#define aiPILOT_PLAYERTHREAT_DISTANCE   3000.0
#define aiPILOT_PLAYERTHREAT_ASPECT     (_degree * 12.0)
#define aiPILOT_PLAYERTHREAT_VALUE		1.0
/* max speed multiplier */
#define aiPILOT_ENGAGE_MAX_SPEED_MUL    0.8
#define aiPILOT_FORMATION_OFFSET        300.0
 
/* default time interval for checking attacker list */
#define aiPILOT_ATTKRCHECK_INTERVAL  12.0

/* circling speed, bank angle default */
#define aiPILOT_CIRCLE_BANK_DEG			30.0
#define aiPILOT_CIRCLE_SPEED_MPH  	  180.0
/* landing defaults							*/
#define aiPILOT_FINAL_APPROACH_DISTANCE		6000.0
#define aiPILOT_FINAL_APPROACH_AGL			1000.0

#define aiPILOT_BULLET_RADIUS_MIN      1.6
#define aiPILOT_BULLET_RADIUS_MAX		  4.6

#define aiPILOT_GUNNER_BULLET_RADIUS   2.6
#define aiPILOT_GUNNER_JIGGLE			  (_degree * 0.1)

#define aiPILOT_IMMELMAN_GS				  3.0
#define aiPILOT_IMMELMAN_ALT_MIN		  800.0
#define aiPILOT_SPLIT_S_GS				  4.0
#define aiPILOT_SPLIT_S_ALT_MIN		  2000.0
#define aiPILOT_NOSEON_ROLL_LIMIT		  (Pi_4 * 1.3)
#define aiPILOT_SPEED_LOSS_LIMIT		  200.0
#define aiPILOT_LEAD_ADJUST_MULT			4.0
#define aiPILOT_ZOOM_ANGLE					60.0

#define aiPILOT_SPEED_P         0.1
#define aiPILOT_SPEED_I         0.01
#define aiPILOT_SPEED_D         0.1
#define aiPILOT_TC_P            0.1
#define aiPILOT_TC_I           10.0
#define aiPILOT_TC_D           0.01             

#ifdef __cplusplus

#include "sfrmtn.h"

/* defines for target data */
#define TARGET_GEOMETRY			curTarget->geometry
#define TARGET_INFO				curTarget->info
#define TARGET_IDX				curTarget->idx
#define TARGET_FLAGS				curTarget->flags
#define TARGET_ACTIVE			curTarget->active
#define TARGET_RANGE				TARGET_GEOMETRY.range
#define TARGET_DRANGE			TARGET_GEOMETRY.dRange
#define TARGET_YAW				TARGET_GEOMETRY.relAttitude.yaw
#define TARGET_ROLL				TARGET_GEOMETRY.relAttitude.roll
#define TARGET_PITCH				TARGET_GEOMETRY.relAttitude.pitch
#define TARGET_ASPECT			TARGET_GEOMETRY.aspect
#define TARGET_VELOCITY			TARGET_GEOMETRY.worldVelocity
#define TARGET_POSITION			TARGET_GEOMETRY.worldPosition
#define TARGET_ALTITUDE			TARGET_GEOMETRY.worldPosition.z
#define LEAD_YAW					TARGET_GEOMETRY.leadAttitude.yaw
#define LEAD_PITCH				TARGET_GEOMETRY.leadAttitude.pitch
#define LEAD_YAW_RATE			TARGET_GEOMETRY.leadRate.yaw
#define LEAD_PITCH_RATE			TARGET_GEOMETRY.leadRate.pitch
#define LEAD_POSITION         TARGET_GEOMETRY.leadPoint
#define LAG_YAW					TARGET_GEOMETRY.lagAttitude.yaw
#define LAG_PITCH					TARGET_GEOMETRY.lagAttitude.pitch
#define LAG_POSITION          TARGET_GEOMETRY.lagPoint
#define GUN_LEAD_YAW				TARGET_GEOMETRY.gunLeadAttitude.yaw
#define GUN_LEAD_PITCH			TARGET_GEOMETRY.gunLeadAttitude.pitch
#define GUN_LEAD_POSITION		TARGET_GEOMETRY.gunLeadPoint
#define GUN_TOF					TARGET_GEOMETRY.gunTOF
#define TARGET_AIR_SPEED		TARGET_GEOMETRY.airSpeed
#define TARGET_MAX_SPEED		TARGET_INFO.maxSpeed
#define TARGET_WEAPONS_RANGE	TARGET_INFO.weaponsRange
#define TARGET_WEAPONS_EFFECT	TARGET_INFO.weaponsEffect
#define GUN_LEAD_RANGE_SQ		TARGET_GEOMETRY.gunLeadRangeSq
#define TARGET_THREAT_VALUE	curTarget->threatValue

#define TARGET_CAN_ENERGY_FIGHT	TARGET_INFO.tacticalCaps[sPilotTactics::ENERGY_FIGHT]
#define TARGET_CAN_ANGLES_FIGHT	TARGET_INFO.tacticalCaps[sPilotTactics::ANGLES_FIGHT]

/* defines for formation flying */
#define FORMATION_YAW		TARGET_GEOMETRY.formationAttitude.yaw
#define FORMATION_PITCH		TARGET_GEOMETRY.formationAttitude.pitch
#define FORMATION_RANGE		TARGET_GEOMETRY.formationRange
#define FORMATION_DRANGE   TARGET_GEOMETRY.formationDRange

/* defines for tactics */
#define PURSUIT_DISTANCE			tactics.pursuitDistance
#define PURSUIT_CONE					tactics.pursuitCone
#define PURSUIT_CANCEL				tactics.cancelPursuit
#define HARD_TURN_ALTLOSS			tactics.hardTurnAltThreshold
#define HARD_TURN_SPEEDLOSS		tactics.hardTurnSpeedThreshold
#define REAR_THREAT_DISTANCE		tactics.rearThreatDistance
#define REAR_THREAT_CONE			tactics.rearThreatCone
#define WEAPON_THREAT_DISTANCE	tactics.weaponThreatDistance
#define HEADON_ASPECT				tactics.aspectHeadOnCone
#define TAIL_ASPECT					tactics.aspectRearCone
#define IS_ENERGY_FIGHT				(tactics.curTactic == sPilotTactics::ENERGY_FIGHT)
#define IS_ANGLES_FIGHT				(tactics.curTactic == sPilotTactics::ANGLES_FIGHT)
#define IS_WEENIE_FIGHT				(tactics.curTactic == sPilotTactics::WEENIE_FIGHT)
#define CAN_ENERGY_FIGHT			(tactics.TacticAvail(sPilotTactics::ENERGY_FIGHT) == 1)
#define CAN_ANGLES_FIGHT			(tactics.TacticAvail(sPilotTactics::ANGLES_FIGHT) == 1)
#define CAN_STRAFE					(tactics.TacticAvail(sPilotTactics::STRAFE) == 1)
#define CAN_ROCKET					(tactics.TacticAvail(sPilotTactics::ROCKET) == 1)
#define CAN_DIVE_BOMB				(tactics.TacticAvail(sPilotTactics::DIVE_BOMB) == 1)
#define CAN_TORPEDO_BOMB			(tactics.TacticAvail(sPilotTactics::TORPEDO_BOMB) == 1)
#define CAN_LIGHT_BOMB				(tactics.TacticAvail(sPilotTactics::LIGHT_BOMB) == 1)
#define CAN_HEAVY_BOMB				(tactics.TacticAvail(sPilotTactics::HEAVY_BOMB) == 1)
#define ENGAGE_DISTANCE				tactics.engageDistance

/* defines for navigation data */
#define NAV_PITCH			navInfo.relAttitude.pitch
#define NAV_YAW			navInfo.relAttitude.yaw
#define NAV_RANGE			navInfo.range
#define NAV_ALTITUDE		navInfo.waypoint.altitude
#define NAV_ALT_THRESH	100

#define DEFLT_FORMATION_WINGLEN    50.0
#define DEFLT_ENGAGEMENT_MIN_SPEED 150.0

/* defines for surface target */
#define SURFACE_TARGET_YAW    surfaceTarget.relAttitude.yaw
#define SURFACE_TARGET_PITCH  surfaceTarget.relAttitude.pitch
#define SURFACE_TARGET_RANGE  surfaceTarget.range
#define SURFACE_TARGET_DRANGE surfaceTarget.dRange
#define SURFACE_TARGET_VDIST  surfaceTarget.vertDistance

/* timer indexes */
#define ATTACKER_CHECK_TIMER           0
#define PLAYER_MESSAGE_TIMER			   1

inline sREAL ANGELS(aiPilot *pil)
{
	return (pil->GetAltitudeFPS() / 1000.0);
}

#define ISINVERTED (fabs(flightModel->GetRoll()) > Pi_2)
#define ATTACKER_CHECK ( timers[ATTACKER_CHECK_TIMER] = attackerCheckInterval + 1.0 )

#define BREAK_TURN_ANGLE			80

#endif  /* __cplusplus */
#endif
