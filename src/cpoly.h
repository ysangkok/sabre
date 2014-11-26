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
/*************************************************
 *           Sabre Fighter Plane Simulator              *
 * Version: 0.1                                  *
 * File   : cpoly.h                              *
 * Date   : June, 1997                           *
 * Author : Dan Hammer                           *
 * Classes for handling polygons, shapes and     *
 * some basic object definitions                 *
 *************************************************/
#ifndef __cpoly_h
#define __cpoly_h

#ifndef __vmath_h
#include "vmath.h"
#endif

#ifndef __port_3d_h
#include "port_3d.h"
#endif

#include "target.h"


#ifndef __rendpoly_h
#include "rendpoly.h"
#endif

#ifndef __rndrpoly_h
#include "rndrpoly.h"
#endif

#include "clstypes.h"

/****************************************
 * Bit-flag values used to set various  *
 * properties of polygons, shapes and   *
 * objects                              *
 ****************************************/
// C_Shape : Clip on minimum z-plane -
#define Z_CLIPME     1L
// C_Shape : Draw this shape first, don't sort by distance
#define ME_FIRST     2L
// C_Shape,C_Poly  : Render cast shadow
#define SHADOW       4L
// C_Shape : I'm an important shape, don't drop me when
// doing distance abstraction
#define MAJOR_SHAPE  8L
// C_Poly : Intensity shade me
#define I_SHADE      1L
// C_3DObject : I'm a target
#define HIT_ME       1L
// C_Shape : copy poly #1's params to all others
#define CPY_PARAMS   16L
// C_Poly  : texture map me
#define TXTR_MAP     2L

#define MAX_OBJECTS 512

inline int CheckPlaneEquation( const Vector &point,
const Vector &norm,
const REAL_TYPE pc)
{
REAL_TYPE dt1 = point.Dot(norm);
if (dt1 + pc >= 0)
return 0;
else
return 1;
}

inline REAL_TYPE Intensity( const Vector &light_source,
const Vector &norm)
{
  REAL_TYPE val = light_source.Dot(norm  ) + 1;
  return static_cast<REAL_TYPE>(val / 2.0);
}

class poly_params
{
public:
  int color;
  unsigned long flags;
  int color_range;
  int tmap;

  poly_params()
    : color(0),
    flags(0),
    color_range(1),
    tmap(-1)
    {}

  poly_params(int a_color,
	      unsigned long a_flag,
	      int a_color_range,
	      int a_tmap)
    : color(a_color),
    flags(a_flag),
    color_range(a_color_range),
    tmap(a_tmap)
    {}

  poly_params(const poly_params &pr)
    {
      copy(pr);
    }

  poly_params & operator=(const poly_params &pr)
    {
      copy(pr);
      return(*this);
    }

  void copy(const poly_params &pr)
    {
      color = pr.color;
      flags = pr.flags;
      color_range = pr.color_range;
      tmap = pr.tmap;
    }

  friend inline int operator ==(const poly_params & p1, const poly_params & p2);

  void read(std::istream &);
  void write(std::ostream &);
  friend inline std::istream &operator >>(std::istream & is, poly_params &ps);
  friend inline std::ostream &operator <<(std::ostream & os, poly_params &ps);
} ;


inline int operator ==(const poly_params &p1, const poly_params &p2)
{
  return ((p1.color == p2.color) && (p1.flags == p2.flags)
	  && (p1.color_range == p2.color_range)
	  && (p1.tmap == p2.tmap));
}

inline std::istream &operator >>(std::istream &is, poly_params &ps)
{
  ps.read(is);
  return(is);
}

inline std::ostream &operator <<(std::ostream &os, poly_params &ps)
{
  ps.write(os);
  return(os);
}

class shape_params
{
public:
  unsigned long flags;
  poly_params *p_params;
  unsigned int n_params;

  shape_params()
    : p_params(NULL),
    n_params(0)
    {}

  shape_params(int n, unsigned long flgs, poly_params *pr)
    : flags(flgs),
    p_params(pr),
    n_params(n)
    {}

  shape_params(const shape_params &sp)
    {
      p_params = NULL;
      n_params = 0;
      copy (sp);
    }

  shape_params &operator =(shape_params &sp)
    {
      copy (sp);
      return (*this);
    }

  void copy (const shape_params &);

  ~shape_params()
    {
      if (p_params != NULL)
	delete [] p_params;
    }

  void add(const poly_params &);

  void read(std::istream &);
  void write(std::ostream &);
  friend inline std::istream & operator >>(std::istream &, shape_params &);
  friend inline std::ostream & operator <<(std::ostream &, shape_params &);
} ;

inline std::istream & operator >>(std::istream &is, shape_params &sp)
{
  sp.read(is);
  return(is);
}

inline std::ostream & operator <<(std::ostream &os, shape_params &sp)
{
  sp.write(os);
  return(os);
}

class C_PolyInfo
{
public:
  unsigned int npoints;
  R_3DPoint *lpoints;
  TxtPoint  *tpoints;
  int delete_flag;
  bounding_cube bcube;

  C_PolyInfo()
    : npoints(0),
    lpoints(NULL),
    tpoints(NULL),
    delete_flag(0)
    {}

  C_PolyInfo(int np, R_3DPoint *pnts, TxtPoint *tpnts = NULL)
    :npoints(np),
    lpoints(pnts),
    tpoints(tpnts),
    delete_flag(0)
    {}

  C_PolyInfo(const C_PolyInfo &cp)
    {
      lpoints = NULL;
      tpoints = NULL;
      copy (cp);
    }

  C_PolyInfo &operator =(const C_PolyInfo &cp)
    {
      copy(cp);
      return(*this);
    }

  ~C_PolyInfo()
    { if (lpoints && delete_flag) delete [] lpoints; }

  C_PolyInfo &operator *=(REAL_TYPE scaler);
  C_PolyInfo &operator +=(const R_3DPoint &p);

  void copy(const C_PolyInfo &);

  void read(std::istream &);
  void write(std::ostream &);

  friend inline std::istream &operator >>(std::istream &is, C_PolyInfo &cp);
  friend inline std::ostream &operator <<(std::ostream &os, C_PolyInfo &cp);

};

inline std::istream &operator >>(std::istream &is, C_PolyInfo &cp)
{
  cp.read(is);
  return(is);
}

inline std::ostream &operator <<(std::ostream &os, C_PolyInfo &cp)
{
  cp.write(os);
  return(os);
}

class C_ShapeInfo
{
public:
  unsigned int npolys;
  C_PolyInfo *polyinfos;
  int delete_flag;
  bounding_cube bcube;


  C_ShapeInfo()
    : npolys(0),
    polyinfos(NULL),
    delete_flag(0)
    {}

  C_ShapeInfo(int np, C_PolyInfo *ip)
    :npolys(np),
    polyinfos(ip),
    delete_flag(0)
    {}

  C_ShapeInfo(const C_ShapeInfo &cs)
    {
      polyinfos = NULL;
      copy(cs);
    }

  C_ShapeInfo &operator =(const C_ShapeInfo &cs)
    {
      copy(cs);
      return(*this);
    }

  ~C_ShapeInfo()
    { if (delete_flag && polyinfos) delete [] polyinfos; }

  C_ShapeInfo &operator *=(REAL_TYPE scaler);
  C_ShapeInfo &operator +=(const R_3DPoint &p);
  void copy(const C_ShapeInfo &);
  void add(const C_PolyInfo &ip);

  void read(std::istream &);
  void write(std::ostream &);

  friend inline std::istream &operator >>(std::istream &, C_ShapeInfo &);
  friend inline std::ostream &operator <<(std::ostream &, C_ShapeInfo &);
};

inline std::istream &operator >>(std::istream &is, C_ShapeInfo &cp)
{
  cp.read(is);
  return(is);
}

inline std::ostream &operator <<(std::ostream &os, C_ShapeInfo &cp)
{
  cp.write(os);
  return(os);
}

extern C_ShapeInfo *shape_infos;

class C_3DObjectInfo
{
public:
  C_ShapeInfo *shapes;
  unsigned int nshapes;
  shape_params *dflt_params;
  bounding_cube bcube;
  char *path;
  char id[16];

  C_3DObjectInfo()
    :shapes(NULL),
    nshapes(0),
    dflt_params(NULL),
    path(NULL)
    {}

  ~C_3DObjectInfo()
    {
      if (shapes)
	delete [] shapes;
      if (dflt_params)
	delete [] dflt_params;
      if (path)
	delete path;
    }

  void readFile(char *path);
  void writeFile(char *path);
  void read(std::istream &);
  void write(std::ostream &);
  friend inline std::istream &operator >>(std::istream &is, C_3DObjectInfo &co);
  friend inline std::ostream &operator <<(std::ostream &os, C_3DObjectInfo &co);
};

inline std::istream &operator >>(std::istream &is, C_3DObjectInfo &co)
{
  co.read(is);
  return(is);
}

inline std::ostream &operator <<(std::ostream &os, C_3DObjectInfo &co)
{
  co.write(os);
  return(os);
}

class C_Poly
{
public:
  int color,base_color;
  int tmap;
  unsigned long flags;
  int color_range;
  int visible;
  REAL_TYPE scale;
  C_PolyInfo *p_info;
  R_3DPoint *wpoints;
  R_3DPoint *zpoints;
  Vector	surface_normal;
  REAL_TYPE plane_constant;
  int shadow_flag;
  static int txtrflag;

  C_Poly();
  virtual ~C_Poly();
  virtual int create(C_PolyInfo *);
  void compute_plane_constant(R_3DPoint &loc,
			      R_3DPoint *xpoints = NULL);
  void set_world_points(R_3DPoint &loc);
  void draw(Port_3D &theport, int = 1);
  void render_shadow(Port_3D &theport, int z_clip,
		     REAL_TYPE z_value,
		     Vector &light_source,
		     int shadow_color);
  void set_shadow(REAL_TYPE z_value, Vector &light_source);
  void draw_shadow(Port_3D &theport, int z_clip, int shadow_color);
  virtual void set_poly_icolor(Vector &);
  friend std::istream &operator >>(std::istream &, C_Poly &);
  void set_params(poly_params *prms);
  void set_params(const C_Poly &);
  virtual void set_scale(REAL_TYPE x)
    { scale = x; }
  virtual void getBounds(bounding_cube &);
  virtual void getWorldPoint(R_3DPoint &, int, R_3DPoint &);
  int getScreenPoints(Port_3D &, R_2DPoint *spoints);
  int getScreenPoints(Port_3D &, TR_2DPoint *spoints);
};

class C_Shape
{
public:
  unsigned int npolys;
  int visible;
  unsigned long flags;
  REAL_TYPE maxlen;
  C_Poly *polys;
  C_ShapeInfo *info_ptr;
  C_Shape();
  virtual ~C_Shape();
  R_3DPoint location;
  int memopt;

  virtual int create(C_ShapeInfo *info_ptr);
  virtual void set_poly_color(int n, int color);
  virtual void set_world_location(R_3DPoint &);
  virtual void set_visible(Port_3D &);
  virtual void draw_shadow(Port_3D &);
  virtual void set_shadow(REAL_TYPE , Vector &);
  virtual void draw(Port_3D &, R_3DPoint &);
  void set_poly_icolor(Vector &);
  friend std::istream & operator >>(std::istream &, C_Shape &);
  virtual void set_params(shape_params *);
  virtual void set_scale(REAL_TYPE);
  virtual void getBounds(bounding_cube &);
  REAL_TYPE shadow_z;
  Vector light_source;
};

#endif
