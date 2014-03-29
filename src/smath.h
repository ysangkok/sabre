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
 * File   : smath.h                              *
 * Date   : April, 1998                          *
 * Author : Dan Hammer                           *
 * Various and sundry math classes and functions *
 * for new AI                                    *
 *************************************************/
#ifndef __smath_h
#define __smath_h

#include <math.h>

#define sREAL float
#define C(x) ((sREAL) (x))

#ifdef WIN32
#pragma warning ( 4 : 4244 )
#pragma warning ( 4 : 4305 )
#endif

// define some convenient radian
// measures
#define Pi           C(3.1415927)
#define Pi_2         C(1.5707963)
#define Pi_4         C(0.7853981)
#define _2Pi         C(6.2831853)
#define _32Pi        C(4.712389 )
#define _degree      C(0.0174532)
#define _half_degree C(8.7266E-3)
#define eps          C(1E-5)

// define macros for some common
// transendental functions in case
// we need to optimize via table
// lookup or other means
#define sATAN(x)  C(atan((x)))
#define sACOS(x)  C(acos((x)))
#define sCOS(x)  C(cos((x)))
#define sSIN(x)  C(sin((x)))
#define sTAN(x)  C(tan((x)))
#define sSQRT(x) C(sqrt((x)))
// conversion macros
#define sMPH2FPS(x) C((x) * 1.46666)
#define sFPS2MPH(x) C((x) * 0.68185)
#define sFPS2KTS(x) C((x) * 0.6)
#define sKTS2FPS(x) C((x) * 1.66666)

// for dealing with C programs, 
// define some structs
typedef struct s_vector_str
{
  sREAL x;
  sREAL y;
  sREAL z;
} s_vector;

typedef struct s_attitude_str
{
  sREAL pitch;
  sREAL roll;
  sREAL yaw;
} s_attitude;

#ifdef __cplusplus

// Basic vector class
class sVector 
{
public:
  sREAL x;
  sREAL y;
  sREAL z;

  sVector()
    {
      x = y = z = 0.0;
    }

  sVector(sREAL i, sREAL j, sREAL k)
    {
      this->x = i;
      this->y = j;
      this->z = k;
    }

  sVector &operator +=(const sVector &v)
    {
      x += v.x;
      y += v.y;
      z += v.z;
      return (*this);
    }

  friend const sVector operator -(const sVector &v0, const sVector &v1)
    {
      return (sVector(v0.x-v1.x,v0.y-v1.y,v0.z-v1.z));
    }

  friend const sVector operator +(const sVector &v0, const sVector &v1)
    {
      return (sVector(v0.x+v1.x,v0.y+v1.y,v0.z+v1.z));
    }

  sVector &operator *=( sREAL scaler )
    {
      x *= scaler;
      y *= scaler;
      z *= scaler;
      return (*this);
    }

  sREAL Dot(const sVector &v)
    {
      return (x * v.x + y * v.y + z * v.z);
    }

  sVector Cross(const sVector &v)
    {
      return (sVector(
		      y * v.z - z * v.y,
		      z * v.x - x * v.z,
		      x * v.y - y * v.x
		      ));
    }

  sREAL MagnitudeSquared()
    {
      return ((x * x) + (y * y) + (z * z));
    }

  sREAL Magnitude()
    {
      return ((sREAL) sqrt(MagnitudeSquared()));
    }

  void Normalize()
    {
      sREAL mag = Magnitude();
      if (mag > 0)
	{
	  x /= mag;
	  y /= mag;
	  z /= mag;
	}
    }

  friend sREAL dot(const sVector &v0, const sVector &v1)
    { 
      return (v0.x * v1.x + v0.y * v1.y + v0.z * v1.z); 
    }

  friend const sVector cross(const sVector &v0, const sVector &v1)
    {
      return (sVector(
		      v0.y * v1.z - v0.z * v1.y,
		      v0.z * v1.x - v0.x * v1.z,
		      v0.x * v1.y - v0.y * v1.x
		      ));
    }

};

// Subclass a Point class from the Vector class
// and some operators so we can use it
// interchangeably with the Vector class
class sPoint : public sVector
{
public:
  sPoint(sREAL x, sREAL y, sREAL z)
    :sVector(x,y,z)
    {}
	
  sPoint()
    :sVector()
    {}

  sPoint &operator =(const sVector &v)
    {
      x = v.x;
      y = v.y;
      z = v.z;
      return (*this);
    }
};

// An attitude class 
class sAttitude
{
public:
  sREAL  pitch;
  sREAL  roll;
  sREAL  yaw;
	
  sAttitude()
    {
      pitch = roll = yaw = 0.0;
    }

  sAttitude(sREAL pitc, sREAL rol, sREAL ya)
    {
      this->pitch = pitc;
      this->roll = rol;
      this->yaw = ya;
    }

  sAttitude(const sAttitude &sa)
    {
      pitch = sa.pitch;
      roll = sa.roll;
      yaw = sa.yaw;
    }

  const sAttitude & operator =(const sAttitude &sa)
    {
      pitch = sa.pitch;
      roll = sa.roll;
      yaw = sa.yaw;
      return (*this);
    }
};

// is value between min & max, inclusive?
inline int INRANGE(sREAL value, sREAL min, sREAL max)
{
  return ((value >= min) && (value <= max));
}

// pick min & max of two values
inline void sMinMax(sREAL val0, sREAL val1, sREAL &min, sREAL &max)
{
  if (val0 >= val1)
    {
      max = val0;
      min = val1;
    }
  else
    {
      max = val1;
      min = val0;
    }
}

sREAL sDistanceSquared(const sPoint &p0, const sPoint &p1);
sREAL sDistance(const sPoint &p0, const sPoint &p1);

// Convert a vector to an attitude
void sVector2sAttitude(const sVector &vect, sAttitude &att);

/*
 * Matrix stuff
 */
void sMatrix3Build(const sAttitude &att, sVector *mat3);
void sMatrix3Mult(sVector *mat0, sVector *mat1, sVector *mat2);
void sMatrix3Transpose(sVector *mat0, sVector *mat1);
void sMatrix3X(sREAL angle, sVector *mat3); 
void sMatrix3Y(sREAL angle, sVector *mat3);
void sMatrix3Z(sREAL angle, sVector *mat3);
void sMatrix3Rotate(sVector *mat, const sVector &v0, sVector &v1);
/*
 * quarternion
 */
void sGetRPH(const sVector &roll, const sVector &pitch, sAttitude  &att);

/*
 * Angle stuff
 */
sREAL sIncrementAngle(sREAL angle, sREAL inc);
sREAL sGetMedianAngle(sREAL angle0, sREAL angle1);
sREAL sGetCircularDistance(sREAL from, sREAL to);
int sIsBetween(sREAL angle0, sREAL angle1, sREAL angle2);

inline sREAL ConvertAngle(sREAL angle)
{
  if (angle >= 0.0)
    return (angle);
  else
    return (sREAL) (_2Pi + angle);
}

/*
 * random stuff
 */
// Get a random per, 0 - 1.0
sREAL sRandPer();
// Get a random int between min and max, inclusive
int sRand(int min, int max);
// Flip a coin
int sFlipCoin();
// Flip a 3-sided coin
int sFlip3SideCoin();
// Flip a 4-sided coin 
int sFlip4SideCoin();

#endif /* __cplusplus */

#endif /* __smath_h */
