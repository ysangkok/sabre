/*
    Sabre Fighter Plane Simulator 
    Copyright (c) 1997 Dan Hammer
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
 *************************************************
 *           SABRE Fighter Plane Simulator       *
 * Version: 0.1                                  *
 * File   : flight.C                             *
 * Date   : March, 1997                          *
 * Author : Dan Hammer                           *
 *************************************************
 * Notes 12/16/95
 * Some progress made on getting pitch & yaw to work in
 * plane coordinants (i.e., if you pitch up and the plane
 * is rolled 90 degrees, you should move parallel to the
 * horizon.
 * The remaining problem is getting the roll correct after
 * the pitch/yaw.
 *
 * There are two methods to deal with this, flagged by
 * 'roll_method'. When 1, a quick, easy solution is used
 * but no attempt made to correct roll. When 0, a more
 * expensive method is used, and the roll is corrected except
 * when phi is near 0 ... which needs more research
 *
 *
 * 06/16/96 Finally got the rotations to work properly. Not
 * very pretty, but it will do for now.
 *
 *
 * 01/04/97 My father suggested that the angle of attack above which
 * stall occurs be a function of velocity. Well, heck if I'm 
 * not going to listen to my father!
 *
 *
 * 02/10/98
 * combined flight.C & flt_cmpl.C, eliminated Flight_Complex subclass
 *
 * 12/08/98
 * Sorry Dad, but according to all available expert testimony, stall
 * ain't no function of velocity
 */
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <limits.h>
#include <values.h>
#include "defs.h"
#include "pc_keys.h"
#include "simerr.h"
#include "simfile.h"
#include "vmath.h"
#include "port_3d.h"
#include "flight.h"
#include "rtkey.h"

int no_g = 0;
int no_f = 0;
int no_r = 0;
int no_l = 0;
int no_d = 0;

#define INT_PERC(x) (((float)x) / 100.0)
extern REAL_TYPE getGroundLevel(R_3DPoint &);

REAL_TYPE Flight::rot_acc_max = 24.0;
REAL_TYPE Flight::rot_rate_max = _PI34;
REAL_TYPE Flight::rot_max = _PI2 * 0.4;

void Flight::read(std::istream &is)
{
  char c;
  READ_TOKI('{',is,c);
  is >> controls >> state >> c;
  READ_TOK('}',is,c);
  state.pitch_rate = state.yaw_rate = state.roll_rate = 0.0;
  forces.pitch_acc = forces.yaw_acc = forces.roll_acc = 0.0;
  state.flight_port.delta_look_from(Vector(0,0,specs->g_height));
  state.weight = specs->weight;
}

void Flight::write(std::ostream &os)
{
  os << "{\n " << controls << " " << state << "\n}\n";
}


void Flight::start()
{
  l_time = 0.1;
  state.elapsed_time = 0.0;
  calcState(l_time);
  calcForces(l_time);
  calcRotations(l_time);
}

void Flight::pause()
{
}

void Flight::update()
{
  // Get elapsed time in microseconds
  l_time = time_frame;
  if (l_time <= 0.0)
    l_time = 0.1;
  if (!no_f)
    applyForces(l_time);
  // Calculate & apply rotations
  if (!no_r)
    applyRotations(l_time);
  // Calc forces for next iteration
  calcState(l_time);
  calcForces(l_time);
  calcRotations(l_time);
}

// Calculate some of the aspects of
// the flight, i.e. heading, angle_of_attack, & yaw
void Flight::calcState(float t)
{
  R_KEY_BEGIN(332)
  float  aoa;
  float  d;

  if (t <= 0.0)
    t = 0.1;

  state.elapsed_time += t;

  // Convert the velocity vector, which is in world coordinants,
  // to port coordinants
  state.vel_pv = to_port(state.velocity.direction);
  // Separate out the velocity occuring along the axis of the plane
  // (which is the z component in port coords)
  float zvl = state.velocity.magnitude * state.vel_pv.Z;
  state.delta_z_vel = (zvl - state.z_vel) / t;
  state.z_vel = zvl;
  // Get climb rate
  zvl = state.velocity.magnitude * state.velocity.direction.Z;
  state.delta_climb_vel = (zvl - state.climb_vel) / t;
  state.climb_vel = zvl;
  // Determine angle of attack, which is the angle formed between
  // the axis of the plane and the velocity vector, in the vertical plane
  if (state.velocity.magnitude > 0.01)
    {
      float  z_sq = state.vel_pv.Z * state.vel_pv.Z;
      aoa = state.vel_pv.Y / (sqrt ( (state.vel_pv.Y * state.vel_pv.Y) + z_sq ) );
      state.angle_of_attack = asin(-aoa);
      state.yaw = state.vel_pv.X / ( sqrt ( (state.vel_pv.X * state.vel_pv.X) + z_sq ) );
      //		state.yaw = acos(state.yaw);
    }
  else
    {
      state.angle_of_attack = 0.0;
      state.yaw = 0.0;
    }
  // Determine heading
  state.h_direction = DVector(state.flight_port.look_at.x -
			      state.flight_port.look_from.x,
			      state.flight_port.look_at.y -
			      state.flight_port.look_from.y,
			      0.0);
  state.h_direction.Normalize();
  state.heading = atan2(state.h_direction.Y + eps,state.h_direction.X + eps);
  // Make heading 0 <= h < 2pi
  if (state.heading < 0.0)
    state.heading = 6.2831853 + state.heading;

  // Set inverted flag
  state.negative_phi = 0;
  state.negative_roll = 0;
  if ((state.flight_port.slook_from.phi < _2PI) &&
      (state.flight_port.slook_from.phi > _PI))
    state.negative_phi = 1;
  if ((state.flight_port.roll > _PI2) &&
      (state.flight_port.roll < _PI34))
    state.negative_roll = 1;
  if (state.negative_roll)
    {
      if (state.negative_phi)
	state.inverted = 0;
      else
	state.inverted = 1;
    }
  else if (state.negative_phi)
    {
      if (state.negative_roll)
	state.inverted = 0;
      else
	state.inverted = 1;
    }
  else
    state.inverted = 0;

  /* calc ground level, agl, and deltas */
  d = getGroundLevel(state.flight_port.look_from);
  state.d_ground_height = (d - state.ground_height) / t;
  state.ground_height = d;
  d = state.flight_port.look_from.z - state.ground_height;
  state.d_agl = d - state.agl;
  state.d_agl /= t;
  state.agl = d;


  if (state.flight_port.look_from.z <= state.ground_height + specs->g_height)
    state.on_ground = 1;
  else
    {
      state.on_ground = 0;
      state.was_airborne = 1;
    }

  float load = forces.lift.magnitude / state.weight;
  if (forces.lift.direction.Y < 0.0)
    load = -load;
  state.d_load = (load - state.load) / t ;
  state.load = load;

  float d1 = state.flight_port.look_from.z / world_scale;

  if (no_l)
    state.air_density = 1.0;
  else
    {
  if (d1 <= 1000.0)
    state.air_density = 1.0;
  else if (d1 >= 100000.0)
    state.air_density = 0.05;
  else
    state.air_density = 1.0 - (d1 / 200000.0) + 0.10;
    }

  R_KEY_END
    }


// Calculate the forces acting on the flight.
// We assume a viewpoint equivalent to that obtained by
// the port.world2port transformation. From this vantage
// point, thrust occurs along the z-axis, lift along the y-axis
void Flight::calcForces(float)
{
  R_KEY_BEGIN(333) // Flight::calcForces

  float aoa_drg;
  float yaw_drg;
  float eff_aoa;
  float a_aoa;

  float vel_sq = state.velocity.magnitude * state.velocity.magnitude;
  float z_vel_sq = state.z_vel * state.z_vel;
  if (state.z_vel < 0)
    z_vel_sq = -z_vel_sq;
  if (vel_sq < 1.0)
    vel_sq = 1.0 + (1.0 - vel_sq);

  /***********************************************************
   *                   thrust                                *
   ***********************************************************/
  forces.thrust.magnitude = specs->max_thrust *
    (controls.throttle * 0.01 ) *
    mods.engine_e *
    state.air_density;
  forces.thrust.direction = DVector(0,0,1);


  /***********************************************************
   *                   drag                                  *
   ***********************************************************/
  forces.drag.magnitude = 0.0;
  float add_drag = 0.0;

  if (!no_d && state.velocity.magnitude > 0.1)
    {
      aoa_drg = (fabs(state.angle_of_attack) + specs->wing_aoa) *
	specs->drag_aoa;
      yaw_drg = fabs(state.yaw) * specs->drag_yaw;
      add_drag += (aoa_drg + yaw_drg) * vel_sq;
      if (state.on_ground)
	{
	  add_drag += specs->drag_whls * vel_sq;
	  if (!controls.landing_gear)
	    add_drag += specs->drag_whls * 100 * vel_sq;
	  if (controls.wheel_brakes && state.velocity.magnitude <=
	      specs->max_wb)
	    add_drag += specs->drag_wb * vel_sq * mods.wheel_brakes_e;
	}
      if (controls.speed_brakes)
	add_drag += specs->drag_sb * z_vel_sq * mods.speed_brakes_e;
      if (controls.landing_gear)
	add_drag += specs->drag_gr * vel_sq;
      if (controls.flaps)
	add_drag += ((float)controls.flaps) * specs->flap_drag * z_vel_sq;
      // Parasitic drag - proportional to velocity square
      forces.drag.magnitude = vel_sq * specs->drag_factor * mods.drag_e;
      // Induced drag - proportional to lift and inversely proportional to velocity square
      forces.drag.magnitude += ((specs->idrag_factor * fabs(forces.lift.magnitude) ) 
				/ vel_sq ) * mods.drag_e;
      // Add additional sources of drag
      forces.drag.magnitude += add_drag * mods.drag_e;
      forces.drag.magnitude *= state.air_density;
    }
  // Make the drag direction opposite that of velocity
  forces.drag.direction = to_port(state.velocity.direction);
  forces.drag.direction.Z *= -1.0;
  forces.drag.direction.Y *= -1.0;
  forces.drag.direction.X *= -1.0;
  forces.drag.direction.Normalize();


  /***********************************************************
   *                   lift                                  *
   ***********************************************************/
  // Have stall angle be a function of velocity (... thanks Dad!)
  // 12/08/98 Sorry Dad, but it ain't

  state.stalled = 0;
  eff_aoa = specs->wing_aoa + state.angle_of_attack;
  a_aoa = fabs(eff_aoa);
  /* mx_aoa = specs->max_aoa_factor * state.velocity.magnitude;
  if (mx_aoa > specs->max_aoa)
    mx_aoa = specs->max_aoa;
	 */

  if (a_aoa <= specs->max_aoa)
    {
      state.near_stall = specs->max_aoa - a_aoa;
      forces.lift.magnitude = z_vel_sq * specs->lift_factor * eff_aoa *
	mods.wing_e;
      if (controls.flaps)
	forces.lift.magnitude += z_vel_sq * ((float)controls.flaps)
	  * specs->flap_lift * mods.flaps_e;
      forces.lift.magnitude *= state.air_density;
    }
  else
    {
      /* 
	 This is a very abrupt stall, admittedly, but
         I wanted it to be dramatic. Better, the lift.magnitude
         should be reduced by some factor
	 */

      forces.lift.magnitude = 0.0;
      state.near_stall = 0.0;

      if (z_vel_sq > 0.1)
	state.stalled = 1;
    }
  if (forces.lift.magnitude < 0)
    {
      forces.lift.magnitude = -forces.lift.magnitude;
      forces.lift.direction = DVector(0,-1,0);
    }
  else
    forces.lift.direction = DVector(0,1,0);
  /* 
     Wing loading is limited here ...
     */
  // Limit load
  /* Pierre de Mouveaux (pmx) 14 nov 1999. Removed limit on load. */ 
  //  if ((forces.lift.magnitude / state.weight) > specs->load_limit)
  //    forces.lift.magnitude = specs->load_limit * state.weight;


  /***********************************************************
   *                   weight                                *
   ***********************************************************/
  if (no_g)
    forces.g.magnitude = 0;
  else
    forces.g.magnitude = state.weight;
  forces.g.direction = DVector(0,0,-1);

  R_KEY_END
    }

/********************************************
 * Calculate rotational forces on the flight*
 ********************************************/
void Flight::calcRotations(float )
{
  R_KEY_BEGIN(330) // Flight::calcRotations
    float torque,drag_torque;
  float eff_aoa;
  float eff_yaw;

  if (no_l)
    state.vel_factor = 1.0;
  else
    {
      state.vel_factor = fabs(state.velocity.magnitude) 
	/ specs->corner_speed;
      if (state.vel_factor > 1.0)
	state.vel_factor = 1.0;
      else if (state.vel_factor < 0.0)
	state.vel_factor = 0.0;
    }
  eff_aoa = state.angle_of_attack;
  eff_yaw = state.yaw;

  // Pitch

  float e_t = controls.elevators + controls.trim;
  // calc tendency of plane to return to 0 angle-of-attack
  torque = specs->return_pitch * eff_aoa * mods.h_stab_e;
  // calc control position of elevators
  // Don't allow plane to pitch down when on ground

  if (state.on_ground)
    {
      if (e_t > 0)
	torque += -e_t * specs->control_pitch * mods.elevators_e;
    }
  else
    torque += -e_t * specs->control_pitch * mods.elevators_e;

  drag_torque = state.pitch_rate * state.pitch_rate * specs->pitch_drag *
    mods.drag_e;
  if (state.pitch_rate < 0)
    drag_torque = -drag_torque;

  forces.pitch_acc = torque - drag_torque + mods.pitch_out;
  forces.pitch_acc *= state.air_density * state.vel_factor;
  

  // Yaw

  torque = specs->return_yaw * eff_yaw * mods.v_stab_e;

  if (controls.rudder)
    torque += controls.rudder * specs->control_yaw * mods.rudder_e;
  if (specs->adv_yaw != 0.0 && controls.ailerons &&
      !state.on_ground && !controls.auto_coord)
    torque += specs->adv_yaw * controls.ailerons;
  routine_key = 3307;
  drag_torque = state.yaw_rate * state.yaw_rate * specs->yaw_drag *
    mods.drag_e;
  if (state.yaw_rate < 0.0)
    drag_torque = -drag_torque;

  forces.yaw_acc = torque - drag_torque + mods.yaw_out;
  forces.yaw_acc *= state.air_density * state.vel_factor;

  // Roll
  forces.roll_acc = 0;
  // Don't allow plane to roll when on ground
  if (!state.on_ground)
    {


 //  state.negative_roll
   	

/* Pierre de Mouveaux (pmx) 14 nov 1999. */ 
// Dihedral effect on roll ...
// reflects tendency of ac to return to 0 degree bank

     float roll; 
     if (state.negative_roll) 
       roll = state.flight_port.roll - _PI;
     else 
       roll = state.flight_port.roll;
     if (roll > _PI)
       roll = -(_2PI-roll);
     float sign = (roll>=0.0)?1.0:-1.0;
     torque = 0.0;
     if ( (roll < .2) &&  (roll > -.2)){
       torque = -roll * specs->dihedralX;			// Constant, plane dependant (shouf be...)
     } else if ( (roll < .4) &&  (roll > -.4)){
         torque = -(sign*specs->dihedralY - roll) * specs->dihedralZ;
         //if (fabs(roll)> 0.01)	// Simplest way to do this...
         // torque = -.2*sign;  	
      }
/**/ 
//	torque = (state.negative_roll?.2:-.2);
//	torque = state.flight_port.roll;
      torque += controls.ailerons * specs->control_roll * mods.ailerons_e;
      if (specs->adv_roll != 0.0 && !controls.auto_coord)
	torque -= specs->adv_roll * state.yaw;
      if (state.stalled && specs->stall_spin != 0.0)
	torque += specs->stall_spin;
      drag_torque = state.roll_rate * state.roll_rate * specs->roll_drag *
	mods.drag_e;
      if (state.roll_rate < 0.0)
	drag_torque = -drag_torque;
      forces.roll_acc = torque - drag_torque + mods.spin_out;
      forces.roll_acc *= state.air_density * state.vel_factor;
      LIMIT(forces.roll_acc,rot_acc_max);
    }
  else
    forces.roll_acc = state.roll_rate = 0.0;



    R_KEY_END
    }

/*************************************************************
 * apply "vector" forces                                     *
 *************************************************************/
void Flight::applyForces(float t)
{
R_KEY_BEGIN(5)
Vector_Q sum_forces,acc;
Vector_Q tmp1,tmp2;
Vector delta;
DVector  vc;
DVector world_v;
// Flag which is set when the 'normal' force is active
int no_z = 0;

	// Add up all 'port coordinant' forces
	sum_forces.direction = to_vector(forces.thrust)
									+ to_vector(forces.drag)
									+ to_vector(forces.lift);
	sum_forces.magnitude = sum_forces.direction.Magnitude();
	if (sum_forces.magnitude > 0)
	{
		sum_forces.direction.X /= sum_forces.magnitude;
		sum_forces.direction.Y /= sum_forces.magnitude;
		sum_forces.direction.Z /= sum_forces.magnitude;
	}
	else
		sum_forces.direction.X = sum_forces.direction.Y =
			sum_forces.direction.Z = 0;

	// Convert to world coordinants & add in gravity
	world_v = to_world(sum_forces.direction);
	sum_forces.direction = world_v;
	sum_forces = sum_forces + forces.g;
	// If on ground, apply normal force
	if (state.on_ground)
	{
		no_z = 1;
		// record landing parameters
		if (state.was_airborne)
		{
			state.landing_z = state.climb_vel;
			state.landing_velocity = state.z_vel;
			state.landing_aoa = state.angle_of_attack;
			state.landing_pitch = state.flight_port.slook_from.phi;
			state.landing_roll = state.flight_port.roll;
			state.was_airborne = 0;
			state.landing_recorded = 1;
		}
		if (sum_forces.direction.Z < 0)
		{
			vc = to_vector(sum_forces);
			vc.Z = 0;
			sum_forces.magnitude = vc.Magnitude();
			vc.Normalize();
			sum_forces.direction = vc;
		}
	}


	// Determine acceleration
	// a = F / m
	float mass = state.weight / g;
	acc.magnitude = sum_forces.magnitude / mass;
	acc.direction = sum_forces.direction;
	DVector delta_v;

	// Apply to position
	// s = v0t + 1/2at^2
	tmp1 = state.velocity;
	tmp1.magnitude *= t;
	tmp2 = acc;
	tmp2.magnitude *= (t * t);
	tmp2.magnitude /= 2;
	tmp1 = tmp1 + tmp2;
	delta_v = to_vector(tmp1);
	delta_v *= world_scale;
	delta = delta_v.to_vector();
	state.flight_port.delta_look_from(delta);
	if (no_z && delta.Z < 0)
		delta.Z = 0;
	// Calculate new velocity
	// v = v0 + at
	tmp1 = acc;
	tmp1.magnitude *= t;
	state.velocity = state.velocity + tmp1;
	if (state.velocity.magnitude >= specs->max_speed)
		state.velocity.magnitude = specs->max_speed;
	// Kill any negative
	if (state.velocity.magnitude < 0.0)
	state.velocity.magnitude = 0.0;
	if (no_z && state.velocity.direction.Z < 0)
	{
		state.velocity.magnitude -= (state.velocity.magnitude * state.velocity.direction.Z);
		state.velocity.direction.Z = 0;
		if (state.flight_port.look_from.z < state.ground_height + specs->g_height)
			state.flight_port.delta_look_from(Vector(0,0,
														(state.ground_height + specs->g_height)
														- state.flight_port.look_from.z));
	}
	if (no_z)
	{
		float aX;
		DVector v = to_port(state.velocity.direction);
		aX = fabs(v.X);
		if (aX > 0.0)
		{
			state.velocity.magnitude -= state.velocity.magnitude * aX;
			v.X = 0.0;
			state.velocity.direction = to_world(v);
		}
	}
	if (state.on_ground	&& controls.wheel_brakes && 
			state.velocity.magnitude <= specs->max_wb)
		state.velocity.magnitude = DAMP(state.velocity.magnitude,specs->wb_damp * t);
R_KEY_END
}


/*************************************************************
 * apply "rotational" forces                                 *
 *************************************************************/
void Flight::applyRotations(float  t )
{
R_KEY_BEGIN(334)
float pitch,yaw,roll;
float t_sq;
float prate,yrate,rrate;

	prate = state.pitch_rate;
	yrate = state.yaw_rate;
	rrate = state.roll_rate;
	t_sq =  t * t;

	/*************************
	* calc pitch            *
	*************************/
	pitch = state.pitch_rate * t + 0.5 * t_sq * forces.pitch_acc;
	state.pitch_rate += forces.pitch_acc * t;
	LIMIT(pitch,rot_max);

	/***********************
	* calc yaw            *
	***********************/
	yaw = state.yaw_rate * t + 0.5 * t_sq * forces.yaw_acc;
	state.yaw_rate += forces.yaw_acc * t;
	LIMIT(yaw,rot_max);

	/**********************
	* calc roll          *
	**********************/
	if (!state.on_ground)
	{
		roll = state.roll_rate * t + 0.5 * t_sq * forces.roll_acc;
		state.roll_rate += forces.roll_acc * t;
		LIMIT(roll,rot_max);
	}
	else
		roll = 0.0;

	state.flight_port.rotate(pitch,roll,yaw);

	if (state.on_ground)
	{
		if (state.negative_phi)
			state.flight_port.set_roll(_PI);
		else
			state.flight_port.set_roll(0);
		state.roll_rate = 0.0;
		forces.roll_acc = 0.0;
	}


	/**********************
	* damp rates         *
	**********************
	LIMIT(state.pitch_rate,rot_rate_max);
	LIMIT(state.yaw_rate,rot_rate_max);
	LIMIT(state.roll_rate,rot_rate_max);
	*/

	state.pitch_rate = DAMP(state.pitch_rate,specs->pitch_damp * t);
	state.yaw_rate = DAMP(state.yaw_rate,specs->yaw_damp * t);
	state.roll_rate = DAMP(state.roll_rate,specs->roll_damp * t);

	// Get change in rotational rates
	state.d_pitch_rate = (state.pitch_rate - prate) / t;
	state.d_yaw_rate = (state.yaw_rate - yrate) / t;
	state.d_roll_rate = (state.roll_rate - rrate) / t;
R_KEY_END
}

