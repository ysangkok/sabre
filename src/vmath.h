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
#ifndef __vmath_h
#define __vmath_h

#include <math.h>
#include <iostream>


#ifndef REAL_TYPE
#define REAL_TYPE float
#endif

#define R2D_TYPE int
// #define FLOAT_Z
#define Z_PREC   24

extern const REAL_TYPE zfact;

extern const REAL_TYPE eps;

inline int r_equal(REAL_TYPE x, REAL_TYPE y)
{
  return (fabs(x - y) <= eps);
}

inline REAL_TYPE abs_max(REAL_TYPE a, REAL_TYPE b)
{
  double ax = fabs(a);
  double bx = fabs(b);
  return static_cast<REAL_TYPE>(ax > bx ? ax : bx);
}

inline REAL_TYPE arc_cos(REAL_TYPE a)
{
  if (a < -1.00)
    return 0.0;
  else if (a > 1.00)
    return 0.0;
  else
    return static_cast<REAL_TYPE>(acos(a));
}

inline REAL_TYPE arc_sin(REAL_TYPE a)
{
  if (a < -1.00)
    a = -1.00;
  else if (a > 1.00)
    a = 1.00;
  return static_cast<REAL_TYPE>(asin(a));
}

class R_3DPoint;

class Vector
{
public:
  REAL_TYPE X;
  REAL_TYPE Y;
  REAL_TYPE Z;

  Vector()
  {
    X=Y=Z=REAL_TYPE(0.0);
  }

  Vector( REAL_TYPE x, REAL_TYPE y, REAL_TYPE z )
  {
    X = x;
    Y = y;
    Z = z;
  }

  inline Vector(const R_3DPoint &);
  inline Vector(const R_3DPoint &p0, const R_3DPoint &p1);

  void operator *= ( REAL_TYPE scaler )
  {
    X *= scaler;
    Y *= scaler;
    Z *= scaler;
  }

  void operator /= ( REAL_TYPE scaler )
  {
    X /= scaler;
    Y /= scaler;
    Z /= scaler;
  }

  void operator += ( const Vector &v )
  {
    X += v.X; Y += v.Y; Z += v.Z;
  }
  void operator -= ( const Vector &v )
  {
    X -= v.X; Y -= v.Y; Z -= v.Z;
  }

  friend std::ostream &operator << (std::ostream &os,Vector &v)
  { return os  << '<' << v.X << ' ' << v.Y << ' ' << v.Z << '>' ; }

  friend std::istream &operator >> (std::istream &is,Vector &v);

  void SetZero( void )
  {
    X = Y = Z = 0;
  }

  Vector Cross(const Vector &v0) const
    {
      return Vector(
		    (Y * v0.Z) - (Z * v0.Y),
		    (Z * v0.X) - (X * v0.Z),
		    (X * v0.Y) - (Y * v0.X)
		    );
    }
  
  REAL_TYPE Dot( const Vector &v0 ) const
    {
      return ( (X * v0.X) + (Y * v0.Y) + (Z * v0.Z) );
    }

  void Normalize()
    {
      REAL_TYPE magnitude = Magnitude();
      if (magnitude > 0 )
	{
	  X = X / magnitude;
	  Y = Y / magnitude;
	  Z = Z / magnitude;
	}
      else
	X = Y = Z = 0;
    }

  REAL_TYPE Magnitude() const
    {
      REAL_TYPE f = (X*X) + (Y*Y) + (Z*Z);
      if (f > 0)
	return static_cast<REAL_TYPE>(sqrt(f));
      else
	return 0;
    }

  REAL_TYPE MagnitudeSquared() const
    {
      return ( ( (X * X) + (Y * Y) + (Z * Z) ) );
    }

  inline Vector & operator =(const R_3DPoint &);

  friend int operator ==(const Vector &v, const Vector &v1)
  { return (r_equal(v.X,v1.X) && r_equal(v.Y,v1.Y) && r_equal(v.Z,v1.Z)); }

  friend REAL_TYPE dot(const Vector &v0, const Vector &v1)
  { return (v0.X * v1.X + v0.Y * v1.Y + v0.Z * v1.Z); }

  friend Vector cross(const Vector &v0, const Vector &v1)
  {
    return (Vector(
		   v0.Y * v1.Z - v0.Z * v1.Y,
		   v0.Z * v1.X - v0.X * v1.Z,
		   v0.X * v1.Y - v0.Y * v1.X
		   ));
  }

  friend Vector operator *( const Vector &v0, const REAL_TYPE scaler)
    { Vector result(v0); result *= scaler; return result; }
  
  friend Vector operator +( const Vector &v0, const Vector &v1 )
    {
      return Vector( v0.X + v1.X, v0.Y + v1.Y, v0.Z + v1.Z );
    }

  friend Vector operator -( const Vector &v0, const Vector &v1 )
    {
      return Vector( v0.X - v1.X, v0.Y - v1.Y, v0.Z - v1.Z );
    }

  friend Vector operator -( const Vector &v )
    {
      return Vector( -v.X, -v.Y, -v.Z );
    }
};


// 3D Point in Rectangular coords
class S_3DPoint;
class R_3DPoint
{
public:
  REAL_TYPE x;
  REAL_TYPE y;
  REAL_TYPE z;
public:

  R_3DPoint(REAL_TYPE xi, REAL_TYPE yi, REAL_TYPE zi)
    :x(xi),
    y(yi),
    z(zi)
    {}

  R_3DPoint()
    :x(0),
    y(0),
    z(0)
    {}

  R_3DPoint(const R_3DPoint &r3D)
    :x(r3D.x),
    y(r3D.y),
    z(r3D.z)
    {}

  R_3DPoint & operator =(const R_3DPoint &r3D)
    {
      x = r3D.x;
      y = r3D.y;
      z = r3D.z;
      return *this;
    }

  R_3DPoint & operator +=(const R_3DPoint &r3D)
    {
      x += r3D.x;
      y += r3D.y;
      z += r3D.z;
      return *this;
    }

  R_3DPoint & operator -=(const R_3DPoint &r3D)
    {
      x -= r3D.x;
      y -= r3D.y;
      z -= r3D.z;
      return *this;
    }

  R_3DPoint & operator *=(const R_3DPoint &r3D)
    {
      x *= r3D.x;
      y *= r3D.y;
      z *= r3D.z;
      return *this;
    }

  R_3DPoint & operator *=(const REAL_TYPE r)
    {
      x *= r;
      y *= r;
      z *= r;
      return *this;
    }

  R_3DPoint & operator /=(const R_3DPoint &r3D)
    {
      x /= r3D.x;
      y /= r3D.y;
      z /= r3D.z;
      return *this;
    }
  R_3DPoint(const Vector &v)
    : x(v.X),
    y(v.Y),
    z(v.Z)
    {}

  R_3DPoint & operator =(const Vector &v)
    {
      x = v.X;
      y = v.Y;
      z = v.Z;
      return (*this);
    }

  R_3DPoint & operator +=(const Vector &v)
    {
      x += v.X;
      y += v.Y;
      z += v.Z;
      return *this;
    }

  R_3DPoint & operator /=(const Vector &v)
    {
      x /= v.X;
      y /= v.Y;
      z /= v.Z;
      return *this;
    }

  R_3DPoint & operator -=(const Vector &v)
    {
      x -= v.X;
      y -= v.Y;
      z -= v.Z;
      return *this;
    }

  R_3DPoint & operator *=(const Vector &v)
    {
      x *= v.X;
      y *= v.Y;
      z *= v.Z;
      return *this;
    }


  REAL_TYPE _x()
    { return x; }
  REAL_TYPE _y()
    { return y; }
  REAL_TYPE _z()
    { return z; }

  friend std::ostream &operator << (std::ostream &os,R_3DPoint &r3D)
    { return os  << '<' << r3D.x << ' ' << r3D.y << ' ' << r3D.z << '>' ; }

  friend std::istream &operator >> (std::istream &is,R_3DPoint &r3D);

  friend R_3DPoint operator +(const R_3DPoint &r1, const R_3DPoint &r2)
    { return R_3DPoint(r1.x+r2.x,r1.y+r2.y,r1.z+r2.z); }

  friend R_3DPoint operator -(const R_3DPoint &r1, const R_3DPoint &r2)
    { return R_3DPoint(r1.x-r2.x,r1.y-r2.y,r1.z-r2.z); }

  friend R_3DPoint operator *(const R_3DPoint &r1, const R_3DPoint &r2)
    { return R_3DPoint(r1.x*r2.x,r1.y*r2.y,r1.z*r2.z); }

  friend R_3DPoint operator /(const R_3DPoint &r1, const R_3DPoint &r2)
    { return R_3DPoint(r1.x/r2.x,r1.y/r2.y,r1.z/r2.z); }

  friend int operator ==(const R_3DPoint &r1, const R_3DPoint &r2)
    { return (r_equal(r1.x,r2.x) && r_equal(r1.y,r2.y)
	      && r_equal(r1.z,r2.z)); }

  friend REAL_TYPE distance_squared(const R_3DPoint &r1, const R_3DPoint &r2);
  friend REAL_TYPE distance(const R_3DPoint &r1, const R_3DPoint &r2);

  friend class S_3DPoint;
  friend class Port_3D;

};

inline REAL_TYPE distance_squared(const R_3DPoint &r1, const R_3DPoint &r2)
{
  REAL_TYPE dx = r1.x - r2.x;
  REAL_TYPE dy = r1.y - r2.y;
  REAL_TYPE dz = r1.z - r2.z;
  return ((dx*dx) + (dy*dy) + (dz*dz));
}

inline REAL_TYPE distance(const R_3DPoint &r1, const R_3DPoint &r2)
{
  return static_cast<REAL_TYPE>(sqrt(distance_squared(r1,r2)));
}

inline Vector to_nvector(const R_3DPoint &from, const R_3DPoint &to, REAL_TYPE m = 1)
{
  Vector result = Vector(to - from);
  result.Normalize();
  result *= m;
  return result;
}

inline Vector::Vector(const R_3DPoint &p0, const R_3DPoint &p1)
{
  X = p1.x - p0.x;
  Y = p1.y - p0.y;
  Z = p1.z - p0.z;
}
  
inline R_3DPoint seg2point(const R_3DPoint &from, R_3DPoint &to)
{
  return (R_3DPoint(Vector(from)
		    + to_nvector(from,to)));
}

inline Vector::Vector(const R_3DPoint &r3D)
  :X(r3D.x),
   Y(r3D.y),
   Z(r3D.z)
{

}

inline Vector &Vector::operator =(const R_3DPoint &r3D)
{
  X = r3D.x;
  Y = r3D.y;
  Z = r3D.z;
  return *this;
}


// 02/25/97
// For texture mapping
#define TXTP_TYPE REAL_TYPE
class TxtPoint
{
public:
  TXTP_TYPE u;
  TXTP_TYPE v;
  TXTP_TYPE i;

  TxtPoint(TXTP_TYPE ui, TXTP_TYPE vi, TXTP_TYPE ii)
    :u(ui),
    v(vi),
    i(ii)
    {}

  TxtPoint()
    :u(0),
    v(0),
    i(0)
    {}

  TxtPoint(const TxtPoint &r3D)
    :u(r3D.u),
    v(r3D.v),
    i(r3D.i)
    { }

  TxtPoint & operator =(const TxtPoint &r3D)
    {
      u = r3D.u;
      v = r3D.v;
      i = r3D.i;
      return *this;
    }

  TxtPoint & operator +=(const TxtPoint &r3D)
    {
      u += r3D.u;
      v += r3D.v;
      i += r3D.i;
      return *this;
    }

  TxtPoint & operator -=(const TxtPoint &r3D)
    {
      u -= r3D.u;
      v -= r3D.v;
      i -= r3D.i;
      return *this;
    }

  TxtPoint & operator *=(const TxtPoint &r3D)
    {
      u *= r3D.u;
      v *= r3D.v;
      i *= r3D.i;
      return *this;
    }

  TxtPoint & operator *=(const REAL_TYPE r)
    {
      u = TXTP_TYPE(REAL_TYPE(u) * r);
      v = TXTP_TYPE(REAL_TYPE(v) * r);
      i = TXTP_TYPE(REAL_TYPE(i) * r);
      return *this;
    }

  TxtPoint & operator /=(const TxtPoint &r3D)
    {
      u /= r3D.u;
      v /= r3D.v;
      i /= r3D.i;
      return *this;
    }

  TxtPoint & operator =(const Vector &vct)
    {
      u = TXTP_TYPE(vct.X);
      v = TXTP_TYPE(vct.Y);
      i = TXTP_TYPE(vct.Z);
      return (*this);
    }

  TXTP_TYPE _u()
    { return u; }
  TXTP_TYPE _v()
    { return v; }
  TXTP_TYPE _i()
    { return i; }

  friend std::ostream &operator << (std::ostream &os,TxtPoint &r3D)
    { return os  << '<' << r3D.u << ' ' << r3D.v << ' ' << r3D.i << '>' ; }

  friend std::istream &operator >> (std::istream &is,TxtPoint &r3D);

  friend TxtPoint operator +(const TxtPoint &r1, const TxtPoint &r2)
    { return TxtPoint(r1.u+r2.u,r1.v+r2.v,r1.i+r2.i); }

  friend TxtPoint operator -(const TxtPoint &r1, const TxtPoint &r2)
    { return TxtPoint(r1.u-r2.u,r1.v-r2.v,r1.i-r2.i); }

  friend TxtPoint operator *(const TxtPoint &r1, REAL_TYPE d)
    { return TxtPoint( REAL_TYPE(r1.u) * d, REAL_TYPE(r1.v) * d,
		       REAL_TYPE(r1.i) * d); }

  friend TxtPoint operator /(const TxtPoint &r1, REAL_TYPE d)
    { return TxtPoint( REAL_TYPE(r1.u) / d, REAL_TYPE(r1.v) / d,
		       REAL_TYPE(r1.i) / d ); }

  friend int operator ==(const TxtPoint &r1, const TxtPoint &r2)
    { return (r_equal(r1.u,r2.u) && r_equal(r1.v,r2.v)
	      && r_equal(r1.i,r2.i)); }
};

class TR_3DPoint
{
public:
  R_3DPoint r;
  TxtPoint  e;

  TR_3DPoint()
    {}

  TR_3DPoint(const TR_3DPoint &tr)
    {
      r = tr.r;
      e = tr.e;
    }

  TR_3DPoint(const R_3DPoint &rr, const TxtPoint &tt)
    {
      r = rr;
      e = tt;
    }

  TR_3DPoint & operator =(const TR_3DPoint &tr)
    {
      r = tr.r;
      e = tr.e;
      return (*this);
    }

};


class R_2DPoint
{
public:
  R2D_TYPE x;
  R2D_TYPE y;
  R2D_TYPE z;

  R_2DPoint()
    :x(R2D_TYPE(0)),
    y(R2D_TYPE(0)),
    z(R2D_TYPE(0))
    {}

  R_2DPoint(const R_2DPoint &pp)
    {
      x = pp.x;
      y = pp.y;
      z = pp.z;
    }

  R_2DPoint & operator =(const R_2DPoint &pp)
    {
      x = pp.x;
      y = pp.y;
      z = pp.z;
      return (*this);
    }
};

class TR_2DPoint
{
public:
  R2D_TYPE x;
  R2D_TYPE y;
  R2D_TYPE z;
  R2D_TYPE u;
  R2D_TYPE v;
  R2D_TYPE i;

  TR_2DPoint()
    :x(R2D_TYPE(0)),
    y(R2D_TYPE(0)),
    z(R2D_TYPE(0)),
    u(R2D_TYPE(0)),
    v(R2D_TYPE(0)),
    i(R2D_TYPE(0))
    {}

  TR_2DPoint(const TR_2DPoint &pp)
    {
      x = pp.x;
      y = pp.y;
      z = pp.z;
      u = pp.u;
      v = pp.v;
      i = pp.i;
    }

  TR_2DPoint & operator =(const TR_2DPoint &pp)
    {
      x = pp.x;
      y = pp.y;
      z = pp.z;
      u = pp.u;
      v = pp.v;
      i = pp.i;
      return (*this);
    }
};

class TRF_2DPoint
{
public:
  REAL_TYPE x;
  REAL_TYPE y;
  REAL_TYPE z;
  REAL_TYPE u;
  REAL_TYPE v;
  REAL_TYPE i;
  REAL_TYPE w;

  TRF_2DPoint()
    :x(REAL_TYPE(0)),
    y(REAL_TYPE(0)),
    z(REAL_TYPE(0)),
    u(REAL_TYPE(0)),
    v(REAL_TYPE(0)),
    i(REAL_TYPE(0)),
	 w(REAL_TYPE(1))
    {}

  TRF_2DPoint(const TRF_2DPoint &pp)
    {
      x = pp.x;
      y = pp.y;
      z = pp.z;
      u = pp.u;
      v = pp.v;
      i = pp.i;
		w = pp.w;
    }

  TRF_2DPoint & operator =(const TRF_2DPoint &pp)
    {
      x = pp.x;
      y = pp.y;
      z = pp.z;
      u = pp.u;
      v = pp.v;
      i = pp.i;
		w = pp.w;
      return (*this);
    }
};

// 3D Point in spherical coords
class S_3DPoint
{
public:
  REAL_TYPE theta;
  REAL_TYPE phi;
  REAL_TYPE rho;
public:

  S_3DPoint(REAL_TYPE thetai, REAL_TYPE phii, REAL_TYPE rhoi)
    :theta(thetai),
    phi(phii),
    rho(rhoi)
    {}

  S_3DPoint()
    :theta(0),
    phi(0),
    rho(0)
    {}

  S_3DPoint(const S_3DPoint &s3D)
    :theta(s3D.theta),
    phi(s3D.phi),
    rho(s3D.rho)
    { }

  S_3DPoint & operator =(const S_3DPoint &s3D)
    {
      theta = s3D.theta;
      phi = s3D.phi;
      rho = s3D.rho;
      return *this;
    }

  S_3DPoint & operator +=(const S_3DPoint &s3D)
    {
      theta += s3D.theta;
      phi += s3D.phi;
      rho += s3D.rho;
      return *this;
    }

  S_3DPoint & operator -=(const S_3DPoint &s3D)
    {
      theta -= s3D.theta;
      phi -= s3D.phi;
      rho -= s3D.rho;
      return *this;
    }

  S_3DPoint & operator *=(const S_3DPoint &s3D)
    {
      theta *= s3D.theta;
      phi *= s3D.phi;
      rho *= s3D.rho;
      return *this;
    }

  S_3DPoint & operator /=(const S_3DPoint &s3D)
    {
      theta /= s3D.theta;
      phi /= s3D.phi;
      rho /= s3D.rho;
      return *this;
    }

  REAL_TYPE _theta()
    { return theta; }
  REAL_TYPE _phi()
    { return phi; }
  REAL_TYPE _rho()
    { return rho; }

  friend std::istream &operator >>( std::istream &is, S_3DPoint &s3D);

  friend std::ostream &operator << ( std::ostream &os, S_3DPoint &s3D)
    { return os  << "<" << s3D.theta << " " << s3D.phi << " " << s3D.rho << ">"; }

  friend S_3DPoint operator +(const S_3DPoint &s1, const S_3DPoint &s2)
    { return S_3DPoint(s1.theta+s2.theta,s1.phi+s2.phi,s1.rho+s2.rho); }

  friend S_3DPoint operator -(const S_3DPoint &s1, const S_3DPoint &s2)
    { return S_3DPoint(s1.theta-s2.theta,s1.phi-s2.phi,s1.rho-s2.rho); }

  friend S_3DPoint operator *(const S_3DPoint &s1, const S_3DPoint &s2)
    { return S_3DPoint(s1.theta*s2.theta,s1.phi*s2.phi,s1.rho*s2.rho); }

  friend S_3DPoint operator /(const S_3DPoint &s1, const S_3DPoint &s2)
    { return S_3DPoint(s1.theta/s2.theta,s1.phi/s2.phi,s1.rho/s2.rho); }

  friend class R_3DPoint;
  friend class Port_3D;
  friend class C_Poly;

};

#endif


