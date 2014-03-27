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
*/
/*************************************************
 *           Sabre Fighter Plane Simulator       *
 * File   : port_3d.h                            *
 * Date   : March, 1997                          *
 *          December,1998                        *
 * Author : Dan Hammer                           *
 * The Port_3D class encapsulates a 3D view, and *
 * provides the transformation functions for     *
 * going between the world, the viewpoint, and   *
 * the screen.                                   *
 *************************************************/
#ifndef __port_3d_h
#define __port_3d_h

#include "vga_13.h"
#include "grafix.h"
#include "clip.h"
#include "rtkey.h"

extern const float pi;
extern const float one_sixth_pi;
extern const float one_fourth_pi;
extern const float one_third_pi;
extern const float half_pi;
extern const float two_pi;
extern const float three_fourth_pi;
extern const float _degree;
extern const float _minute;
extern const float _second;

#define _PI  pi
#define _PI2 half_pi
#define _PI4 one_fourth_pi
#define _2PI two_pi
#define _PI34 three_fourth_pi
#define _DEGREE _degree
#define _MINUTE _minute
#define _SECOND _second

inline REAL_TYPE limit_angle(REAL_TYPE angle)
{
  if (fabs(angle) >= _2PI)
    {
      REAL_TYPE m = fmod(angle,_2PI);
      if (angle < 0)
	m = -m;
      angle = m;
    }
  if (angle == _2PI)
    angle = 0;
  if (angle < 0)
    angle = _2PI + angle;
  return angle;
}

class Port_3D
{
public:
	R_3DPoint look_from;
	R_3DPoint look_at;
	S_3DPoint slook_from;
	Vector view_normal;
	REAL_TYPE z_min,z_max,horizon;
	REAL_TYPE cos_theta,sin_theta,cos_phi,sin_phi,r;
	REAL_TYPE roll,sin_roll,cos_roll;
	int over_flow;

	static Rect screen;
	static REAL_TYPE cx,cy;
	static REAL_TYPE fovx,fovy;
	static void initPort3D();
	static void setScreen(Rect &r);

	Port_3D();
	Port_3D(Port_3D &);
	Port_3D &operator =(Port_3D &);
	void copy(Port_3D &);
	Port_3D(R_3DPoint &from, R_3DPoint &at, REAL_TYPE d, REAL_TYPE p = 0);
	Port_3D(S_3DPoint &sfrom, R_3DPoint &at, REAL_TYPE d, REAL_TYPE p = 0);
	void port2screen(R_3DPoint &port, int *screen_x, int *screen_y);
	inline void calc_rho();
	inline void calc_angles();
	inline void calc_look_at();
	inline void calc_look_from();
	void set_view(const R_3DPoint &lf, const R_3DPoint &la);
	void align_port(R_3DPoint &rp);
	inline void port2screen(const R_3DPoint &port, float *screen_x, float *screen_y);
	inline void world2port(const R_3DPoint &wld, const TxtPoint &tt, TR_3DPoint *tr);
	inline void port2screen(const R_3DPoint &tr, R_2DPoint *rp);
	inline void port2screen(const TR_3DPoint &tr, TR_2DPoint *rp, int = 1);
	void port2screen(const TR_3DPoint &tr, TRF_2DPoint *rp);

	void screen2port(float screen_x, float screen_y, R_3DPoint *port, int = 1);
	inline void world2port(const R_3DPoint &world, R_3DPoint *port);
	inline void port2world(const R_3DPoint &port, R_3DPoint *world);
	inline void transform(const R_3DPoint &point, int *screen_x, int *screen_y);
	void get_view_horizon(REAL_TYPE d, R_3DPoint *wh_left, 
								 R_3DPoint *wh_center, R_3DPoint *wh_right,
								 R_3DPoint * = NULL);
	void rotate(REAL_TYPE pitchInc, REAL_TYPE rollInc, REAL_TYPE yawInc);
	void rotate_to_point(const R_3DPoint &lf);
	inline void operator <(REAL_TYPE);
	inline void operator <<(REAL_TYPE);
	inline void operator >(REAL_TYPE);
	inline void operator >>(REAL_TYPE);
	inline void operator ^(REAL_TYPE);
	inline void operator /(REAL_TYPE);
	inline void operator +(REAL_TYPE);

	inline void set_theta(REAL_TYPE th);
	inline void set_phi(REAL_TYPE ph);
	inline void set_roll(REAL_TYPE rll);
	inline void set_rho(REAL_TYPE rh);
	inline void delta_look_from(const Vector &);
	inline void set_look_from(const R_3DPoint &);
	inline const Vector &calc_view_normal();
	friend class C_Shape;
	friend istream &operator >>(istream &is, Port_3D &port);
	void read(istream &is);
	friend ostream &operator <<(ostream &os, Port_3D &port);
	void write(ostream &os);


	int IsVisible(int x, int y)
	{
		return (is_visible(screen,x,y));
	}

	REAL_TYPE get_phi(void)
	{
		return slook_from.phi;
	}
	REAL_TYPE get_theta(void)
	{
		return slook_from.theta;
	}
	REAL_TYPE get_roll(void)
	{
		return roll;
	}
	const R_3DPoint &get_look_from(void)
	{
		return look_from;
	}
	const R_3DPoint &get_look_at(void)
	{
		return look_at;
	}
	const Vector &get_view_normal(void)
	{ 
		return view_normal; 
	}
	void set_z_min(REAL_TYPE zm)
	{ 
		z_min = zm; 
	}
	void set_z_max(REAL_TYPE zm)
	{ 
		z_max = zm; 
	}
	REAL_TYPE get_z_min(void)
	{ 
		return z_min; 
	}
	REAL_TYPE get_z_max(void)
	{ 
		return z_max; 
	}

};

inline int world2portN(const R_3DPoint &w, R_3DPoint *p,
			float *sc_x, float *sc_y, Port_3D &port)
{
  port.world2port(w,p);
  if (p->z > 0.0)
    {
      port.port2screen(*p,sc_x,sc_y);
      *sc_x -= port.cx;
      *sc_y -= port.cy;
      *sc_y = -(*sc_y);
      return (1);
    }
  else
    return(0);
}


inline void Port_3D::calc_rho()
{
  REAL_TYPE dX,dY,dZ;
  dX = look_from.x - look_at.x;
  dY = look_from.y - look_at.y;
  dZ = look_from.z - look_at.z;
  slook_from.rho = sqrt((dX*dX) + (dY*dY) + (dZ*dZ));
}

// Derive rectangular coordinants from spherical
inline void Port_3D::calc_look_at()
{
  look_at.x = look_from.x - ((slook_from.rho * sin_phi) * cos_theta);
  look_at.y = look_from.y - ((slook_from.rho * sin_phi) * sin_theta);
  look_at.z = look_from.z - (slook_from.rho * cos_phi);
  calc_view_normal();
}

inline void Port_3D::calc_look_from()
{
  look_from.x = ((slook_from.rho * sin_phi) * cos_theta) + look_at.x;
  look_from.y = ((slook_from.rho * sin_phi) * sin_theta) + look_at.y;
  look_from.z = (slook_from.rho * cos_phi) + look_at.z;
  calc_view_normal();
}

inline void Port_3D::port2screen(const R_3DPoint &port, float *screen_x, float *screen_y)
{
  over_flow = 0;
  if (port.z <= 0)
    {
      over_flow = 1;
      return;
    }
  R_KEY_BEGIN(1)
    *screen_x = (fovx * port.x / port.z) + cx;
  *screen_y = -(fovy * port.y * aspect_ratio / port.z) + cy;
  R_KEY_END
    }

// Transform a point in world coordinants to port coordinants
inline void Port_3D::world2port(const R_3DPoint &world, R_3DPoint *port)
{
  REAL_TYPE x,y,z;
  REAL_TYPE xtemp1,ytemp1,ztemp1,xtemp2,ytemp2,ztemp2;

  // Translate to port origins
  x = world.x - look_at.x;
  y = world.y - look_at.y;
  z = world.z - look_at.z;

  // Rotate whole system 90 degrees about z axis 
  // to get a 'left-handed' system
  // Since cos(90) = 0, you're basically swapping x & y coords

  xtemp2 = x;
  x = y;
  y = -xtemp2;


  // Rotate theta rads about z axis
  xtemp1 = (x * cos_theta) + (y * sin_theta);
  ytemp1 = ((-x) * sin_theta) + (y * cos_theta);
  ztemp1 = z;

  // Rotate phi rads about xaxis
  xtemp2 = xtemp1;
  ytemp2 = (ytemp1 * cos_phi)  + (ztemp1 * sin_phi);
  ztemp2 = ((-ytemp1) * sin_phi) + (ztemp1 * cos_phi);

  // Translate origin up along rho
  port->z =  slook_from.rho - ztemp2;
  port->x = (xtemp2 * cos_roll) + (ytemp2 * sin_roll);
  port->y = ((-xtemp2) * sin_roll) + (ytemp2 * cos_roll);
}

// Transform a point in port coordinants to
// world coordinants. Apply clockwise rotation
// using the fact that cos(-theta) = cos(theta), and
// sin(-theta) = -sin(theta)
inline void Port_3D::port2world(const R_3DPoint &port, R_3DPoint *world)
{
  REAL_TYPE x,y,z;
  REAL_TYPE xtemp1,ytemp1,ztemp1,xtemp2,ytemp2,ztemp2;

  xtemp2 = (port.x * cos_roll) - (port.y * sin_roll);
  ytemp2 = (port.x * sin_roll) + (port.y * cos_roll);
  // Translate origin down rho
  ztemp2 = slook_from.rho - port.z;

  // Undo rotation about XAxis
  xtemp1 = xtemp2;
  ytemp1 = (ytemp2 * cos_phi)  - (ztemp2 * sin_phi);
  ztemp1 = (ytemp2 * sin_phi) + (ztemp2 * cos_phi);
  // Undo rotation about ZAxis
  x = (xtemp1 * cos_theta) - (ytemp1 * sin_theta);
  y = (xtemp1 * sin_theta) + (ytemp1 * cos_theta);
  z = ztemp1;

  // Convert to right-handed system
  xtemp2 = y;
  y = x;
  x = -xtemp2;

  // Translate back to world origin
  world->x = x + look_at.x;
  world->y = y + look_at.y;
  world->z = z + look_at.z;

}

// An all-in-1 call to transform/project a point to the screen
inline void Port_3D::transform(const R_3DPoint &world, int *screen_x,
			       int *screen_y)
{
  R_3DPoint port;
  world2port(world,&port);
  port2screen(port,screen_x,screen_y);
}

// Texture-mapping & zbuffering variants
inline void Port_3D::world2port(const R_3DPoint &wld, const TxtPoint &tt,
			 TR_3DPoint *tr)
{
  R_3DPoint prt;
  world2port(wld,&prt);
  tr->r = prt;
  tr->e = tt;
}

inline void Port_3D::port2screen(const R_3DPoint &tr, R_2DPoint *rp)
{
  over_flow = 0;
  REAL_TYPE z_inv;
  if (tr.z >= 1.0)
    z_inv = 1.0 / tr.z;
  else
    z_inv = 1.0;
  rp->x = R2D_TYPE((fovx * tr.x * z_inv) + cx );
  rp->y = R2D_TYPE(-(fovy * tr.y * aspect_ratio * z_inv) + cy );
  rp->z = R2D_TYPE(z_inv * zfact);
}

inline void Port_3D::port2screen(const TR_3DPoint &tr, TR_2DPoint *rp, int persp)
{
  over_flow = 0;
  REAL_TYPE z_inv;
  if (tr.r.z >= 1.0)
    z_inv = 1.0 / tr.r.z;
  else
    z_inv = 1.0;
  rp->x = R2D_TYPE((fovx * tr.r.x * z_inv) + cx );
  rp->y = R2D_TYPE(-(fovy * tr.r.y * aspect_ratio * z_inv) + cy );
  rp->z = R2D_TYPE(z_inv * zfact);
  if (persp)
    {
      rp->u = R2D_TYPE(REAL_TYPE(tr.e.u) * z_inv * zfact);
      rp->v = R2D_TYPE(REAL_TYPE(tr.e.v) * z_inv * zfact);
    }
  else
    {
      rp->u = R2D_TYPE(tr.e.u) << Z_PREC;
      rp->v = R2D_TYPE(tr.e.v) << Z_PREC;
    }
}

inline void Port_3D::calc_angles()
{
  sin_theta = sin(slook_from.theta);
  sin_phi = sin(slook_from.phi);
  cos_theta = cos(slook_from.theta);
  cos_phi = cos(slook_from.phi);
}

inline const Vector &Port_3D::calc_view_normal()
{
  view_normal = look_at - look_from;
  view_normal.Normalize();
  return view_normal;
}

inline void Port_3D::operator +(REAL_TYPE delta_v)
{
  calc_view_normal();
  Vector dv = view_normal;
  dv *= delta_v;
  look_from += dv;
  look_at += dv;
}

inline void Port_3D::delta_look_from(const Vector &dv)
{
  look_from += dv;
  look_at += dv;
}

inline void Port_3D::set_theta(REAL_TYPE th)
{
  slook_from.theta = th; sin_theta = sin(th); cos_theta = cos(th);
  calc_look_at();
}

inline void Port_3D::set_phi(REAL_TYPE ph)
{
  slook_from.phi = ph; sin_phi = sin(ph); cos_phi = cos(ph);
  calc_look_at();
}

inline void Port_3D::set_roll(REAL_TYPE rll)
{
  roll = limit_angle(rll); sin_roll = sin(rll); cos_roll=cos(rll);
}

inline void Port_3D::set_rho(REAL_TYPE rh)
{ 
	slook_from.rho = rh; 
}

inline void Port_3D::operator <(REAL_TYPE delta_theta)
{
  // 0 <= theta <= 2pi
  slook_from.theta = limit_angle(slook_from.theta + delta_theta);
  sin_theta = sin(slook_from.theta);
  cos_theta = cos(slook_from.theta);
  calc_look_at();
}

inline void Port_3D::operator <<(REAL_TYPE delta_theta)
{
  // 0 <= theta <= 2pi
  slook_from.theta = limit_angle(slook_from.theta + delta_theta);
  sin_theta = sin(slook_from.theta);
  cos_theta = cos(slook_from.theta);
  calc_look_from();
}

inline void Port_3D::operator >(REAL_TYPE delta_phi)
{
  slook_from.phi = limit_angle(slook_from.phi + delta_phi);
  sin_phi = sin(slook_from.phi);
  cos_phi = cos(slook_from.phi);
  calc_look_at();
}

inline void Port_3D::operator >>(REAL_TYPE delta_phi)
{
  slook_from.phi = limit_angle(slook_from.phi + delta_phi);
  sin_phi = sin(slook_from.phi);
  cos_phi = cos(slook_from.phi);
  calc_look_from();
}

inline void Port_3D::operator ^(REAL_TYPE delta_rho)
{
  slook_from.rho += delta_rho;
  calc_angles();
  calc_look_at();
}

inline void Port_3D::operator /(REAL_TYPE delta_roll)
{
  roll = limit_angle(roll + delta_roll);
  sin_roll = sin(roll);
  cos_roll = cos(roll);
}

inline void Port_3D::set_look_from(const R_3DPoint &lf)
{
	look_from = lf;	
	calc_look_at();
}

float calcRollForPoint(Port_3D &port, R_3DPoint &w);

#endif


