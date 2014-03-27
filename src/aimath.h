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
#ifndef __aimath_h
#define __aimath_h


#ifdef __cplusplus
#include "vmath.h"
#include "port_3d.h"
#include "smath.h"

extern float world_scale;

inline sVector & Vector2sVector(const Vector &v, sVector &sv)
{
	sv.x = (sREAL) v.X;
	sv.y = (sREAL) v.Y;
	sv.z = (sREAL) v.Z;

	return sv;
}

inline Vector & sVector2Vector(const sVector &sv, Vector &v)
{
	v.X = (REAL_TYPE) sv.x;
	v.Y = (REAL_TYPE) sv.y;
	v.Z = (REAL_TYPE) sv.z;

	return v;
}

inline sPoint & R_3DPoint2sPoint(const R_3DPoint &r, sPoint &s)
{
	s.x = (sREAL) (r.x / world_scale);
	s.y = (sREAL) (r.y / world_scale);
	s.z = (sREAL) (r.z / world_scale);
	return s;
}

inline R_3DPoint & sPoint2R_3DPoint(const sPoint &s, R_3DPoint &r)
{
	r.x = ((REAL_TYPE) s.x * world_scale);
	r.y = ((REAL_TYPE) s.y * world_scale);
	r.z = ((REAL_TYPE) s.z * world_scale);
	return r;
}

inline sREAL CvFrSabreAngle(REAL_TYPE sbrAng)
{
	if ((sREAL) sbrAng > Pi)
		return (sREAL)sbrAng - _2Pi;
	else
		return (sREAL) sbrAng;
}

inline REAL_TYPE CvToSabreAngle(sREAL ang)
{
	return ((REAL_TYPE) ConvertAngle(ang));
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
