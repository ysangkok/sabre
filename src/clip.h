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
/*************************************************
 *           SABRE Fighter Plane Simulator              *
 * Version: 0.1                                  *
 * File   : clip.h                               *
 * Date   : March, 1997                          *
 * Author : Dan Hammer                           *
 *************************************************/
#ifndef __clip_h
#define __clip_h

#include "grafix.h"
#include "vmath.h"

typedef unsigned int outcode;
int ch_clip(int *, Rect *bounds);
int ch_zclip(R_2DPoint &, R_2DPoint &, Rect *bounds);

#define MAX_CLIP 30
int poly_clip(int *poly, int *clipped_poly, int n,
	      int *n_out, Rect *bounds);
int f_poly_clip(float *in_poly, float *clipped_poly, int n,
		int *n_out, Rect *bnds);

int r_poly_clip(R_2DPoint *in_poly, R_2DPoint *clipped_poly, int n,
		 int *nout, Rect *bnd);

int tr_poly_clip(TR_2DPoint *in_poly, TR_2DPoint *clipped_poly, int n,
		 int *nout, Rect *bnd);

int trf_poly_clip(TRF_2DPoint *in_poly, TRF_2DPoint *clipped_poly, int n,
		 int *nout, Rect *bnd);

class bounding_cube
{
public:
  int flg;
  REAL_TYPE min_x,max_x;
  REAL_TYPE min_y,max_y;
  REAL_TYPE min_z,max_z;
  bounding_cube()
    {
      min_x = max_x = 0.0;
      min_y = max_y = 0.0;
      min_z = max_z = 0.0;
      flg = 0;
    }

  bounding_cube(const bounding_cube &bc)
    {
      min_x = bc.min_x;
      max_x = bc.max_x;
      min_y = bc.min_y;
      max_y = bc.max_y;
      min_z = bc.min_z;
      max_z = bc.max_z;
      flg = 0;
    }

  bounding_cube &operator =(const bounding_cube &bc)
    {
      min_x = bc.min_x;
      max_x = bc.max_x;
      min_y = bc.min_y;
      max_y = bc.max_y;
      min_z = bc.min_z;
      max_z = bc.max_z;
      flg = 0;
      return *this;
    }

  bounding_cube &operator *=(REAL_TYPE scaler)
    {
      min_x *= scaler;
      max_x *= scaler;
      min_y *= scaler;
      max_y *= scaler;
      min_z *= scaler;
      max_x *= scaler;
      return *this;
    }

  void reset()
    { flg = 0; }
  void set(R_3DPoint &p);
  void set(R_2DPoint &p);
  void set(const bounding_cube &bc);
  R_3DPoint centerPoint();

  void write(std::ostream &os)
    {
      os << "[ " << min_x << " " << max_x << " ]\n"; 
      os << "[ " << min_y << " " << max_y << " ]\n"; 
      os << "[ " << min_z << " " << max_z << " ]\n"; 
    }

  friend std::ostream &operator <<(std::ostream &os, bounding_cube &bc)
    {
      bc.write(os);
      return (os);
    }

} ;

inline int inbounds(const R_3DPoint &p, const bounding_cube &b_cube)
{
  return (
	  (p.z >= b_cube.min_z) && (p.z <= b_cube.max_z) &&
	  (p.y >= b_cube.min_y) && (p.y <= b_cube.max_y) &&
	  (p.x >= b_cube.min_x) && (p.x <= b_cube.max_x)
	  ) ;
}

int inbounds(const R_3DPoint &p1, const R_3DPoint &p2, const bounding_cube &b_cube);



enum bounding_plane { px_min, px_max, py_min,
			      py_max, pz_min, pz_max };

int poly_clip_3D(R_3DPoint *in_poly, int in_count,
		 R_3DPoint *out_poly, int *out_count,
		 bounding_cube *b_cube, bounding_plane plane );

int poly_clip_3D(R_3DPoint *in_poly, int in_count,
		 R_3DPoint *out_poly, int *out_count,
		 bounding_cube *b_cube);

int zpoly_clip(R_3DPoint *in_poly, int in_count,
	       R_3DPoint *out_poly, int *out_count,
	       REAL_TYPE min_z);

int zpoly_clip(TR_3DPoint *in_poly, int in_count,
	       TR_3DPoint *out_poly, int *out_count,
	       REAL_TYPE min_z);

int zline_clip(R_3DPoint &p0, R_3DPoint &p1, REAL_TYPE min_z);
#endif
