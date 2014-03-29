/*
    Sabre Fighter Plane Simulator 
    Copyright (c) 1997/98 Dan Hammer
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
/*******************************************************
 * flight.cpp                                          *
 *******************************************************/
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <limits.h>
#include <values.h>
#include "sim.h"
#include "simfile.h"
#include "defs.h"
#include "vmath.h"
#include "port_3d.h"
#include "rtkey.h"
#include "colorspc.h"
#include "fltlite.h"

extern float time_expired;

/* convert a double vector into port coords */
DVector FlightLight::to_port(DVector &v)
{
  R_3DPoint r_w,r_p;
  r_w = v.to_R_3DPoint();
  r_w += state.flight_port.look_from;
  state.flight_port.world2port(r_w,&r_p);
  return (DVector(r_p));
}

DVector FlightLight::to_world(DVector &v)
{
  R_3DPoint r_w,r_p;
  r_p = v.to_R_3DPoint();
  //	r_p += state.flight_port.look_from;
  state.flight_port.port2world(r_p,&r_w);
  r_w -= state.flight_port.look_from;
  return (DVector(r_w));
}

void FlightLight_Specs::read(std::istream &is)
{
  char c = ' ';
  READ_TOK('{',is,c);
  is >> weight >> thrust;
  is >> drag_factor >> lift_factor >> time_limit >> init_speed;
  is >> init_pitch_rate >> init_yaw_rate >> init_roll_rate >> c;
  READ_TOK('}',is,c);
  mass = weight / g;
}

void FlightLight_Specs::write(std::ostream &os)
{
  os << "{\n";
  os << weight << thrust;
  os << drag_factor << lift_factor << time_limit << init_speed;
  os << init_pitch_rate << init_yaw_rate << init_roll_rate;
  os << "}\n";
}

std::istream &operator >>(std::istream &is, FlightLight_Specs &fs)
{
  fs.read(is);
  return (is);
}

std::ostream &operator <<(std::ostream &os, FlightLight_Specs &fs)
{
  fs.write(os);
  return (os);
}

FlightLight::FlightLight(FlightLight_Specs *spcs)
  : specs(spcs)
{
  do_rotations = 0;
}

void FlightLight::set_specs(FlightLight_Specs *spcs)
{
  specs = spcs;
  //	state.pitch_rate = specs->init_pitch_rate;
  //	state.yaw_rate = specs->init_yaw_rate;
  //	state.roll_rate = specs->init_roll_rate;
}

void FlightLight::activate(Flight &launcher)
{
  state.flight_port = launcher.state.flight_port;
  //	flt.state.flight_port.delta_look_from(Vector(0.05,0.05,-0.04));
  DVector v = launcher.state.flight_port.calc_view_normal();
  Vector_Q vq1(specs->init_speed,v);
  state.velocity = launcher.state.velocity + vq1;
}

void FlightLight::activate(Port_3D &fport, FlightLight_Specs *flt_specs,
			   R_3DPoint &p1, Vector &v, Vector_Q *vq)
{
  Vector v1;
  set_specs(flt_specs);
  state.flight_port = fport;
  R_3DPoint p2,wp1,wp2;
  v.Normalize();
  p2 = p1 + (R_3DPoint) v;
  fport.port2world(p1,&wp1);
  fport.port2world(p2,&wp2);
  state.flight_port.set_view(wp1,wp2);
  v1 = state.flight_port.calc_view_normal();
  Vector_Q vq1(specs->init_speed,DVector(v1));
  if (vq)
    state.velocity = *vq + vq1;
  else
    state.velocity = vq1;
}


void FlightLight::activate(R_3DPoint &p, Vector &dir)
{
  Vector v1;
  dir.Normalize();
  R_3DPoint p1 = p + (R_3DPoint)dir;
  state.flight_port.set_view(p,p1);
  v1 = state.flight_port.calc_view_normal();
  Vector_Q vq1(specs->init_speed,DVector(v1));
  state.velocity = vq1;
}

void FlightLight::start()
{
  // start the timer
  calc_state();
  //	calc_forces();
}

void FlightLight::update()
{
  // Get elapsed time in microseconds
  l_time = time_frame;
  calc_state();
  apply_forces(l_time);
  //  if (do_rotations != 0)
    apply_rotations(l_time);
  //	calc_forces();
}

void FlightLight::pause()
{

}

extern REAL_TYPE getGroundLevel(R_3DPoint &);

extern int gggresult;
void FlightLight::calc_state()
{
  REAL_TYPE zg = getGroundLevel(state.flight_port.look_from);
  if (state.flight_port.look_from.z <= zg)
    {
      state.on_ground = 1;
      //      state.flight_port.look_from.z = zg;
    }
  else
    state.on_ground = 0;
}

/*
void FlightLight::calc_forces()
{
R_KEY_BEGIN(4)
	float vel_sq = state.velocity.magnitude * state.velocity.magnitude;
	if (vel_sq < 1.0)
		vel_sq = 1.0 + (1.0 - vel_sq);
	forces.thrust.magnitude = specs->thrust;
	forces.thrust.direction = DVector(0,0,1);
	forces.drag.magnitude = vel_sq * specs->drag_factor;
	forces.drag.direction = DVector(0,0,-1);
	forces.lift.magnitude = vel_sq * specs->lift_factor;
	forces.lift.direction = DVector(0,1,0);
	forces.g.magnitude = specs->weight;
	forces.g.direction = DVector(0,0,-1);
R_KEY_END
}
*/

/*
static int thrust_rec = 0;
static float time_fr  = 0.0;
*/

void FlightLight::apply_forces(float t)
{
  R_KEY_BEGIN(99)
    Vector_Q thrust,drag,lift,gee;
  Vector_Q sum_forces,acc;
  Vector_Q tmp1,tmp2;
  Vector delta;
  // Flag which is set when the 'normal' force is active
  int no_z = 0;
  float vel_sq = state.velocity.magnitude * state.velocity.magnitude;
  if (vel_sq < 1.0)
    vel_sq = C(1.0) + (C(1.0) - vel_sq);
  thrust.magnitude = specs->thrust;
  thrust.direction = DVector(0,0,1);
  drag.magnitude = vel_sq * specs->drag_factor;
  drag.direction = DVector(0,0,-1);
  lift.magnitude = vel_sq * specs->lift_factor;
  lift.direction = DVector(0,1,0);
  gee.magnitude = specs->weight;
  gee.direction = DVector(0,0,-1);


  // Add up all 'port coordinant' forces
  sum_forces.direction = to_vector(thrust)
    + to_vector(drag)
    + to_vector(lift);
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
  sum_forces.direction = to_world(sum_forces.direction);
  sum_forces = sum_forces + gee;
  // If on ground, apply normal force
  if (state.on_ground)
    {
      no_z = 1;
      if (sum_forces.direction.Z < 0)
	{
	  sum_forces.magnitude += sum_forces.magnitude *
	    sum_forces.direction.Z;
	  sum_forces.direction.Z = 0;
	  sum_forces.direction.Normalize();
	}
    }

  // Determine acceletation
  // a = F / m
  acc.magnitude = sum_forces.magnitude / specs->mass;
  acc.direction = sum_forces.direction;

  DVector delta_v;
  //	acc.direction.Normalize();
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
  if (state.velocity.magnitude >= 32000)
    state.velocity.magnitude = 32000;
  // Kill any negative
  if (state.velocity.magnitude < 0.0)
    state.velocity.magnitude = 0.0;
  if (no_z && state.velocity.direction.Z < 0)
    {
      state.velocity.magnitude -= (state.velocity.magnitude *
				   state.velocity.direction.Z);
      state.velocity.direction.Z = 0;
      /*
      if (state.flight_port.look_from.z < 0)
	state.flight_port.delta_look_from(Vector(0,0,
						 -state.flight_port.look_from.z));
						 */
    }
  R_KEY_END
    }


void FlightLight::apply_rotations(float t)
{
  //	t = 1.0;
  if (specs->init_pitch_rate != 0.0)
    state.flight_port > (specs->init_pitch_rate * t);
  if (specs->init_yaw_rate != 0.0)
    state.flight_port < (specs->init_yaw_rate * t);
  if (specs->init_roll_rate != 0.0)
  state.flight_port / (specs->init_roll_rate * t);
}
