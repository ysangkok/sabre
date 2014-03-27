/*
    Sabre Fighter Plane Simulator 
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
 *           Sabre Fighter Plane Simulator       *
 * File   : clip.C                               *
 * Date   : March, 1997                          *
 * Author : Dan Hammer                           *
 * Various implementations of the Cohen-Sutherland *
 * clipping algorythm.                           *
 *************************************************/
#include <stdio.h>
#include <iostream>
#include <math.h>
#include "defs.h"
#include "grafix.h"
#include "vmath.h"
#include "clip.h"

enum { C_TOP=0x1,C_BOTTOM=0x2,C_RIGHT=0x4,C_LEFT=0x8 };

inline outcode CompOutCode(REAL_TYPE x, REAL_TYPE y,
									REAL_TYPE xmin, REAL_TYPE xmax,
									REAL_TYPE ymin, REAL_TYPE ymax)
{
	outcode code = 0;
	if (y > ymax)
		code |= C_TOP;
	else if (y < ymin)
		code |= C_BOTTOM;
	if (x > xmax)
		code |= C_RIGHT;
	else if (x < xmin)
		code |= C_LEFT;
	return code;
}

int ch_clip(int *points, Rect *bounds)
{
REAL_TYPE x0,y0,x1,y1,xmin,ymin,xmax,ymax;
outcode outcode0,outcode1,outcodeOut;
int accept = 0, done = 0;

	x0 = REAL_TYPE(points[0]);
	y0 = REAL_TYPE(points[1]);
	x1 = REAL_TYPE(points[2]);
	y1 = REAL_TYPE(points[3]);
	xmin = REAL_TYPE(bounds->topLeft.x);
	xmax = REAL_TYPE(bounds->botRight.x);
	ymin = REAL_TYPE(bounds->topLeft.y);
	ymax = REAL_TYPE(bounds->botRight.y);

	outcode0 = CompOutCode(x0,y0,xmin,xmax,ymin,ymax);
	outcode1 = CompOutCode(x1,y1,xmin,xmax,ymin,ymax);
	do
	{
		if ( ! ( outcode0 | outcode1 ) )
		{
			done = 1;
			accept = 1;
		}
		else if ( outcode0 & outcode1)
			done = 1;
		else
		{
			REAL_TYPE x,y;
			outcodeOut = outcode0 ? outcode0 : outcode1;
			if (outcodeOut & C_TOP)
			{
				x = x0 + ((x1 - x0) * ((ymax - y0) / (y1 - y0)));
				y = ymax;
			}
			else if (outcodeOut & C_BOTTOM)
			{
				x = x0 + ((x1 - x0) * ((ymin - y0) / (y1 - y0)));
				y = ymin;
			}
			else if (outcodeOut & C_RIGHT)
			{
				y = y0 + ((y1 - y0) * ((xmax - x0) / (x1 - x0)));
				x = xmax;
			}
			else
			{
				y = y0 + ((y1 - y0) * ((xmin - x0) / (x1 - x0)));
				x = xmin;
			}
			if (outcodeOut == outcode0)
			{
				x0 = x; y0 = y;
				outcode0 = CompOutCode(x0,y0,xmin,xmax,ymin,ymax);
			}
			else
			{
				x1 = x; y1 = y;
				outcode1 = CompOutCode(x1,y1,xmin,xmax,ymin,ymax);
			}
		}
	} while (!done);

	if (accept)
	{
		points[0] = (int) x0;
		points[1] = (int) y0;
		points[2] = (int) x1;
		points[3] = (int) y1;
	}
	return accept;
}

/* clip a line with z info */
int ch_zclip(R_2DPoint &pt0, R_2DPoint &pt1, Rect *bounds)
{
REAL_TYPE x0,y0,z0,x1,y1,z1,xmin,ymin,xmax,ymax;
outcode outcode0,outcode1,outcodeOut;
int accept = 0, done = 0;

	x0 = REAL_TYPE(pt0.x);
	y0 = REAL_TYPE(pt0.y);
	z0 = REAL_TYPE(pt0.z);

	x1 = REAL_TYPE(pt1.x);
	y1 = REAL_TYPE(pt1.y);
	z1 = REAL_TYPE(pt1.z);

	xmin = REAL_TYPE(bounds->topLeft.x);
	xmax = REAL_TYPE(bounds->botRight.x);
	ymin = REAL_TYPE(bounds->topLeft.y);
	ymax = REAL_TYPE(bounds->botRight.y);

	outcode0 = CompOutCode(x0,y0,xmin,xmax,ymin,ymax);
	outcode1 = CompOutCode(x1,y1,xmin,xmax,ymin,ymax);

	do
	{
		if ( ! ( outcode0 | outcode1 ) )
		{
			done = 1;
			accept = 1;
		}
		else if ( outcode0 & outcode1)
			done = 1;
		else
		{
			REAL_TYPE x,y,z;
			outcodeOut = outcode0 ? outcode0 : outcode1;
			if (outcodeOut & C_TOP)
			{
				x = x0 + ((x1 - x0) * ((ymax - y0) / (y1 - y0)));
				z = z0 + ((z1 - z0) * ((ymax - y0) / (y1 - y0)));
				y = ymax;
			}
			else if (outcodeOut & C_BOTTOM)
			{
				x = x0 + ((x1 - x0) * ((ymin - y0) / (y1 - y0)));
				z = z0 + ((z1 - z0) * ((ymin - y0) / (y1 - y0)));
				y = ymin;
			}
			else if (outcodeOut & C_RIGHT)
			{
				y = y0 + ((y1 - y0) * ((xmax - x0) / (x1 - x0)));
				z = z0 + ((z1 - z0) * ((xmax - x0) / (x1 - x0)));
				x = xmax;
			}
			else
			{
				y = y0 + ((y1 - y0) * ((xmin - x0) / (x1 - x0)));
				z = z0 + ((z1 - z0) * ((xmin - x0) / (x1 - x0)));
				x = xmin;
			}
			if (outcodeOut == outcode0)
			{
				x0 = x; y0 = y;
				z0 = z;
				outcode0 = CompOutCode(x0,y0,xmin,xmax,ymin,ymax);
			}
			else
			{
				x1 = x; y1 = y;
				z1 = z;
				outcode1 = CompOutCode(x1,y1,xmin,xmax,ymin,ymax);
			}
		}
	} while (!done);

	if (accept)
	{
		pt0.x = (R2D_TYPE) x0;
		pt0.y = (R2D_TYPE) y0;
		pt0.z = (R2D_TYPE) z0;
		pt1.x = (R2D_TYPE) x1;
		pt1.y = (R2D_TYPE) y1;
		pt1.z = (R2D_TYPE) z1;
	}
	return accept;
}

enum sides { left, top, right, bottom };
Rect *bounds;

static int clip_polys[3][MAX_CLIP];
static int clip_counts[3];

inline int inside(int side, int *pt)
{
	if (side == left)
		return (*pt >= bounds->topLeft.x);
	if (side == top)
		return (*(pt+1) >= bounds->topLeft.y);
	if (side == right)
		return (*pt <= bounds->botRight.x);
	if (side == bottom)
		return (*(pt+1) <= bounds->botRight.y);
	return -1;
}

// Calculate the intersection between first & second for the side
inline void intersect(int *first, int *second, int *result, int side)
{
REAL_TYPE x0,y0,x1,y1,xR,yR,m;
x0 = REAL_TYPE(first[0]);
y0 = REAL_TYPE(first[1]);
x1  = REAL_TYPE(second[0]);
y1 = REAL_TYPE(second[1]);

	if (x1 - x0 == 0)
		m = 0;
	else
		m = (y1 - y0) / (x1 - x0);
	if (side == left || side == right)
	{
		if (side == left)
			xR = bounds->topLeft.x;
		else
			xR = bounds->botRight.x;
		yR = (m * (xR - x0)) + y0;
	}
	else
	{
		if (side == top)
			yR = bounds->topLeft.y;
		else
			yR = bounds->botRight.y;
		if (m == 0)
			xR = x0;
		else
			xR = ((yR - y0) / m) +  x0;
	}
	result[0] = (int) xR;
	result[1] = (int) yR;
}

int poly_clip(int *in_poly, int *clipped_poly, int n,
	      int *n_out, Rect *bnds)
{
int *out_ptr;
int *out_n = NULL;
int *in_ptr;
int in_n;

	bounds = bnds;
	// do for all four sides of bounds rectangle
	for (int side = 0; side < 4; side++)
	{
		if (side < 3)
		{
			out_ptr = & clip_polys[side][0];
			out_n = &clip_counts[side];
		}
		else
		{
			out_ptr = clipped_poly;
			out_n = n_out;
		}
		if (side > 0)
		{
			in_ptr = & clip_polys[side-1][0];
			in_n = clip_counts[side-1];
		}
		else
		{
			in_ptr = in_poly;
			in_n = n;
		}
		int *s = &in_ptr[(in_n*2)-2];
		int *p = in_ptr;
		*out_n = 0;
		for (int j = 0; j < in_n; j++)
		{
			if (inside(side,p))
			{
				if (inside(side,s))
				{
					*out_ptr++ = *p;
					*out_ptr++ = *(p + 1);
					(*out_n)++;
				}
				else
				{
					intersect(s,p,out_ptr,side);
					out_ptr += 2;
					*out_ptr++ = *p;
					*out_ptr++ = *(p + 1);
					*out_n += 2;
				}
			}
			else
			{
				if (inside(side,s))
				{
					intersect(s,p,out_ptr,side);
					out_ptr += 2;
					(*out_n)++;
				}
			}
			s = p;
			p += 2;
		}
	}
	return *out_n;
}


/************************************************
 * floating-point 2-D clip version              *
 ************************************************/
static float f_clip_polys[3][MAX_CLIP];
static int f_clip_counts[3];

inline int f_inside(int side, float *pt)
{
	if (side == left)
		return (*pt >= (float) bounds->topLeft.x);
	if (side == top)
		return (*(pt+1) >= (float) bounds->topLeft.y);
	if (side == right)
		return (*pt <= (float) bounds->botRight.x);
	if (side == bottom)
		return (*(pt+1) <= (float) bounds->botRight.y);
	return -1;
}

// Calculate the intersection between first & second for the side
inline void f_intersect(float *first, float *second, float *result, int side)
{
float x0,y0,x1,y1,xR,yR,m;

	x0 = first[0];
	y0 = first[1];
	x1 = second[0];
	y1 = second[1];
	if (x1 - x0 == 0)
		m = 0;
	else
		m = (y1 - y0) / (x1 - x0);
	if (side == left || side == right)
	{
		if (side == left)
			xR = (float) bounds->topLeft.x;
		else
			xR = (float) bounds->botRight.x;
		yR = (m * (xR - x0)) + y0;
	}
	else
	{
		if (side == top)
			yR = (float) bounds->topLeft.y;
		else
			yR = (float) bounds->botRight.y;
		if (m == 0)
			xR = x0;
		else
			xR = ((yR - y0) / m) +  x0;
	}
	result[0] = xR;
	result[1] = yR;
}

int f_poly_clip(float *in_poly, float *clipped_poly, int n,
		int *n_out, Rect *bnds)
{
float *out_ptr;
int *out_n = NULL;
float *in_ptr;
int in_n;

	bounds = bnds;
	// do for all four sides of bounds rectangle
	for (int side = 0; side < 4; side++)
	{
		if (side < 3)
		{
			out_ptr = & f_clip_polys[side][0];
			out_n = & f_clip_counts[side];
		}
		else
		{
			out_ptr = clipped_poly;
			out_n = n_out;
		}
		if (side > 0)
		{
			in_ptr = & f_clip_polys[side-1][0];
			in_n = f_clip_counts[side-1];
		}
		else
		{
			in_ptr = in_poly;
			in_n = n;
		}
		float *s = &in_ptr[(in_n*2)-2];
		float *p = in_ptr;
		*out_n = 0;
		for (int j = 0; j < in_n; j++)
		{
			if (f_inside(side,p))
			{
				if (f_inside(side,s))
				{
					*out_ptr++ = *p;
					*out_ptr++ = *(p + 1);
					(*out_n)++;
				}
				else
				{
					f_intersect(s,p,out_ptr,side);
					out_ptr += 2;
					*out_ptr++ = *p;
					*out_ptr++ = *(p + 1);
					*out_n += 2;
				}
			}
			else
			{
				if (f_inside(side,s))
				{
					f_intersect(s,p,out_ptr,side);
					out_ptr += 2;
					(*out_n)++;
				}
			}
			s = p;
			p += 2;
		}
	}
	return *out_n;
}

/***********************************************
 * 2D point with z_buffer clipping             *
 ***********************************************/
static R_2DPoint r_clip_polys[3][MAX_CLIP];
static int r_clip_counts[3];
static R2D_TYPE min_x,max_x;
static R2D_TYPE min_y,max_y;

inline int r_inside(int side, R_2DPoint *pt)
{
	if (side == left)
		return (pt->x >= min_x);
	if (side == top)
		return (pt->y >= min_y);
	if (side == right)
		return (pt->x <= max_x);
	if (side == bottom)
		return (pt->y <= max_y);
	return -1;
}

// Calculate the intersection between first & second for the side
inline void r_intersect(R_2DPoint *first, R_2DPoint *second,
								R_2DPoint *result, int side)
{
REAL_TYPE t;
REAL_TYPE x0,x1,y0,y1,z0,z1;

	x0 = REAL_TYPE(first->x);
	y0 = REAL_TYPE(first->y);
	z0 = REAL_TYPE(first->z);


	x1 = REAL_TYPE(second->x);
	y1 = REAL_TYPE(second->y);
	z1 = REAL_TYPE(second->z);


	if (side == left)
	{
		result->x = R2D_TYPE(min_x);
		t = (min_x - x0) / (x1 - x0);
		result->y = R2D_TYPE(y0 + (t * (y1 - y0)));
		result->z = R2D_TYPE(z0 + (t * (z1 - z0)));
	}
	else if (side == right)
	{
		result->x = R2D_TYPE(max_x);
		t = (max_x - x0) / (x1 - x0);
		result->y = R2D_TYPE(y0 + (t * (y1 - y0)));
		result->z = R2D_TYPE(z0 + (t * (z1 - z0)));
	}
	else if (side == top)
	{
		result->y = R2D_TYPE(min_y);
		t = (min_y - y0) / (y1 - y0);
		result->x = R2D_TYPE(x0 + (t * (x1 - x0)));
		result->z = R2D_TYPE(z0 + (t * (z1 - z0)));
	}
	else
	{
		result->y = R2D_TYPE(max_y);
		t = (max_y - y0) / (y1 - y0);
		result->x = R2D_TYPE(x0 + (t * (x1 - x0)));
		result->z = R2D_TYPE(z0 + (t * (z1 - z0)));
	}
}

int r_poly_clip(R_2DPoint *in_poly, R_2DPoint *clipped_poly, int n,
						int *n_out, Rect *bnds)
{
R_2DPoint *out_ptr;
int *out_n = NULL;
R_2DPoint *in_ptr;
int in_n;

	bounds = bnds;
	min_x = R2D_TYPE(bnds->topLeft.x);
	max_x = R2D_TYPE(bnds->botRight.x);
	min_y = R2D_TYPE(bnds->topLeft.y);
	max_y = R2D_TYPE(bnds->botRight.y);

	// do for all four sides of bounds rectangle
	for (int side = 0; side < 4; side++)
	{
		if (side < 3)
		{
			out_ptr = r_clip_polys[side];
			out_n = &r_clip_counts[side];
		}
		else
		{
			out_ptr = clipped_poly;
			out_n = n_out;
		}
		if (side > 0)
		{
			in_ptr = r_clip_polys[side-1];
			in_n = r_clip_counts[side-1];
		}
		else
		{
			in_ptr = in_poly;
			in_n = n;
		}

		R_2DPoint *s = &in_ptr[in_n-1];
		R_2DPoint *p = in_ptr;
		*out_n = 0;
		for (int j = 0; j < in_n; j++)
		{
			if (r_inside(side,p))
			{
				if (r_inside(side,s))
				{
					*out_ptr++ = *p;
					(*out_n)++;
				}
				else
				{
					r_intersect(s,p,out_ptr,side);
					out_ptr++;
					*out_ptr++ = *p;
					*out_n += 2;
				}
			}
			else
			{
				if (r_inside(side,s))
				{
					r_intersect(s,p,out_ptr,side);
					out_ptr++;
					(*out_n)++;
				}
			}
			s = p;
			p++;
		}
	}
	return *out_n;
}


/***********************************************
 * 2D point with texture/z_buffer info clipping *
 ***********************************************/
static TR_2DPoint tr_clip_polys[3][MAX_CLIP];
static int tr_clip_counts[3];

inline int tr_inside(int side, TR_2DPoint *pt)
{
	if (side == left)
		return (pt->x >= min_x);
	if (side == top)
		return (pt->y >= min_y);
	if (side == right)
		return (pt->x <= max_x);
	if (side == bottom)
		return (pt->y <= max_y);
	return -1;
}

// Calculate the intersection between first & second for the side
inline void tr_intersect(TR_2DPoint *first, TR_2DPoint *second,
		  TR_2DPoint *result, int side)
{
REAL_TYPE t;
REAL_TYPE x0,x1,y0,y1,z0,z1;
REAL_TYPE u0,u1,v0,v1;

	x0 = REAL_TYPE(first->x);
	y0 = REAL_TYPE(first->y);
	z0 = REAL_TYPE(first->z);
	u0 = REAL_TYPE(first->u);
	v0 = REAL_TYPE(first->v);

	x1 = REAL_TYPE(second->x);
	y1 = REAL_TYPE(second->y);
	z1 = REAL_TYPE(second->z);
	u1 = REAL_TYPE(second->u);
	v1 = REAL_TYPE(second->v);

	if (side == left)
	{
		result->x = R2D_TYPE(min_x);
		t = (min_x - x0) / (x1 - x0);
		result->y = R2D_TYPE(y0 + (t * (y1 - y0)));
		result->z = R2D_TYPE(z0 + (t * (z1 - z0)));
		result->u = R2D_TYPE(u0 + (t * (u1 - u0)));
		result->v = R2D_TYPE(v0 + (t * (v1 - v0)));
	}
	else if (side == right)
	{
		result->x = R2D_TYPE(max_x);
		t = (max_x - x0) / (x1 - x0);
		result->y = R2D_TYPE(y0 + (t * (y1 - y0)));
		result->z = R2D_TYPE(z0 + (t * (z1 - z0)));
		result->u = R2D_TYPE(u0 + (t * (u1 - u0)));
		result->v = R2D_TYPE(v0 + (t * (v1 - v0)));
	}
	else if (side == top)
	{
		result->y = R2D_TYPE(min_y);
		t = (min_y - y0) / (y1 - y0);
		result->x = R2D_TYPE(x0 + (t * (x1 - x0)));
		result->z = R2D_TYPE(z0 + (t * (z1 - z0)));
		result->u = R2D_TYPE(u0 + (t * (u1 - u0)));
		result->v = R2D_TYPE(v0 + (t * (v1 - v0)));
	}
	else
	{
		result->y = R2D_TYPE(max_y);
		t = (max_y - y0) / (y1 - y0);
		result->x = R2D_TYPE(x0 + (t * (x1 - x0)));
		result->z = R2D_TYPE(z0 + (t * (z1 - z0)));
		result->u = R2D_TYPE(u0 + (t * (u1 - u0)));
		result->v = R2D_TYPE(v0 + (t * (v1 - v0)));
	}
}

int tr_poly_clip(TR_2DPoint *in_poly, TR_2DPoint *clipped_poly, int n,
					  int *n_out, Rect *bnds)
{
TR_2DPoint *out_ptr;
int *out_n = NULL;
TR_2DPoint *in_ptr;
int in_n;

	bounds = bnds;
	min_x = R2D_TYPE(bnds->topLeft.x);
	max_x = R2D_TYPE(bnds->botRight.x);
	min_y = R2D_TYPE(bnds->topLeft.y);
	max_y = R2D_TYPE(bnds->botRight.y);

	// do for all four sides of bounds rectangle
	for (int side = 0; side < 4; side++)
	{
		if (side < 3)
		{
			out_ptr = tr_clip_polys[side];
			out_n = &tr_clip_counts[side];
		}
		else
		{
			out_ptr = clipped_poly;
			out_n = n_out;
		}
		if (side > 0)
		{
			in_ptr = tr_clip_polys[side-1];
			in_n = tr_clip_counts[side-1];
		}
		else
		{
			in_ptr = in_poly;
			in_n = n;
		}

		TR_2DPoint *s = &in_ptr[in_n-1];
		TR_2DPoint *p = in_ptr;
		*out_n = 0;
		for (int j = 0; j < in_n; j++)
		{
			if (tr_inside(side,p))
			{
				if (tr_inside(side,s))
				{
					*out_ptr++ = *p;
					(*out_n)++;
				}
				else
				{
					tr_intersect(s,p,out_ptr,side);
					out_ptr++;
					*out_ptr++ = *p;
					*out_n += 2;
				}
			}
			else
			{
				if (tr_inside(side,s))
				{
					tr_intersect(s,p,out_ptr,side);
					out_ptr++;
					(*out_n)++;
				}
			}
			s = p;
			p++;
		}
	}
	return *out_n;
}


/***********************************************
 * 2DF point with texture/z_buffer info clipping *
 ***********************************************/
static TRF_2DPoint trf_clip_polys[3][MAX_CLIP];
static int trf_clip_counts[3];
static REAL_TYPE trf_min_x,trf_max_x,
					  trf_min_y,trf_max_y;

inline int trf_inside(int side, TRF_2DPoint *pt)
{
	if (side == left)
		return (pt->x >= trf_min_x);
	if (side == top)
		return (pt->y >= trf_min_y);
	if (side == right)
		return (pt->x <= trf_max_x);
	if (side == bottom)
		return (pt->y <= trf_max_y);
	return -1;
}

// Calculate the intersection between first & second for the side
inline void trf_intersect(TRF_2DPoint *first, TRF_2DPoint *second,
		  TRF_2DPoint *result, int side)
{
REAL_TYPE t;
REAL_TYPE x0,x1,y0,y1,z0,z1;
REAL_TYPE u0,u1,v0,v1;
REAL_TYPE w0,w1;

	x0 = REAL_TYPE(first->x);
	y0 = REAL_TYPE(first->y);
	z0 = REAL_TYPE(first->z);
	u0 = REAL_TYPE(first->u);
	v0 = REAL_TYPE(first->v);
	w0 = REAL_TYPE(first->w);

	x1 = REAL_TYPE(second->x);
	y1 = REAL_TYPE(second->y);
	z1 = REAL_TYPE(second->z);
	u1 = REAL_TYPE(second->u);
	v1 = REAL_TYPE(second->v);
	w1 = REAL_TYPE(second->w);

	if (side == left)
	{
		result->x = min_x;
		t = (min_x - x0) / (x1 - x0);
		result->y = y0 + (t * (y1 - y0));
		result->z = z0 + (t * (z1 - z0));
		result->u = u0 + (t * (u1 - u0));
		result->v = v0 + (t * (v1 - v0));
		result->w = w0 + (t * (w1 - w0));
	}
	else if (side == right)
	{
		result->x = max_x;
		t = (max_x - x0) / (x1 - x0);
		result->y = y0 + (t * (y1 - y0));
		result->z = z0 + (t * (z1 - z0));
		result->u = u0 + (t * (u1 - u0));
		result->v = v0 + (t * (v1 - v0));
		result->w = w0 + (t * (w1 - w0));
	}
	else if (side == top)
	{
		result->y = min_y;
		t = (min_y - y0) / (y1 - y0);
		result->x = x0 + (t * (x1 - x0));
		result->z = z0 + (t * (z1 - z0));
		result->u = u0 + (t * (u1 - u0));
		result->v = v0 + (t * (v1 - v0));
		result->w = w0 + (t * (w1 - w0));
	}
	else
	{
		result->y = max_y;
		t = (max_y - y0) / (y1 - y0);
		result->x = x0 + (t * (x1 - x0));
		result->z = z0 + (t * (z1 - z0));
		result->u = u0 + (t * (u1 - u0));
		result->v = v0 + (t * (v1 - v0));
		result->w = w0 + (t * (w1 - w0));
	}
}

int trf_poly_clip(TRF_2DPoint *in_poly, TRF_2DPoint *clipped_poly, int n,
					  int *n_out, Rect *bnds)
{
TRF_2DPoint *out_ptr;
int *out_n = NULL;
TRF_2DPoint *in_ptr;
int in_n;

	bounds = bnds;
	trf_min_x = bnds->topLeft.x;
	trf_max_x = bnds->botRight.x;
	trf_min_y = bnds->topLeft.y;
	trf_max_y = bnds->botRight.y;

	// do for all four sides of bounds rectangle
	for (int side = 0; side < 4; side++)
	{
		if (side < 3)
		{
			out_ptr = trf_clip_polys[side];
			out_n = &trf_clip_counts[side];
		}
		else
		{
			out_ptr = clipped_poly;
			out_n = n_out;
		}
		if (side > 0)
		{
			in_ptr = trf_clip_polys[side-1];
			in_n = trf_clip_counts[side-1];
		}
		else
		{
			in_ptr = in_poly;
			in_n = n;
		}

		TRF_2DPoint *s = &in_ptr[in_n-1];
		TRF_2DPoint *p = in_ptr;
		*out_n = 0;
		for (int j = 0; j < in_n; j++)
		{
			if (trf_inside(side,p))
			{
				if (trf_inside(side,s))
				{
					*out_ptr++ = *p;
					(*out_n)++;
				}
				else
				{
					trf_intersect(s,p,out_ptr,side);
					out_ptr++;
					*out_ptr++ = *p;
					*out_n += 2;
				}
			}
			else
			{
				if (trf_inside(side,s))
				{
					trf_intersect(s,p,out_ptr,side);
					out_ptr++;
					(*out_n)++;
				}
			}
			s = p;
			p++;
		}
	}
	return *out_n;
}

/*******************************************************
 * clipping in 3 dimensions                            *
 *******************************************************/
inline int inside_3D(R_3DPoint *pt, bounding_plane plane,
							bounding_cube *cube)
{
  if (plane == pz_min)
    return (pt->z >= cube->min_z);
  if (plane == pz_max)
    return (pt->z <= cube->max_z);
  if (plane == px_min)
    return (pt->x >= cube->min_x);
  if (plane == px_max)
    return (pt->x <= cube->max_x);
  if (plane == py_min)
    return (pt->y >= cube->min_y);
  if (plane == py_max)
    return (pt->y <= cube->max_y);
  return -1;
}

inline void intersect_3D(R_3DPoint *first, R_3DPoint *second,
								R_3DPoint *result, bounding_plane plane,
								bounding_cube *cube)
{
REAL_TYPE x0,y0,z0,x1,y1,z1,t;

	x0 = first->x;
	y0 = first->y;
	z0 = first->z;
	x1 = second->x;
	y1 = second->y;
	z1 = second->z;

	if (plane == px_min || plane == px_max)
	{
		REAL_TYPE x;
		if (plane == px_min)
			x = cube->min_x;
		else
			x = cube->max_x;
		t = (x - x0) / (x1 - x0);
		result->x = x;
		result->y = y0 + (t * (y1 - y0));
		result->z = z0 + (t * (z1 - z0));
	}
	else if (plane == py_min || plane == py_max)
	{
		REAL_TYPE y;
		if (plane == py_min)
			y = cube->min_y;
		else
			y = cube->max_y;
		t = (y - y0) / (y1 - y0);
		result->y = y;
		result->x = x0 + (t * (x1 - x0));
		result->z = z0 + (t * (z1 - z0));
	}
	else
	{
		REAL_TYPE z;
		if (plane == pz_min)
			z = cube->min_z;
		else
			z = cube->max_z;
		t = (z - z0) / (z1 - z0);
		result->z = z;
		result->y = y0 + (t * (y1 - y0));
		result->x = x0 + (t * (x1 - x0));
	}
}

int poly_clip_3D(R_3DPoint *in_poly, int in_count,
						R_3DPoint *out_poly, int *out_count,
						bounding_cube *cube, bounding_plane plane )
{
R_3DPoint *s = &in_poly[in_count-1];
R_3DPoint *p = in_poly;

	*out_count = 0;
	for (int j = 0; j < in_count; j++)
	{
		if (inside_3D(p,plane,cube))
		{
			if (inside_3D(s,plane,cube))
			{
				*out_poly++ = *p;
				(*out_count)++;
			}
			else
			{
				intersect_3D(s,p,out_poly,plane,cube);
				out_poly++;
				*out_poly++ = *p;
				(*out_count) += 2;
			}
		}
		else
		{
			if (inside_3D(s,plane,cube))
			{
				intersect_3D(s,p,out_poly,plane,cube);
				out_poly++;
				(*out_count)++;
			}
		}
		s = p;
		p++;
	}
	return (*out_count);
}


int poly_clip_3D(R_3DPoint *in_poly, int in_count,
						R_3DPoint *out_poly, int *out_count,
						bounding_cube *b_cube)
{
R_3DPoint clip_polys3[5][MAX_CLIP];
int clip_counts3[5];
int plane;
R_3DPoint *i_poly,*o_poly;
int i_count, *o_count;

	for (plane = px_min; plane <= pz_max; plane++)
	{
		if (plane == px_min)
		{
			i_poly = in_poly;
			i_count = in_count;
			o_poly = clip_polys3[0];
			o_count = &clip_counts3[0];
		}
		else if (plane == pz_max)
		{
			i_poly = clip_polys3[pz_min];
			i_count = clip_counts3[pz_min];
			o_poly = out_poly;
			o_count = out_count;
		}
		else
		{
			i_poly = clip_polys3[plane-1];
			i_count = clip_counts3[plane-1];
			o_poly = clip_polys3[plane];
			o_count = &clip_counts3[plane];
		}
		if (poly_clip_3D(i_poly,i_count,o_poly,o_count,b_cube,(bounding_plane) plane) <= 0)
			return 0;
	}
	return *out_count;
}

/********************************************************
 * Texture-mapped polygon clipping                      *
 ********************************************************/
inline void z_intersect(TR_3DPoint *tr1, TR_3DPoint *tr2,
			TR_3DPoint *result, REAL_TYPE z_min)
{
REAL_TYPE t;
REAL_TYPE u1,u2,v1,v2;

	t = (z_min - tr1->r.z) / (tr2->r.z - tr1->r.z);
	result->r.z = z_min;
	result->r.y = tr1->r.y + (t * (tr2->r.y - tr1->r.y));
	result->r.x = tr1->r.x + (t * (tr2->r.x - tr1->r.x));

	u1 = REAL_TYPE(tr1->e.u);
	u2 = REAL_TYPE(tr2->e.u);
	v1 = REAL_TYPE(tr1->e.v);
	v2 = REAL_TYPE(tr2->e.v);

	result->e.u = TXTP_TYPE(u1 + (t * (u2 - u1)));
	result->e.v = TXTP_TYPE(v1 + (t * (v2 - v1)));
	result->e.i = tr1->e.i;
}

int zpoly_clip(TR_3DPoint *in_poly, int in_count,
					TR_3DPoint *out_poly, int *out_count,
					REAL_TYPE z_min)
{
TR_3DPoint *s = &in_poly[in_count-1];
TR_3DPoint *p = in_poly;

	*out_count = 0;
	for (int j = 0; j < in_count; j++)
	{
		if (p->r.z >= z_min)
		{
			if (s->r.z >= z_min)
			{
				*out_poly++ = *p;
				(*out_count)++;
			}
			else
			{
				z_intersect(s,p,out_poly,z_min);
				out_poly++;
				*out_poly++ = *p;
				(*out_count) += 2;
			}
		}
		else
		{
			if (s->r.z >= z_min)
			{
				z_intersect(s,p,out_poly,z_min);
				out_poly++;
				(*out_count)++;
			}
		}
		s = p;
		p++;
	}
	return (*out_count);
}


/********************************************************
 * 3D Z-clipping                                        *
 ********************************************************/
inline void z_intersect(R_3DPoint *r1, R_3DPoint *r2,
			R_3DPoint *result, REAL_TYPE z_min)
{
REAL_TYPE t;

	t = (z_min - r1->z) / (r2->z - r1->z);
	result->z = z_min;
	result->y = r1->y + (t * (r2->y - r1->y));
	result->x = r1->x + (t * (r2->x - r1->x));
}

int zpoly_clip(R_3DPoint *in_poly, int in_count,
					R_3DPoint *out_poly, int *out_count,
					REAL_TYPE z_min)
{
R_3DPoint *s = &in_poly[in_count-1];
R_3DPoint *p = in_poly;

  *out_count = 0;
  for (int j = 0; j < in_count; j++)
    {
      if (p->z >= z_min)
	{
	  if (s->z >= z_min)
	    {
	      *out_poly++ = *p;
	      (*out_count)++;
	    }
	  else
	    {
	      z_intersect(s,p,out_poly,z_min);
	      out_poly++;
	      *out_poly++ = *p;
	      (*out_count) += 2;
	    }
	}
      else
	{
	  if (s->z >= z_min)
	    {
	      z_intersect(s,p,out_poly,z_min);
	      out_poly++;
	      (*out_count)++;
	    }
	}
      s = p;
      p++;
    }
  return (*out_count);
}

/* clip a 3D line against a minimum z value */
int zline_clip(R_3DPoint &p0, R_3DPoint &p1, REAL_TYPE min_z)
{
  R_3DPoint tmp;
  int accept;
  if (p0.z < min_z && p1.z < min_z)
    accept = 0;
  else if (p0.z >= min_z && p1.z >= min_z)
    accept = 1;
  else
    {
      accept = 1;
      z_intersect(&p0,&p1,&tmp,min_z);
      if (p0.z < min_z)
	p0 = tmp;
      else
	p1 = tmp;
    }
  return (accept);

}

void bounding_cube::set(R_3DPoint &p)
{
  if (!flg)
    {
      min_x = max_x = p.x;
      min_y = max_y = p.y;
      min_z = max_z = p.z;
      flg = 1;
    }
  else
    {
      if (p.x > max_x)
	max_x = p.x;
      if (p.x < min_x)
	min_x = p.x;
      if (p.y > max_y)
	max_y = p.y;
      if (p.y < min_y)
	min_y = p.y;
      if (p.z > max_z)
	max_z = p.z;
      if (p.z < min_z)
	min_z = p.z;
    }
}

void bounding_cube::set(R_2DPoint &p)
{
  if (!flg)
    {
      min_x = max_x = (REAL_TYPE) p.x;
      min_y = max_y = (REAL_TYPE) p.y;
      min_z = max_z = 0.0;
      flg = 1;
    }
  else
    {
      if (max_x < ((REAL_TYPE) p.x))
	max_x = (REAL_TYPE) p.x;
      if (min_x > ((REAL_TYPE) p.x))
	min_x = (REAL_TYPE) p.x;
      if (max_y < ((REAL_TYPE) p.y))
	max_y = (REAL_TYPE) p.y;
      if (min_y > ((REAL_TYPE) p.y))
	min_y = (REAL_TYPE) p.y;
    }
}

void bounding_cube::set(const bounding_cube &bc)
{
  if (!flg)
    {
      min_x = bc.min_x;
      max_x = bc.max_x;
      min_y = bc.min_y;
      max_y = bc.max_y;
      min_z = bc.min_z;
      max_z = bc.max_z;
      flg = 1;
    }
  else
    {
      if (bc.max_x > max_x)
	max_x = bc.max_x;
      if (bc.min_x < min_x)
	min_x = bc.min_x;
      if (bc.max_y > max_y)
	max_y = bc.max_y;
      if (bc.min_y < min_y)
	min_y = bc.min_y;
      if (bc.max_z > max_z)
	max_z = bc.max_z;
      if (bc.min_z < min_z)
	min_z = bc.min_z;
    }
}

R_3DPoint bounding_cube::centerPoint()
{
  R_3DPoint result;
  result.x = min_x + ((max_x - min_x) / 2);
  result.y = min_y + ((max_y - min_y) / 2);
  result.z = min_z + ((max_z - min_z) / 2);
  return(result);
}

/* 
   return 1 if the line created by points p1 and p2 intersects
   the cube b_cube
   */
int inbounds(const R_3DPoint &p1, const R_3DPoint &p2, const bounding_cube &b_cube)
{
  
  /* Trivial test */
  if (
      inbounds(p1,b_cube) ||
      inbounds(p2,b_cube)
      )
    return (1);

  REAL_TYPE t,d;
  R_3DPoint p0;
  REAL_TYPE x,y,z;

  /* For each plane, test for intersection */
  
  /**************************
   * x axis                 *
   **************************/
  // First check for parallel line
  d = p2.x - p1.x;
  if (d != 0.0)
    {
      /****************************
       * Plane on max_x           *
       ****************************/
      p0.x = b_cube.max_x;
      p0.y = b_cube.max_y;
      p0.z = b_cube.max_z;
      t = -( (p1.x - p0.x) / d );
      // Check if within line segment
      if (t >= 0.0 && t <= 1.0)
	{
	  // calc y, z for intersection point
	  y = p1.y + ((p2.y - p1.y) * t);
	  z = p1.z + ((p2.z - p1.z) * t);
	  // check if within bounds 
	  if ((y >= b_cube.min_y && y <= b_cube.max_y) &&
	      (z >= b_cube.min_z && z <= b_cube.max_z))
	    return (1);
	}

      /***************************
       * Plane on min_x          *
       ***************************/
      p0.x = b_cube.min_x;
      t = -( (p1.x - p0.x) / d );
      // Check if within line segment
      if (t >= 0.0 && t <= 1.0)
	{
	  // calc y, z for intersection point
	  y = p1.y + ((p2.y - p1.y) * t);
	  z = p1.z + ((p2.z - p1.z) * t);
	  // check if within bounds 
	  if ((y >= b_cube.min_y && y <= b_cube.max_y) &&
	      (z >= b_cube.min_z && z <= b_cube.max_z))
	    return (1);
	}
    }

  /**************************
   * y axis                 *
   **************************/
  // First check for parallel line
  d = p2.y - p1.y;
  if (d != 0.0)
    {
      /****************************
       * Plane on max_y           *
       ****************************/
      p0.x = b_cube.max_x;
      p0.y = b_cube.max_y;
      p0.z = b_cube.max_z;
      t = -( (p1.y - p0.y) / d );
      // Check if within line segment
      if (t >= 0.0 && t <= 1.0)
	{
	  // calc x, z for intersection point
	  x = p1.x + ((p2.x - p1.x) * t);
	  z = p1.z + ((p2.z - p1.z) * t);
	  // check if within bounds 
	  if ((x >= b_cube.min_x && x <= b_cube.max_x) &&
	      (z >= b_cube.min_z && z <= b_cube.max_z))
	    return (1);
	}

      /***************************
       * Plane on min_y          *
       ***************************/
      p0.z = b_cube.min_z;
      t = -( (p1.z - p0.z) / d );
      // Check if within line segment
      if (t >= 0.0 && t <= 1.0)
	{
	  // calc x, y for intersection point
	  x = p1.x + ((p2.x - p1.x) * t);
	  z = p1.z + ((p2.z - p1.z) * t);
	  // check if within bounds 
	  if ((x >= b_cube.min_x && x <= b_cube.max_x) &&
	      (z >= b_cube.min_z && z <= b_cube.max_z))
	    return (1);
	}
    }
  

  /**************************
   * z axis                 *
   **************************/
  // First check for parallel line
  d = p2.z - p1.z;
  if (d != 0.0)
    {
      /****************************
       * Plane on max_z           *
       ****************************/
      p0.x = b_cube.max_x;
      p0.y = b_cube.max_y;
      p0.z = b_cube.max_z;
      t = -( (p1.z - p0.z) / d );
      // Check if within line segment
      if (t >= 0.0 && t <= 1.0)
	{
	  // calc x, y for intersection point
	  x = p1.x + ((p2.x - p1.x) * t);
	  y = p1.y + ((p2.y - p1.y) * t);
	  // check if within bounds 
	  if ((x >= b_cube.min_x && x <= b_cube.max_x) &&
	      (y >= b_cube.min_y && y <= b_cube.max_y))
	    return (1);
	}

      /***************************
       * Plane on min_z          *
       ***************************/
      p0.z = b_cube.min_z;
      t = -( (p1.z - p0.z) / d );
      // Check if within line segment
      if (t >= 0.0 && t <= 1.0)
	{
	  // calc x, y for intersection point
	  x = p1.x + ((p2.x - p1.x) * t);
	  y = p1.y + ((p2.y - p1.y) * t);
	  // check if within bounds 
	  if ((x >= b_cube.min_x && x <= b_cube.max_x) &&
	      (y >= b_cube.min_y && y <= b_cube.max_y))
	    return (1);
	}
    }
  
  return (0);
}






