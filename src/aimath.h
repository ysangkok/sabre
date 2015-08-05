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
 * File   : aimath.h                             *
 * Date   : April, 1998                          *
 * Author : Dan Hammer                           *
 * Convert from and to "ai" structures           *
 *************************************************/
#ifndef AIMATH_H
#define AIMATH_H


#ifdef __cplusplus
#include "vmath.h"
#include "port_3d.h"
#include "smath.h"

extern float world_scale;

inline sVector & Vector2sVector(const Vector &v, sVector &sv)
{
	sv.x = static_cast<sREAL>(v.X);
	sv.y = static_cast<sREAL>(v.Y);
	sv.z = static_cast<sREAL>(v.Z);

	return sv;
}

inline Vector & sVector2Vector(const sVector &sv, Vector &v)
{
	v.X = static_cast<REAL_TYPE>(sv.x);
	v.Y = static_cast<REAL_TYPE>(sv.y);
	v.Z = static_cast<REAL_TYPE>(sv.z);

	return v;
}

inline sPoint & R_3DPoint2sPoint(const R_3DPoint &r, sPoint &s)
{
	s.x = static_cast<sREAL>(r.x / world_scale);
	s.y = static_cast<sREAL>(r.y / world_scale);
	s.z = static_cast<sREAL>(r.z / world_scale);
	return s;
}

inline R_3DPoint & sPoint2R_3DPoint(const sPoint &s, R_3DPoint &r)
{
	r.x = static_cast<REAL_TYPE>(s.x) * world_scale;
	r.y = static_cast<REAL_TYPE>(s.y) * world_scale;
	r.z = static_cast<REAL_TYPE>(s.z) * world_scale;
	return r;
}

inline sREAL CvFrSabreAngle(REAL_TYPE sbrAng)
{
	if (static_cast<sREAL>(sbrAng) > Pi)
		return static_cast<sREAL>(sbrAng) - _2Pi;
	else
		return static_cast<sREAL>(sbrAng);
}

inline REAL_TYPE CvToSabreAngle(sREAL ang)
{
	return static_cast<REAL_TYPE>(ConvertAngle(ang));
}

inline void Port2sAttitude(const Port_3D &port, sAttitude &att)
{
sPoint  lookFrom;
sPoint  lookAt;
sVector dirNormal;

	R_3DPoint2sPoint(port.look_from, lookFrom);
	R_3DPoint2sPoint(port.look_at,lookAt);
	dirNormal = lookAt - lookFrom;
	dirNormal.Normalize();
	sVector2sAttitude(dirNormal,att);
	att.roll = CvFrSabreAngle(port.roll);
}

#endif

#endif
