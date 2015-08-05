/*
    SABRE Fighter Plane Simulator 
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
*/
/*************************************************
 *           SABRE Fighter Plane Simulator       *
 * File   : port_3d.C                            *
 * Date   : March, 1997                          *
 *          December, 1998                       *
 * Author : Dan Hammer                           *
 *************************************************/
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include "vmath.h"
#include "rtkey.h"
#include "vga_13.h"
#include "port_3d.h"
#include "simfile.h"

typedef REAL_TYPE C;

const float pi =		3.1415927f;
const float one_sixth_pi =	0.5235987f;
const float one_fourth_pi =	0.7853891f;
const float one_third_pi =	1.0471976f;
const float half_pi =		1.5707963f;
const float two_pi =		6.2831853f;
const float three_fourth_pi =	4.7123890f;
const float _degree =		0.0174533f;
const float _minute =		2.908882e-04f;
const float _second =		4.848137e-06f;

Rect Port_3D::screen(0,0,SCREEN_WIDTH-1,SCREEN_HEIGHT-1);
REAL_TYPE Port_3D::cx = SCREEN_WIDTH / 2;
REAL_TYPE Port_3D::cy = SCREEN_HEIGHT / 2;
REAL_TYPE Port_3D::fovx = 300;
REAL_TYPE Port_3D::fovy = 280;
extern REAL_TYPE world_scale;

void Port_3D::read(std::istream &is)
{
  /* used to be fovx */
  float ignore;
 
  is >> slook_from >> look_from;
  look_from *= world_scale;
  is >> ignore >> roll;
  is  >> z_min >> z_max >> horizon;
  calc_angles();
  calc_look_at();
  calc_view_normal();
  sin_roll = static_cast<REAL_TYPE>(sin(roll));
  cos_roll = static_cast<REAL_TYPE>(cos(roll));
}

std::istream &operator >>(std::istream &is, Port_3D &port)
{
  port.read(is);
  return is;
}

void Port_3D::write(std::ostream &os)
{
  os << slook_from  << "\n";
  os << look_from << "\n";
  os << look_at << "\n";
}

std::ostream &operator <<(std::ostream &os, Port_3D &port)
{
  port.write(os);
  return os;
}

void Port_3D::initPort3D()
{
	sim_printf("Port_3D::initPort3D\n");
  screen = Rect(0,0,SCREEN_WIDTH-1,SCREEN_HEIGHT-1);
  cx = screen.topLeft.x + ((screen.botRight.x - screen.topLeft.x) / 2);
  cy = screen.topLeft.y + ((screen.botRight.y - screen.topLeft.y) / 2);
}

void Port_3D::setScreen(Rect &scr)
{
  screen = scr;
  cx = screen.topLeft.x + ((screen.botRight.x - screen.topLeft.x) / 2);
  cy = screen.topLeft.y + ((screen.botRight.y - screen.topLeft.y) / 2);
}

Port_3D::Port_3D()
  :look_from(0,0,0),
   slook_from(0,0,20),
   roll(0)
{
  calc_rho();
  calc_angles();
  calc_look_at();
  calc_view_normal();
  sin_roll = static_cast<REAL_TYPE>(sin(roll));
  cos_roll = static_cast<REAL_TYPE>(cos(roll));
  z_min = 1.0;
  z_max = 160.0;
  r = 0.0;
  horizon = 100;
}


void Port_3D::copy(Port_3D &p)
{
  look_from = p.look_from;
  look_at = p.look_at;
  slook_from = p.slook_from;
  view_normal = p.view_normal;
  z_min = p.z_min;
  z_max = p.z_max;
  horizon = p.horizon;
  cos_theta = p.cos_theta;
  sin_theta = p.sin_theta;
  cos_phi = p.cos_phi;
  sin_phi = p.sin_phi;
  r = p.r;
  roll = p.roll;
  sin_roll = p.sin_roll;
  cos_roll = p.cos_roll;
  over_flow = p.over_flow;
}

Port_3D::Port_3D(Port_3D &p)
{
  copy(p);
}

Port_3D &Port_3D::operator =(Port_3D &p)
{
  copy(p);
  return (*this);
}

Port_3D::Port_3D(S_3DPoint &sfrom, R_3DPoint &at, REAL_TYPE , REAL_TYPE p)
{
  roll = p;
  slook_from = sfrom;
  look_at = at;
  z_min = 1.0;
  z_max = 160.0;
  calc_angles();
  calc_look_at();
  calc_view_normal();
  sin_roll = static_cast<REAL_TYPE>(sin(roll));
  cos_roll = static_cast<REAL_TYPE>(cos(roll));
  horizon = 100;
}

// Project a point (in port coordinants, i.e. xE,yE,zE) onto the screen
// Note that if we are using FixedPoint, a port.z value < 1.0 will
// very likely result in a divide overflow.
void Port_3D::port2screen(R_3DPoint &port,
			  int *screen_x, int *screen_y)
{
	REAL_TYPE f_d1,f_d2;
	over_flow = 0;
	if (port.z <= 0)
	{
		over_flow = 1;
		return;
	}
	R_KEY_BEGIN(2)
	f_d1 = (fovx * port.x) / port.z;
	f_d2 = (fovy * port.y * aspect_ratio) / port.z;
	if (fabs(f_d2) > 32000.0 || fabs(f_d1) > 32000.0 )
	{
		over_flow = 1;
		R_KEY_END
		return;
	}
	else
	{
		*screen_x = static_cast<int>(f_d1 + cx);
		*screen_y = static_cast<int>((f_d2 * -1.0) + cy);
	}
	R_KEY_END
}


void Port_3D::port2screen(const TR_3DPoint &tr, TRF_2DPoint *rp)
{
  REAL_TYPE z_inv;
  REAL_TYPE z;

  z = tr.r.z;
  if (z < 1.0)
	  z = 1.0;
	z_inv = static_cast<REAL_TYPE>(1.0 / z);
  rp->x = (fovx * tr.r.x * z_inv) + cx ;
  rp->y = -(fovy * tr.r.y * aspect_ratio * z_inv) + cy;
  rp->z = REAL_TYPE((z - 1.0) / z);
  rp->u = REAL_TYPE(tr.e.u);
  rp->v = REAL_TYPE(tr.e.v);
  rp->w = z;
}

/*************************************************************
 * Unproject a point in screen coords to port coords. Z value
 * is given
 *************************************************************/
void Port_3D::screen2port(float screen_x, float screen_y, R_3DPoint *port, int flg)
{
  if (flg)
    {
	  // De-normalize screen coords
      screen_x -= cx;
      screen_y = -screen_y - cy;
    }
  port->x = port->z * (screen_x / fovx);
  port->y = port->z * (screen_y / (fovy * aspect_ratio));
}

/************************************************************
 * Set view: given two points, derive spherical coords.
 * Note that roll is not determined here
 ************************************************************/
void Port_3D::set_view(const R_3DPoint &lf, const R_3DPoint &la)
{
float dx,dy,dz;
float dtheta,dphi,dr;
int which_quad;

	dx = lf.x - la.x;
	dy = lf.y - la.y;
	dz = lf.z - la.z;
	if (dx >= 0.0 && dy >= 0.0)
		which_quad = 1;
	else if (dx > 0.0 && dy < 0.0)
		which_quad = 2;
	else if (dx < 0.0 && dy < 0.0)
		which_quad = 3;
	else
		which_quad = 4;
	dr = C(sqrt(dx*dx + dy*dy));
	slook_from.rho = 1.0;
	if (dr == 0.0)
		dtheta = slook_from.theta;
	else
	{
		dtheta = C(acos(dx/dr));
		if (dy <= 0.0)
			dtheta = two_pi - dtheta;
	}
	if (dz == 0.0)
		dphi = half_pi;
	else if (dr == 0.0)
		dphi = 0.0;
	else
	{
		dphi = C(atan(dr/dz));
		if (dphi < 0.0)
		{
			if (which_quad < 3)
				dphi = pi + dphi;
			else
				dphi = pi -  dphi;
		}
		else if (which_quad == 3)
			dphi = two_pi - dphi;
	}

	if (dphi > pi)
		dphi = two_pi - dphi;

	slook_from.phi = dphi;
	slook_from.theta = dtheta;
	look_from = lf;

	calc_angles();
	calc_look_at();
}

/**************************************************************
 * Rotate the port using an admittedly wierd and
 * probably inefficient method somewhat related
 * to quarternions in that it uses a forward and
 * up normals. 
 **************************************************************/
void Port_3D::rotate(REAL_TYPE pitch, REAL_TYPE rol, REAL_TYPE yaw)
{
  /* forward normal */
  R_3DPoint fwdNorm(0,0,1);
  /* upward normal */
  R_3DPoint upNorm(0,1,0);
  REAL_TYPE xTmp,yTmp,zTmp;
  R_3DPoint fwdPort;
  R_3DPoint upPort;
  R_3DPoint fwdWorld;
  R_3DPoint upWorld;
  R_3DPoint rollPoint;
  float		 rr;
  REAL_TYPE sinPitch,cosPitch,sinYaw,cosYaw;

  sinPitch = C(sin(pitch));
  cosPitch = C(cos(pitch));
  sinYaw = C(sin(yaw));
  cosYaw = C(cos(yaw));
  // Transform forward normal
  // Rotate about x-axis for pitch
  xTmp = fwdNorm.x;
  yTmp = fwdNorm.y * cosPitch - fwdNorm.z * sinPitch;
  zTmp = fwdNorm.y * sinPitch + fwdNorm.z * cosPitch;
  // Rotate about y-axis for yaw
  fwdPort.y = yTmp;
  fwdPort.x = xTmp * cosYaw + zTmp * sinYaw;
  fwdPort.z = -xTmp * sinYaw + zTmp * cosYaw;
  port2world(fwdPort,&fwdWorld);
  /* Transform upward normal */
  xTmp = upNorm.x;
  yTmp = upNorm.y * cosPitch - upNorm.z * sinPitch;
  zTmp = upNorm.y * sinPitch + upNorm.z * cosPitch;
  upPort.y = yTmp;
  upPort.x = xTmp * cosYaw + zTmp * sinYaw;
  upPort.z = -xTmp * sinYaw + zTmp * cosYaw;
  port2world(upPort,&upWorld);
  // set view to new forward normal
  set_view(look_from,fwdWorld);
  // set roll to 0  
  this->roll = 0.0;
  cos_roll = 1.0;
  sin_roll = 0.0;
  // Correct roll by getting
  // upward normal back from world coords
  world2port(upWorld,&rollPoint);
  // Get roll needed to match world
  rr = C(-atan(rollPoint.x / (rollPoint.y+eps)));
  if (rollPoint.y < 0.0)
    rr -= SABRE_PI;
  if (rr < 0.0)
    rr = SABRE_2PI + rr;

  this->roll = limit_angle(rr + rol);
  sin_roll = C(sin(this->roll));
  cos_roll = C(cos(this->roll));
}

/************************************************************
 * Rotate to given point, maintaining roll
 ************************************************************/
void Port_3D::rotate_to_point(const R_3DPoint &lf)
{
REAL_TYPE pitch;
REAL_TYPE yaw;
R_3DPoint p0;

	world2port(lf,&p0);
	yaw = C(atan(p0.x / (p0.z + eps)));
	if (p0.z < 0.0)
		yaw += pi;
	rotate(0.0,0.0,yaw);
	world2port(lf,&p0);
	pitch = C(-atan(p0.y / fabs(p0.z + eps)));
	rotate(pitch,0.0,0.0);
}

// Calculate 3 points in world coordinants representing the left, center
// and right expanse of the horizon
static REAL_TYPE vh_limit = C(0.009);
void Port_3D::get_view_horizon(REAL_TYPE dst, R_3DPoint *wh_left,
			       R_3DPoint *wh_center,
			       R_3DPoint *wh_right,
			       R_3DPoint *ref)
{
REAL_TYPE sv_roll;
REAL_TYPE mag;
R_3DPoint p,d;

  // Get the view normal on the xy plane
  d = look_at - look_from;
  mag = C(sqrt((d.x*d.x)+(d.y*d.y)));
  if (mag < vh_limit)
    mag = vh_limit;
  d.x = d.x / mag;
  d.y = d.y / mag;

  // Place the horizon @ requested distance
  if (ref)
    {
      p.x = (ref->x + (d.x * dst));
      p.y = (ref->y + (d.y * dst));
    }
  else
    {
      p.x = (look_from.x + (d.x * dst));
      p.y = (look_from.y + (d.y * dst));
    }
  p.z = 0;

  *wh_center = p;
  // Now comes the serious stuff!
  // We need to get two world points which,
  // when transformed, will be on the edge of
  // the screen
  // Create a new view with a 0 roll
  sv_roll = roll;
  set_roll(0);
  R_3DPoint p1;
  // Get a local coords of the calculated point on the horizon
  world2port(p,&p1);
  // Set some limit on how small z can be
  if (fabs(p1.z) < 0.0001)
    p1.z = 0;

  // 'unproject' an screen-x value of -30 & 349 to get the
  // world coords of points on the horizon which will
  // meet/overlap the edge of the screen
  REAL_TYPE c = static_cast<REAL_TYPE>(cx);
  p1.x = ((-(p1.z * (SCREEN_WIDTH + SCREEN_HEIGHT))) - (c * p1.z)) / fovx;
  port2world(p1,wh_left);
  p1.x = (((SCREEN_WIDTH + SCREEN_HEIGHT) *  p1.z) - (c * p1.z)) / fovx;
  port2world(p1,wh_right);
  set_roll(sv_roll);
}


/* 
   Calculate the roll which would be needed to align
   given point, given in world coordinants, with the 
   given port
   */
float calcRollForPoint(Port_3D &port, R_3DPoint &w)
{
float result = 0.0;
Port_3D vport;
R_3DPoint p;
float sc_x,sc_y;

  vport = port;
  vport.set_roll(0.0);
  if (world2portN(w,&p,&sc_x,&sc_y,vport))
    {
      result = C(-atan(p.x / (p.y + eps)));
      if (p.y < 0.0)
			result -= SABRE_PI;
      if (result < 0.0)
	result = SABRE_2PI + result;
    }
  return (result);
}

float calcRollForPoint2(Port_3D &port, R_3DPoint &w, R_3DPoint *p, float *sc_x, float *sc_y);
float calcRollForPoint2(Port_3D &port, R_3DPoint &w, R_3DPoint *p, float *sc_x, float *sc_y)
{
float result = 0.0;
Port_3D vport;

  vport = port;
  vport.set_roll(0.0);
  if (world2portN(w,p,sc_x,sc_y,vport))
    {
      if (p->y < 0.0)
	{
	  p->y *= -1.0;
	  //	  p.x *= -1.0;
	}
      result = C(-atan(p->x / (p->y + eps)));
      if (p->y < 0.0)
	result -= SABRE_PI;
      if (result < 0.0)
	result = SABRE_2PI + result;
    }
  return (result);
}
