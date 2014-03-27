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
/****************************************************************
 * vector.cpp                                                   *
 ****************************************************************/
#include <stdio.h>
#include <iostream.h>
#include <math.h>
#include "simfile.h"
#include "vmath.h"

const REAL_TYPE eps = 1E-7;
#ifndef FLOAT_Z
const REAL_TYPE zfact = REAL_TYPE(1L << Z_PREC);
#else
const REAL_TYPE zfact = REAL_TYPE(1.0);
#endif

istream &operator >> (istream &is, Vector &v)
{
  char c;
  READ_TOKI('<',is,c);
  is >> v.X >> v.Y >> v.Z >> c;
  READ_TOK('>',is,c);
  return is;
}

istream &operator >> (istream &is,R_3DPoint &r3D)
{
  char c;
  READ_TOKI('<',is,c)
    is >> r3D.x >> r3D.y >> r3D.z >> c;
  READ_TOK('>',is,c)
    return is;
}

/********
 */



istream &operator >> (istream &is, S_3DPoint &s3D)
{
  char c;
  READ_TOKI('<',is,c)
    is >> s3D.theta >> s3D.phi >> s3D.rho >> c;
  READ_TOK('>',is,c)
    return is;
}

istream &operator >> (istream &is, TxtPoint &tp)
{
  char c;
  READ_TOKI('<',is,c)
    is >> tp.u >> tp.v >> c;
  READ_TOK('>',is,c)
    return is;
}



