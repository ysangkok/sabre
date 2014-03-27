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
 * File   : copoly.h                             *
 * Date   : March, 1997                          *
 * Author : Dan Hammer                           *
 * "Oriented Poly" class. Polygons and shapes    *
 * which are defined in "port coordinants" so    *
 * that they can be translated and rotated.      *
 * To draw them, you need a "reference port"     *
 * which transforms them into world coordinants. *
 *************************************************/
#ifndef __copoly_h
#define __copoly_h

#ifndef __cpoly_h
#include "cpoly.h"
#endif

inline int O_CheckPlaneEquation(const Vector &point,
				const Vector &normal,
				const REAL_TYPE p_const)
{
  float dt1 = point.Dot(normal);
  if (dt1 + p_const >= 0)
    return 1;
  else
    return 0;
}

inline float O_Intensity( const Vector &light_source,
			  const Vector &normal)
{
  float val = light_source.Dot(normal) + 1.0;
  return (val / 2.0);
}

class C_Oriented_Poly : public C_Poly
{
public:
  // This member also allows scaling
  static REAL_TYPE co_scaler;
  C_Oriented_Poly();
  ~C_Oriented_Poly();
  int create(C_PolyInfo *);
  void set_world_points(Port_3D &);
  void set_poly_icolor(Vector &);
  void create_points(Port_3D &ref_port);
  void render_shadow(Port_3D &theport, int z_clip,
		     REAL_TYPE z_value,
		     Vector &light_source,
		     int shadow_color,
		     Port_3D &ref_port);
  void draw(Port_3D &theport, Port_3D &ref_port, int = 1);
  friend std::ostream & operator <<(std::ostream &, C_Oriented_Poly &);
  void write(std::ostream &os);
  int getScreenPoints(Port_3D &, Port_3D &, R_2DPoint *spoints);
  int getScreenPoints(Port_3D &, Port_3D &, TR_2DPoint *spoints);

};

inline void set_co_scaler(REAL_TYPE sc)
{
  C_Oriented_Poly::co_scaler = sc;
}

class C_Oriented_Shape : public C_Shape
{
public:
  C_Oriented_Poly *o_polys;
  Port_3D *ref_port;

  C_Oriented_Shape();
  ~C_Oriented_Shape();
  int create(C_ShapeInfo *info_ptr);
  void set_world_location(R_3DPoint &)
    { }
  void set_visible(Port_3D &);
  void draw(Port_3D &, R_3DPoint &);
  void render_shadow(Port_3D &, Vector &);
  void set_world_points(Port_3D &);
  void set_poly_icolor(Vector &light);
  void set_poly_color(int n, int color);
  int drawn_flag;
  friend std::istream &operator >>(std::istream &, C_Oriented_Shape &);
  void set_params(shape_params *);
  friend std::ostream &operator <<(std::ostream &, C_Oriented_Shape &);
  void create_points(Port_3D &ref_port);
  void delete_points();
};

#endif
