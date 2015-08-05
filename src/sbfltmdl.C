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
 * File   : sbfltmdl.h                           *
 * Date   : April, 1998                          *
 * Author : Dan Hammer                           *
 * Wrap our Sabre flight model inside an         *
 * sFlightModel so AI can access it.             *
 *************************************************/
#include <stdio.h>
#include "simfile.h"
#include "sbfltmdl.h"
#include "aimath.h"

extern float time_frame;

#define STATE       flight->state
#define CONTROLS    flight->controls
#define FORCES		  flight->forces
#define SPECS		  flight->specs
#define FLIGHT_PORT STATE.flight_port
#define LOOK_FROM   FLIGHT_PORT.look_from
//#define LOOK_AT     FLIGHT_PORT.look_at
//#define FLIGHT_ROLL FLIGHT_PORT.roll

sbrFlightModel::sbrFlightModel(Flight *flit)
{
sPoint  position;
sAttitude attitude;

	this->flight = flit;
	R_3DPoint2sPoint(LOOK_FROM,position);
	Port2sAttitude(FLIGHT_PORT,attitude);
	sSlewer::SetPositionAndAttitude(position,attitude);
	turnRate = 0.0;
}

void sbrFlightModel::Update(double /* ignored */)
{
sPoint  position;
sAttitude attitude;

	flight->l_time = time_frame;
	if (flight->l_time <= 0.0)
		flight->l_time = C(0.1);
	flight->applyForces(flight->l_time);
	flight->applyRotations(flight->l_time);
	flight->calcState(flight->l_time);
	DoControlCallback();
	flight->calcForces(flight->l_time);
	flight->calcRotations(flight->l_time);

	/* call Flight::Update() */
	/* convert position and attitude */
	R_3DPoint2sPoint(LOOK_FROM,position);
	Port2sAttitude(FLIGHT_PORT,attitude);
	
	turnRate = (attitude.yaw - att.yaw) / time_frame;
	sSlewer::SetPositionAndAttitude(position,attitude);
}

void sbrFlightModel::Destroy()
{
}

void sbrFlightModel::SetPositionAndAttitude(const sPoint &position, const sAttitude &attitude)
{
sPoint	 lookAt;
R_3DPoint look_at;
R_3DPoint look_from;
Vector	 v;

	sSlewer::SetPositionAndAttitude(position,attitude);
	sPoint2R_3DPoint(position,look_from);
	lookAt = position + dirNormal;
	sPoint2R_3DPoint(lookAt,look_at);
	FLIGHT_PORT.set_view(look_from,look_at);
	FLIGHT_PORT.set_roll(CvToSabreAngle(attitude.roll));
	sVector2Vector(dirNormal,v);
	STATE.velocity.direction = v;
}

void sbrFlightModel::Rotate(const sAttitude &rotateMatrix)
{
sPoint  position;
sAttitude attitude;

	FLIGHT_PORT.rotate(CvToSabreAngle(rotateMatrix.pitch),
								CvToSabreAngle(rotateMatrix.roll),
								CvToSabreAngle(rotateMatrix.yaw));
	R_3DPoint2sPoint(LOOK_FROM,position);
	Port2sAttitude(FLIGHT_PORT,attitude);
	sSlewer::SetPositionAndAttitude(position,attitude);
}

void sbrFlightModel::SetPosition(const sPoint &position)
{
R_3DPoint look_from;

	sPoint2R_3DPoint(position,look_from);
	FLIGHT_PORT.set_look_from(look_from);
	sSlewer::SetPosition(pos);
}

void sbrFlightModel::SetAttitude(const sAttitude &attitude)
{
	SetPositionAndAttitude(pos,attitude);		
}

sREAL sbrFlightModel::GetPitchRate()
{
	return -STATE.pitch_rate;
}

void sbrFlightModel::SetPitchRate(sREAL pitchRate)
{
	STATE.pitch_rate = pitchRate;
}

sREAL sbrFlightModel::GetPitch()
{
	return att.pitch;
}

void sbrFlightModel::SetPitch(sREAL pitch)
{
sAttitude attitude = att;

	attitude.pitch = pitch;
	SetAttitude(attitude);		
}

sREAL sbrFlightModel::GetRollRate()
{
	return STATE.roll_rate;
}

void sbrFlightModel::SetRollRate(sREAL rollRate)
{
	STATE.roll_rate = rollRate;
}

sREAL sbrFlightModel::GetRoll()
{
	return att.roll;
}

void sbrFlightModel::SetRoll(sREAL roll)
{
sAttitude attitude = att;

	attitude.roll = roll;
	FLIGHT_PORT.set_roll(roll);
	sSlewer::SetPositionAndAttitude(pos,attitude);
//	SetAttitude(attitude);
}

sREAL sbrFlightModel::GetYawRate()
{
	return -STATE.yaw_rate;
}

void sbrFlightModel::SetYawRate(sREAL yawRate)
{
	STATE.yaw_rate = yawRate;
}

sREAL sbrFlightModel::GetYaw()
{
	return att.yaw;
}

void sbrFlightModel::SetYaw(sREAL yaw)
{
sAttitude attitude = att;

	attitude.yaw = yaw;
	SetAttitude(attitude);
}

sREAL sbrFlightModel::GetAOA()
{
	return STATE.angle_of_attack;
}

sREAL sbrFlightModel::GetAOARate()
{
	/*
	 * FIXME!!
	 */
	return 0.0;
}

void sbrFlightModel::SetAOA(sREAL AOA)
{
	STATE.angle_of_attack = AOA;
}

sREAL sbrFlightModel::GetLift()
{
	return FORCES.lift.magnitude;
}

void sbrFlightModel::SetLift(sREAL lift)
{
	FORCES.lift.magnitude = lift;
}

sREAL sbrFlightModel::GetAirSpeedFPS()
{
	return STATE.z_vel;
}

sREAL sbrFlightModel::GetAirSpeedMPH()
{
	return sFPS2MPH(STATE.z_vel);
}

void sbrFlightModel::GetVelocity(sVector &vel)
{
	Vector port_vel = STATE.vel_pv.to_vector();
	Vector2sVector(port_vel,vel);
}

void sbrFlightModel::GetWorldVelocity(sVector &worldVel)
{
	DVector worldVelD = to_vector(STATE.velocity);
	Vector worldV = worldVelD.to_vector();
	Vector2sVector(worldV,worldVel);
}

void sbrFlightModel::SetVelocity(__attribute__((unused)) const sVector &vel)
{
	/*
	 * FIXME!!
	 */
}

sREAL sbrFlightModel::GetPitchControlPer()
{
	return (CONTROLS.elevators / CONTROLS.elevator_max);
}

void sbrFlightModel::SetPitchControlPer(sREAL per)
{
	if (per > 1.0)
		per = 1.0;
	if (per < -1.0)
		per = -1.0;
	CONTROLS.elevators = static_cast<float>(per) * CONTROLS.elevator_max;
}

void sbrFlightModel::IncPitchControlPer(sREAL inc)
{
	SetPitchControlPer(GetPitchControlPer() + inc);
}

sREAL sbrFlightModel::GetRollControlPer()
{
	return CONTROLS.ailerons / CONTROLS.aileron_max;
}

void sbrFlightModel::SetRollControlPer(sREAL per)
{
	if (per > 1.0)
		per = 1.0;
	if (per < -1.0)
		per = -1.0;
	CONTROLS.ailerons = static_cast<float>(per) * CONTROLS.aileron_max;
}

void sbrFlightModel::IncRollControlPer(sREAL inc)
{
	SetRollControlPer(GetRollControlPer() - inc);
}

sREAL sbrFlightModel::GetYawControlPer()
{
	return CONTROLS.rudder / CONTROLS.rudder_max;
}

void sbrFlightModel::SetYawControlPer(sREAL per)
{
	if (per > 1.0)
		per = 1.0;
	if (per < -1.0)
		per = -1.0;
	CONTROLS.rudder = per * CONTROLS.rudder_max;
}

void sbrFlightModel::IncYawControlPer(sREAL inc)
{
	SetYawControlPer(GetYawControlPer() - inc);
}

sREAL sbrFlightModel::GetEngineControlPer()
{
	return CONTROLS.throttle * C(0.01);
}

void sbrFlightModel::SetEngineControlPer(sREAL per)
{
	if (per > 1.0)
		per = 1.0;
	if (per < 0.0)
		per = 0.0;
	CONTROLS.throttle = C(per * 100.0);
}

void sbrFlightModel::IncEngineControlPer(sREAL inc)
{
	SetEngineControlPer(GetEngineControlPer() + inc);
}

sREAL sbrFlightModel::GetLoad()
{
	return STATE.load;
}

sREAL sbrFlightModel::GetLoadRate()
{
	return STATE.d_load;
}

sREAL sbrFlightModel::GetAltitudeFPS()
{
	return FLIGHT_PORT.look_from.z / world_scale;
}

sREAL sbrFlightModel::GetBestClimbSpeedFPS()
{
	return SPECS->corner_speed;
}

sREAL sbrFlightModel::GetStallSpeedFPS()
{
	return SPECS->lspeed;
}

sREAL sbrFlightModel::GetMaxSpeedFPS()
{
	return SPECS->max_speed;
}

sREAL sbrFlightModel::GetCornerSpeedFPS()
{
	return SPECS->corner_speed;
}

sREAL sbrFlightModel::GetThrust()
{
	return FORCES.thrust.magnitude;
}

sREAL sbrFlightModel::GetDrag()
{
	return FORCES.drag.magnitude;
}

sREAL sbrFlightModel::GetWeight()
{
	return STATE.weight;
}

void sbrFlightModel::SetWEP(__attribute__((unused)) int wep)
{
	/*
	 * FIXME!!
	 */
}

int sbrFlightModel::GetWEP()
{
	return 0;
}

void sbrFlightModel::GetDirectionNormal(sVector &directionNormal)
{
	directionNormal = dirNormal;
}

sREAL sbrFlightModel::GetTurnRate()
{
	return turnRate;
}

sREAL sbrFlightModel::GetHeightAGL()
{
	return STATE.agl / world_scale;
}

int sbrFlightModel::GetEngineCount()
{
	/*
	 * FIXME!!
	 */
	return 1;
}

extern REAL_TYPE getGroundLevel(R_3DPoint &p);

sREAL sbrFlightModel::GetGroundHeight(const sPoint &s0)
{
R_3DPoint p0;

	sPoint2R_3DPoint(s0,p0);
	return getGroundLevel(p0) / world_scale;
}

sREAL sbrFlightModel::GetPitchAccel()
{
	return STATE.d_pitch_rate;
}

sREAL sbrFlightModel::GetYawAccel()
{
	return STATE.d_yaw_rate;
}

sREAL sbrFlightModel::GetRollAccel()
{
	return STATE.d_roll_rate;
}

void sbrFlightModel::SetFlaps(sREAL per)
{
	if (per > 1.0)
		per = 1.0;
	if (per < 0.0)
		per = 0.0;

	CONTROLS.flaps = per * CONTROLS.flaps_max;
}

void sbrFlightModel::SetSpeedBreaks(sREAL per)
{
	if (per > 0.5)
		CONTROLS.speed_brakes = 1;
	else
		CONTROLS.speed_brakes = 0;
}

void sbrFlightModel::ExtendLandingGear()
{
	CONTROLS.landing_gear = 1;
}

void sbrFlightModel::RetractLandingGear()
{
	CONTROLS.landing_gear = 0;
}

int sbrFlightModel::GetGearPoints(sPoint *)
{
	/* 
	 * FIXME!!
	 */
	return 0;
}

sREAL sbrFlightModel::GetRestPitch()
{
	/*
	 * FIXME!!
	 */
	return 0;
}

const char *sbrFlightModel::GetModel()
{
	return SPECS->model;
}

void sbrFlightModel::SetWheelBrakes(int engaged)
{
	CONTROLS.wheel_brakes = engaged;
}
