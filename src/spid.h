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
 * File   : spid.h                               *
 * Date   : April, 1998                          *
 * Author : Dan Hammer                           *
 * PID class                                     *
 * PID stands for Proportional Integral          *
 * Derivative. These are 3 coefficiants which    *
 * are used in a standard control algorythm:     *
 * Given a set point y (where you want to be),   *
 * a current value x (where you are), and a rate *
 * of change r:                                  *
 * distance = y - x                              *
 * per = (distance * p - r * d) * i              *
 * The work is getting the p,i and d coefficiants*
 * balanced for a given flight model. We also use*
 * a "base" which represents the air-speed at    *
 * which the 3 coefficiants have been balanced ..*
 * we than adjust them to the current speed by   *
 * making them proportional to the square of the *
 * the base speed over the current speed         *
 *************************************************/
#ifndef __spid_h
#define __spid_h

#include "smath.h"

#define DEFAULT_P      1.0
#define DEFAULT_I      1.0
#define DEFAULT_D      2.0
#define DEFAULT_BASE   (260.0*260.0)
/*
 * Make "C" happy
 */
typedef struct _sPID_struct
{
  sREAL p;
  sREAL i;
  sREAL d;
  sREAL base;
} _sPID;

#ifdef __cplusplus
extern "C" {
#endif
extern void default_sPID(_sPID *spid);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

class sPID
{
public:
  sREAL p;
  sREAL i;
  sREAL d;
  sREAL base;

  sPID()
    :p(DEFAULT_P),
    i(DEFAULT_I),
    d(DEFAULT_D),
    base(DEFAULT_BASE*DEFAULT_BASE)
    {}

  void Copy(const sPID &spid)
    {
      p = spid.p;
      i = spid.i;
      d = spid.d;
      base = spid.base;
    }
  
  sPID(const sPID &spid)
    {
      Copy(spid);
    }

  const sPID &operator =(const sPID &spid)
    {
      Copy(spid);
      return *this;
    }

  inline const sPID &operator =(const _sPID & _spid);
  inline sREAL CalcPer(sREAL setPoint, sREAL curPoint, sREAL rate, sREAL curBase);  
};

inline sREAL sPID::CalcPer(sREAL setPoint, sREAL curPoint, sREAL rate, sREAL curBase)
{
  sREAL fact;
  sREAL pp,ii,dd;
  sREAL result;
  sREAL distance;

  fact = base / (curBase+eps);
  pp = p * fact;
  ii = i * fact;
  dd = d * fact;
  distance = setPoint - curPoint;
  result = (distance * pp - rate * dd) * ii;
  return result;
}

inline const sPID &sPID::operator =(const _sPID & _spid)
{
  p = _spid.p;
  i = _spid.i;
  d = _spid.d;
  base = _spid.base;
  return *this;
}

#endif


#endif

