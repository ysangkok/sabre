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
 * File   : smath.C                              *
 * Date   : April, 1998                          *
 * Author : Dan Hammer                           *
 *************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "smath.h"

sREAL sDistanceSquared(const sPoint &p0, const sPoint &p1)
{
  sVector v0(p1.x - p0.x,p1.y - p0.y,p1.z - p0.z);
  return (v0.MagnitudeSquared());
}

sREAL sDistance(const sPoint &p0, const sPoint &p1)
{
  return (sSQRT(sDistanceSquared(p0,p1)));
}

void sVector2sAttitude(const sVector &vect, sAttitude &att)
{
  att.roll = 0.0;
  sREAL d;
  att.yaw = static_cast<sREAL>(atan2(-vect.x,vect.y+eps));
  d = sSQRT((vect.x*vect.x)+(vect.y*vect.y));
  att.pitch = C(atan2(vect.z,d+eps));
}

sREAL sRandPer()
{
  int d;
  d = rand();
  return static_cast<sREAL>(d) / RAND_MAX;
}

int sRand(int min, int max)
{
  int swap;
  int result;

  if (min > max)
    {
      swap = min;
      min = max;
      max = swap;
    }
  sREAL rmin = static_cast<sREAL>(min);
  sREAL rmax = static_cast<sREAL>(max) + 1;
  sREAL v = sRandPer();
  sREAL i = rmin + ((rmax - rmin) * v);
  result = static_cast<int>(i);
  if (result < min)
    result = min;
  if (result > max)
    result = max;
  return result;
}

int sFlipCoin()
{
  sREAL rp = sRandPer();
  int result = rp <= 0.5;
  return (result);
}

int sFlip3SideCoin()
{
  return (sRandPer() <= 0.333);
}

int sFlip4SideCoin()
{
  return (sRandPer() <= 0.25);
}

/***************************************************
 * matrix stuff                                    *
 ***************************************************/
void sMatrix3Build(const sAttitude &att, sVector *mat3)
{
  sVector tmp0[3];
  sVector tmp1[3];
  sVector tmp2[3];

  sMatrix3X(att.pitch,tmp0);
  sMatrix3Y(att.roll,tmp1);
  sMatrix3Mult(tmp0,tmp1,tmp2);
  sMatrix3Z(att.yaw,tmp0);
  sMatrix3Mult(tmp0,tmp2,mat3);
}

void sMatrix3Mult(sVector *mat0, sVector *mat1, sVector *mat2)
{
  mat2[0].x = mat0[0].x * mat1[0].x + mat0[0].y * mat1[1].x + mat0[0].z * mat1[2].x;
  mat2[0].y = mat0[0].x * mat1[0].y + mat0[0].y * mat1[1].y + mat0[0].z * mat1[2].y;
  mat2[0].z = mat0[0].x * mat1[0].z + mat0[0].y * mat1[1].z + mat0[0].z * mat1[2].z;

  mat2[1].x = mat0[1].x * mat1[0].x + mat0[1].y * mat1[1].x + mat0[1].z * mat1[2].x;
  mat2[1].y = mat0[1].x * mat1[0].y + mat0[1].y * mat1[1].y + mat0[1].z * mat1[2].y;
  mat2[1].z = mat0[1].x * mat1[0].z + mat0[1].y * mat1[1].z + mat0[1].z * mat1[2].z;

  mat2[2].x = mat0[2].x * mat1[0].x + mat0[2].y * mat1[1].x + mat0[2].z * mat1[2].x;
  mat2[2].y = mat0[2].x * mat1[0].y + mat0[2].y * mat1[1].y + mat0[2].z * mat1[2].y;
  mat2[2].z = mat0[2].x * mat1[0].z + mat0[2].y * mat1[1].z + mat0[2].z * mat1[2].z;
}

void sMatrix3X(sREAL angle, sVector *mat3)
{
  sREAL c,s;
  c = sCOS(angle);
  s = sSIN(angle);

  mat3[0].x = 1; mat3[0].y = 0; mat3[0].z = 0;
  mat3[1].x = 0; mat3[1].y = c; mat3[1].z = -s;
  mat3[2].x = 0; mat3[2].y = s; mat3[2].z = c;
}

void sMatrix3Y(sREAL angle, sVector *mat3)
{
  sREAL  c,s;
  c = sCOS(angle);
  s = sSIN(angle);

  mat3[0].x = c; mat3[0].y = 0; mat3[0].z = -s;
  mat3[1].x = 0; mat3[1].y = 1; mat3[1].z = 0;
  mat3[2].x = s; mat3[2].y = 0; mat3[2].z = c;
}

void sMatrix3Z(sREAL angle, sVector *mat3)
{
  sREAL c,s;
  c = sCOS(angle);
  s = sSIN(angle);

  mat3[0].x = c; mat3[0].y = -s; mat3[0].z = 0;
  mat3[1].x = s; mat3[1].y = c;  mat3[1].z = 0;
  mat3[2].x = 0; mat3[2].y = 0;  mat3[2].z = 1;
}

void sMatrix3Rotate(sVector *mat3, const sVector &v0, sVector &v1)
{
  v1.x = v0.x * mat3[0].x + v0.y * mat3[0].y + v0.z * mat3[0].z;
  v1.y = v0.x * mat3[1].x + v0.y * mat3[1].y + v0.z * mat3[1].z;
  v1.z = v0.x * mat3[2].x + v0.y * mat3[2].y + v0.z * mat3[2].z;
}

void sMatrix3Transpose(sVector *mat0, sVector *mat1)
{
  mat1[0].x = mat0[0].x;
  mat1[1].x = mat0[0].y;
  mat1[2].x = mat0[0].z;

  mat1[0].y = mat0[1].x;
  mat1[1].y = mat0[1].y;
  mat1[2].y = mat0[1].z;

  mat1[0].z = mat0[2].x;
  mat1[1].z = mat0[2].y;
  mat1[2].z = mat0[2].z;
}

void sGetRPH(const sVector &roll, const sVector &pitch, sAttitude  &att)
{
  sVector mat[3];
  sREAL r,p,h;
  sVector roll1,roll2;
  sVector pitch1;

  if (pitch.y != 0.0 || pitch.x != 0.0 )
    {
      h = C(atan2(pitch.y,pitch.x) - Pi * 0.5);
      sMatrix3Z(_2Pi-h,mat);
      //      MakeMatrixDoubleZ((2*PI-h),Mat);
      sMatrix3Rotate(mat,pitch,pitch1);
      sMatrix3Rotate(mat,roll,roll1);
      /*
	RotateDoubleVector(&Pitch,Mat);
	RotateDoubleVector(&Roll,Mat);
	*/
    }
  else
    h = 0;

  p = C(atan2(pitch.z, pitch.y));
  sMatrix3X(_2Pi-p,mat);
  //   MakeMatrixDoubleX((2*PI-p),Mat);
  sMatrix3Rotate(mat,roll1,roll2);
  // RotateDoubleVector(&Roll,Mat);
  r = C(atan2(roll2.z,roll2.x));

  att.pitch = p;
  att.roll = r;
  att.yaw = h;
  /*
    rph->x=p;
    rph->y=r;
    rph->z=h;
    */
}

/*
 *  Increment an angle & wrap around
 *  Pi
 */
sREAL sIncrementAngle(sREAL angle, sREAL inc)
{
  int cw = 1;   // clockwise
  if (inc <= 0.0)
    cw = 0;
  angle += inc;
  if (cw && angle > Pi)
    angle = -Pi + (angle - Pi);
  else if (angle <= -Pi)
    angle = Pi + (angle + Pi);
  return (angle);
}

/*
 *  Get angle between (shortest distance)
 */
sREAL GetMedianAngle(sREAL angle0, sREAL angle1);
sREAL GetMedianAngle(sREAL angle0, sREAL angle1)
{
  sREAL result;
  sREAL d = sGetCircularDistance(angle0, angle1);
  d /= 2.0;
  result = sIncrementAngle(angle0,d);
  return (result);
}

/*
 *	Get shortest arc length and direction between two angles 
 */
sREAL sGetCircularDistance(sREAL from, sREAL to)
{
  sREAL result;
  /* clockwise distance */
  sREAL cw_d = 0.0;
  /* counter-clockwise distance */
  sREAL ccw_d= 0.0;

  if ((to >= 0.0 && from >= 0.0)
      || 
      (to <= 0.0 && from <= 0.0))
    result = to - from;
  else
    {
      if (from >= 0.0)
	{
	  /* 0.0 > to > -Pi */
	  cw_d = Pi - from + Pi + to;
	  ccw_d = to - from;
	}
      else
	{
	  /* 0.0 > from > -Pi */
	  cw_d = to - from;
	  ccw_d = -Pi - from + to - Pi;
	}
      if (fabs(cw_d) < fabs(ccw_d))
	result = cw_d;
      else
	result = ccw_d;
    }
  return (result);
}

/*
 * is angle2 between angle0 & angle1 ?
 */
int sIsBetween(sREAL angle0, sREAL angle1, sREAL angle2)
{
  sREAL c_angle0,c_angle1,c_angle2;
  sREAL min,max;
  /*
   *  convert to 0 ... 2Pi
   */
  c_angle0 = ConvertAngle(angle0);
  c_angle1 = ConvertAngle(angle1);
  c_angle2 = ConvertAngle(angle2);
  sMinMax(c_angle0,c_angle1,min,max);
  return (c_angle2 >= min && c_angle2 <= max);
}
