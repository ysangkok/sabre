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
/*************************************************************************
 * convpoly.cpp                                                          *
 * for convex polygons                                                   *
 *************************************************************************/
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <values.h>
#include "defs.h"
#include "simerr.h"
#include "grafix.h"
#include "traveler.h"
#include "pen.h"
#include "convpoly.h"
#include "bits.h"
#include "vga_13.h"


static void BuildEdgeArray(int *l_points, int l_npoints);



#define LOW_VALUE -MAXINT
#define HIGH_VALUE MAXINT

//#define CHECK_BAD_POLY(x,y) if ( ( (y) > 300 ) || ( (y) < -300 ) || ( (x) > 400 ) || ( (x) < -400 ) ) { bad_poly = 1; return; }
// #define CHECK_POLY

static int *l_edge;
static int *r_edge;
static int min_Y,max_Y;
static int min_X,max_X;
static int bad_poly;
static Rect bnds(0, 0, 319, 199);
extern int frame_switch;
int frame_switch = 0;
extern const int frame_color;
const int frame_color = 10;

inline void SET_EDGE_BOUNDS(int x, int y)
{
  if (((y) >= 0 ) && ((y) <=  (SCREEN_HEIGHT - 1)))
    {
      int x_u = (x);
      int y_u = (y);
      if ((x_u) < 0)
	(x_u) = 0; 
      else if ((x_u)> (SCREEN_WIDTH - 1))
	(x_u) = (SCREEN_WIDTH - 1); 
      if ((y_u) < min_Y)
	min_Y = (y_u);
      if ((y_u) > max_Y)
	max_Y = (y_u);
      if ((x_u) < min_X)
	min_X = (x_u);
      if ((x_u) > max_X)
	max_X = (x_u);
      if (l_edge[(y_u)] > (x))
	l_edge[(y_u)] = (x);
      if (r_edge[(y_u)] < (x))
	r_edge[(y_u)] = (x);
    }
}

void init_edge_bounds()
{
  if (l_edge != NULL)
    delete l_edge;
  if (r_edge != NULL)
    delete r_edge;

  l_edge = new int[SCREEN_HEIGHT];
  MYCHECK(l_edge != NULL);
  r_edge = new int[SCREEN_HEIGHT];
  MYCHECK(r_edge != NULL);

  bnds.botRight.x = MAX_X;
  bnds.botRight.y = MAX_Y;

  for (int i=0;i<SCREEN_HEIGHT;i++)
    {
      l_edge[i] = HIGH_VALUE;
      r_edge[i] = LOW_VALUE;
    }
}

void BuildEdgeArray(int *l_points, int l_npoints)
{
	int i, yOld;
	int *p;

	bad_poly = 0;
	min_Y = HIGH_VALUE;
	max_Y = LOW_VALUE;
	min_X = HIGH_VALUE;
	max_X = LOW_VALUE;
	p = l_points;
	for (i = 0; i < (l_npoints-1); i++)
	{
#ifdef CHECK_POLY
		CHECK_BAD_POLY(*p,*(p+1))
		CHECK_BAD_POLY(*(p+2),*(p+3))
#endif
		SET_EDGE_BOUNDS(*p,*(p+1));
		// Horizontal line
		if (*(p+1) == *(p+3))
		{
			SET_EDGE_BOUNDS(*(p+2),*(p+3));
			p += 2;
			continue;
		}
		// Vertical line
		if (*p == *(p+2))
		{
			int x1 = *p;
			int dir = *(p + 1) < *(p + 3) ? 1 : -1;
			int i2 = *(p + 1) + dir;
			for (;i2 != *(p+3);i2 += dir)
			{
				SET_EDGE_BOUNDS(x1,i2);
			}
			p += 2;
			continue;
		}
		yOld = *(p+1);
		LineTraveler traveler(Point(*p,*(p+1)),Point(*(p+2),*(p+3)),1);
		while (traveler.here != traveler.there)
		{
		  // All this just to avoid a damned far call!
			if (traveler.majAct)
				traveler.here.x += traveler.xdelta;
			else
				traveler.here.y += traveler.ydelta;
			 traveler.n += traveler.minorDelta;
			if (traveler.n >= traveler.majorDelta)
			{
				if (traveler.majAct)
					traveler.here.y += traveler.ydelta;
				else
					traveler.here.x += traveler.xdelta;
				traveler.n = traveler.n - traveler.majorDelta;
			}
			if (traveler.here.y != yOld)
			  {
			    SET_EDGE_BOUNDS(traveler.here.x,traveler.here.y);
			  }
			yOld = traveler.here.y;
		}
		p += 2;
	}
}


void frame_convpoly(int *l_points, int l_npoints, int colr)
{
	int i;
	int *p;

	if (colr == -1)
	  colr = frame_color;
	p = l_points;
	for (i = 0; i < (l_npoints-1); i++)
	{
		b_linedraw(*p,*(p+1),*(p+2),*(p+3),colr,&bnds);
		p += 2;
	}
}


void fill_convpoly(int *points, int nPoints, int colr)
{
	int j;
	unsigned char  *b_ptr;
//	int *rm_ptr, *lm_ptr;
	unsigned char *buffer_ptr;

	if (frame_switch) {
		frame_convpoly(points,nPoints,colr);
		return;
	}
	BuildEdgeArray(points,nPoints);
#ifdef CHECK_POLY
	if (bad_poly)
	{
		printf("bad_poly!\n");
		exit(1);
	}
#endif
	buffer_ptr = lock_xbuff();
	if (!buffer_ptr)
		return;
	b_ptr = buffer_ptr + (min_Y * SCREEN_PITCH);
	for (j = min_Y; j <= max_Y; j++)
	{
		memset(b_ptr+l_edge[j],colr,
			r_edge[j] - l_edge[j] + 1);
		r_edge[j] = LOW_VALUE;
		l_edge[j] = HIGH_VALUE;
		b_ptr += SCREEN_PITCH;
	}
	free_xbuff();
}


