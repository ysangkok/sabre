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
#ifndef __sbfltmdl_h
#define __sbfltmdl_h

#ifdef __cplusplus
#include "simerr.h"
#include "simfile.h"
#include "vmath.h"
#include "port_3d.h"
#include "flight.h"
#include "sfltmdl.h"

class sbrFlightModel : public sFlightModel
{
protected:
	Flight *flight;
	sREAL   turnRate;
public:
	sbrFlightModel(Flight *flight);
	/*
	 * virtuals from sSlewer
	 */
	void SetPositionAndAttitude(const sPoint &position, const sAttitude &attitude);
	void Rotate(const sAttitude &rotateMatrix);
	void SetPosition(const sPoint &position);
	void SetAttitude(const sAttitude &attitude);
	/*
	 *  virtuals from sFlightModel
	 */
	void Update(double frame_time);
	void Destroy();
	sREAL GetPitchRate();
	void SetPitchRate(sREAL pitchRate);
	sREAL GetPitch();
	void SetPitch(sREAL pitch);
	sREAL GetRollRate();
	void SetRollRate(sREAL rollRate);
	sREAL GetRoll();
	void SetRoll(sREAL roll);
	sREAL GetYawRate();
	void SetYawRate(sREAL yawRate);
	sREAL GetYaw();
	void SetYaw(sREAL yaw);
	sREAL GetAOA();
	sREAL GetAOARate();
	void SetAOA(sREAL AOA);
	sREAL GetLift();
	void SetLift(sREAL lift);
	sREAL GetAirSpeedFPS();
	sREAL GetAirSpeedMPH();
	void GetVelocity(sVector &vel);
	void SetVelocity(const sVector &vel);
	sREAL GetPitchControlPer();
	void SetPitchControlPer(sREAL per);
	void IncPitchControlPer(sREAL inc) ;
	sREAL GetRollControlPer() ;
	void SetRollControlPer(sREAL per) ;
	void IncRollControlPer(sREAL inc);
	sREAL GetYawControlPer();
	void SetYawControlPer(sREAL per);
	void IncYawControlPer(sREAL inc);
	sREAL GetEngineControlPer();
	void SetEngineControlPer(sREAL per);
	void IncEngineControlPer(sREAL per);
	sREAL GetLoad();
	sREAL GetLoadRate();
	sREAL GetAltitudeFPS();
	sREAL GetBestClimbSpeedFPS();
	sREAL GetStallSpeedFPS();
	sREAL GetMaxSpeedFPS();
	sREAL GetThrust();
	sREAL GetDrag();
	sREAL GetWeight();
	void GetWorldVelocity(sVector &worldVel);
	void SetWEP(int wep);
	int GetWEP();
	void GetDirectionNormal(sVector &directionNormal);
	sREAL GetTurnRate();
	sREAL GetHeightAGL();
	int GetEngineCount();
	sREAL GetGroundHeight(const sPoint &);
	sREAL GetPitchAccel();
	sREAL GetYawAccel();
	sREAL GetRollAccel();
	void SetFlaps(sREAL per);
	void SetSpeedBreaks(sREAL per);
	void ExtendLandingGear();
	void RetractLandingGear();
	int GetGearPoints(sPoint *);
	sREAL GetRestPitch();
	const char *GetModel();
	sREAL GetCornerSpeedFPS();
	void SetWheelBrakes(int engaged);

	Flight *GetFlight(void)
	{
		return flight;
	}
	Port_3D *GetPort(void)
	{
		return & GetFlight()->state.flight_port;
	}
	int ControlsProportional()
	{
		return 0;
	}
};

#endif /* __cplusplus */


#endif