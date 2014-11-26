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
/*******************************************************************
 * Floating point equivalents of the vector classes                *
 *******************************************************************/

#ifndef __dvmath_h
#define __dvmath_h

/*
** Vector Definitions
*/
class DVector;
class DVector
{
public:

  float X, Y, Z;

  void operator *= ( float scaler )
  {
		X *= scaler;
		Y *= scaler;
		Z *= scaler;
  }

  void operator /= ( float scaler )
  {
		X /= scaler;
		Y /= scaler;
		Z /= scaler;
  }

  void operator += ( const DVector &v )
  {
	 X += v.X; Y += v.Y; Z += v.Z;
  }
  void operator -= ( const DVector &v )
  {
	 X -= v.X; Y -= v.Y; Z -= v.Z;
  }

  friend std::ostream &operator << (std::ostream &os,DVector &v)
	 { return os  << '<' << v.X << ' ' << v.Y << ' ' << v.Z << '>' ; }
  friend std::istream &operator >> (std::istream &is,DVector &v);

  void SetZero( void )
  {
	 X = Y = Z = 0;
  }

  DVector       Cross( const DVector &v ) const;
  float Dot( const DVector &v ) const;
  void   Normalize( void );
  float Magnitude( void ) const;
  float MagnitudeSquared( void ) const;

  DVector()
  {
		X = Y = Z = 0;
  }

  DVector( float x, float y, float z )
  {
	  X = x;
	  Y = y;
	  Z = z;
  }

  DVector(const DVector &dv)
  {
	  X = dv.X;
	  Y = dv.Y;
	  Z = dv.Z;
  }
  DVector(const R_3DPoint &);
  DVector(const Vector &);
  DVector & operator =(const DVector &v)
  {  X = v.X; Y = v.Y; Z = v.Z; return *this; }
  DVector & operator =(const R_3DPoint &);
  DVector & operator =(const Vector &);
  R_3DPoint to_R_3DPoint()
  { return R_3DPoint(X,Y,Z); }
  Vector to_vector()
  { return Vector(X,Y,Z); }
};

inline DVector operator -( const DVector &v )
{
	return DVector( -v.X, -v.Y, -v.Z );
}


inline DVector operator * ( const DVector &a, const float scaler)
{ DVector result(a); result *= scaler; return result; }

inline DVector operator + ( const DVector &a, const DVector &b )
{
	return DVector( a.X + b.X, a.Y + b.Y, a.Z + b.Z );
}

inline DVector operator - ( const DVector &a, const DVector &b )
{
	return DVector( a.X - b.X, a.Y - b.Y, a.Z - b.Z );
}

inline DVector DVector::Cross( const DVector &b ) const
{
	DVector result;
	result.X = (Y * b.Z) - (Z * b.Y);
	result.Y = (Z * b.X) - (X * b.Z);
	result.Z = (X * b.Y) - (Y * b.X);

	return result;
}

inline float DVector::Dot( const DVector &b ) const
{
	return ( (X * b.X) + (Y * b.Y) + (Z * b.Z) );
}

inline float DVector::Magnitude( void ) const
{
	float f = (X*X) + (Y*Y) + (Z*Z);
	if (f > 0)
		return static_cast<float>(sqrt(f));
	else
		return 0;
}

inline float DVector::MagnitudeSquared( void ) const
{
	return (  ( (X * X) + (Y * Y) + (Z * Z) ) );
}

inline void DVector::Normalize( void )
{

	float magnitude = Magnitude();
	if (magnitude > 0 )
	{
		X = X / magnitude;
		Y = Y / magnitude;
		Z = Z / magnitude;
	}
	else
		X = Y = Z = 0;
}

inline DVector::DVector(const R_3DPoint &r3D)
{
#ifdef USING_FIXED
	X = fix2f(r3D.x.value);
	Y = fix2f(r3D.y.value);
	Z = fix2f(r3D.z.value);
#else
	X = r3D.x;
	Y = r3D.y;
	Z = r3D.z;
#endif
}

inline DVector &DVector::operator =(const R_3DPoint &r3D)
{
#ifdef USING_FIXED
	X = fix2f(r3D.x.value);
	Y = fix2f(r3D.y.value);
	Z = fix2f(r3D.z.value);
#else
	X = r3D.x;
	Y = r3D.y;
	Z = r3D.z;
#endif
	return *this;
}

inline DVector::DVector(const Vector &v)
{
#ifdef USING_FIXED
	X = fix2f(v.X.value);
	Y = fix2f(v.Y.value);
	Z = fix2f(v.Z.value);
#else
	X = v.X;
	Y = v.Y;
	Z = v.Z;
#endif
}

inline DVector &DVector::operator =(const Vector &v)
{
#ifdef USING_FIXED
	X = fix2f(v.X.value);
	Y = fix2f(v.Y.value);
	Z = fix2f(v.Z.value);
#else
	X = v.X;
	Y = v.Y;
	Z = v.Z;
#endif
	return *this;
}


#endif
