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
 * File   : sfltmdl.h                            *
 * Date   : April, 1998                          *
 * Author : Dan Hammer                           *
 * An "abstract" flight model, for use with AI   *
 * or a human player. Defines an interface which *
 * allows the flight model to be controlled both *
 * directly (by setting roll rates, velocity,    *
 * etc), or indirectly (by setting control       *
 * inputs).                                      *
 *************************************************/
#ifndef __sfltmdl_h
#define __sfltmdl_h
#ifdef __cplusplus
#include <stdio.h>

#include "sslewer.h"

/*
 *  We will use a callback method where the flight model
 *  will call a provided function when it is at various
 *  points in it's update routine. These bit-flags are set
 *  in the call to SetUpdateFunction
 */
#define SFMCB_VFORCES_UPDATE      0x01     // call back on calc of vector forces
#define SFMCB_RFORCES_UPDATE      0x02     // call back on calc of rotational forces
#define SFMCB_POSITION_UPDATE     0x04     // call back on calc of position
#define SFMCB_ORIENTATION_UPDATE  0x08     // call back on calc of orientation
#define SFMCB_CONTROL_UPDATE      0x10     // call back to set flight controls

#define SFMCB_FORCES    SFMCB_VFORCES_UPDATE | SFMCB_RFORCES_UPDATE
#define SFMCB_POSITIONS  SFMCB_POSITION_UPDATE | SFMCB_ORIENTATION_UPDATE
#define SFMCB_ALL SFMCB_FORCES | SFMCB_POSITIONS | SFMCB_CONTROL_UPDATE

class sFlightModel : public sSlewer
{
protected:
  /* 
     This callback function allows various aspects of the flight
     model to be overridden to adjust, tweak or completely invalidate
     the calculations.
     */
  void (* updateCallback)(int sw, int iter, sFlightModel *fm, void *);
  /* flags indicating which updates the above function applies to */
  unsigned int callbackFlags;
  /* additional data the callbackee wishes to have passed */
  void *callbackData;
  /* current calculation iteration value */
  int iter;
public:
  /*
    values of sw in above callback indicating what's about to be updated
    VFORCES_UPDATE  = model is about to calculate vector forces from state info
    RFORCES_UPDATE  = model is about to calculate rotational forces from state info
    POSITION_UPDATE = model is about to position from vector velocities
    ORIENTATION_UPDATE = model is about to calculate orientation from rotational velocities
    CONTROL_UPDATE  = set flight control values for new iteration
    */
  enum { VFORCES_UPDATE, RFORCES_UPDATE, POSITION_UPDATE, ORIENTATION_UPDATE, CONTROL_UPDATE };
protected:
  void DoVForcesCallback();
  void DoRForcesCallback();
  void DoPositionCallback();
  void DoOrientationCallback();
  void DoControlCallback();
public:

  sFlightModel()
    {
      updateCallback = NULL;
      callbackFlags = 0;
      callbackData = NULL;
    }

  virtual ~sFlightModel()
    {}


  void SetUpdateCallback(unsigned int callbackFlags, void (* updateCallback)(int, int, sFlightModel *, void *),
									void *callbackData)
    {
      this->callbackFlags = callbackFlags;
      this->updateCallback = updateCallback;
      this->callbackData = callbackData;
    }

	/* Pure virtual functions */
	virtual void Update(double frame_time) = 0;
	virtual void Destroy() = 0;
	virtual sREAL GetPitchRate() = 0;
	virtual void SetPitchRate(sREAL pitchRate) = 0;
	virtual sREAL GetPitch() = 0;
	virtual void SetPitch(sREAL pitch) = 0;
	virtual sREAL GetRollRate() = 0;
	virtual void SetRollRate(sREAL rollRate) = 0;
	virtual sREAL GetRoll() = 0;
	virtual void SetRoll(sREAL roll) = 0;
	virtual sREAL GetYawRate() = 0;
	virtual void SetYawRate(sREAL yawRate) = 0;
	virtual sREAL GetYaw() = 0;
	virtual void SetYaw(sREAL yaw) = 0;
	virtual sREAL GetAOA() = 0;
	virtual sREAL GetAOARate() = 0;
	virtual void SetAOA(sREAL AOA) = 0;
	virtual sREAL GetLift() = 0;
	virtual void SetLift(sREAL lift) = 0;
	virtual sREAL GetAirSpeedFPS() = 0;
	virtual sREAL GetAirSpeedMPH() = 0;
	virtual void GetVelocity(sVector &vel) = 0;
	virtual void SetVelocity(const sVector &vel) = 0;
	virtual sREAL GetPitchControlPer() = 0;
	virtual void SetPitchControlPer(sREAL per) = 0;
	virtual void IncPitchControlPer(sREAL inc)  = 0;
	virtual sREAL GetRollControlPer()  = 0;
	virtual void SetRollControlPer(sREAL per)  = 0;
	virtual void IncRollControlPer(sREAL inc) = 0;
	virtual sREAL GetYawControlPer() = 0;
	virtual void SetYawControlPer(sREAL per) = 0;
	virtual void IncYawControlPer(sREAL inc) = 0;
	virtual sREAL GetEngineControlPer() = 0;
	virtual void SetEngineControlPer(sREAL per) = 0;
	virtual void IncEngineControlPer(sREAL per) = 0;
	virtual sREAL GetLoad() = 0;
	virtual sREAL GetLoadRate() = 0;
	virtual sREAL GetAltitudeFPS() = 0;
	virtual sREAL GetBestClimbSpeedFPS() = 0;
	virtual sREAL GetStallSpeedFPS() = 0;
	virtual sREAL GetMaxSpeedFPS() = 0;
	virtual sREAL GetThrust() = 0;
	virtual sREAL GetDrag() = 0;
	virtual sREAL GetWeight() = 0;
	virtual void GetWorldVelocity(sVector &worldVel) = 0;
	virtual void SetWEP(int wep) = 0;
	virtual int GetWEP() = 0;
	virtual void GetDirectionNormal(sVector &directionNormal) = 0;
	virtual sREAL GetTurnRate() = 0;
	virtual sREAL GetHeightAGL() = 0;
	virtual int GetEngineCount() = 0;
	virtual sREAL GetGroundHeight(const sPoint &) = 0;
	virtual sREAL GetPitchAccel() = 0;
	virtual sREAL GetYawAccel() = 0;
	virtual sREAL GetRollAccel() = 0;
	virtual void SetFlaps(sREAL per) = 0;
	virtual void SetSpeedBreaks(sREAL per) = 0;
	virtual void ExtendLandingGear() = 0;
	virtual void RetractLandingGear() = 0;
	virtual int GetGearPoints(sPoint *) = 0;
	virtual sREAL GetRestPitch() = 0;
	virtual const char *GetModel() = 0;
	virtual int ControlsProportional()
	{
		return 1;
	}
	virtual sREAL GetCornerSpeedFPS() = 0;
	virtual void SetWheelBrakes(int) = 0;
};
#endif
#endif
