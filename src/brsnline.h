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
/************************************************************
 * brsnline.h                                               *
 ************************************************************/
#ifndef BRSNLINE_H
#define BRSNLINE_H

/*
 Bresenham Line class
 */

class BresenhamLine
{
public:
  int  x0,y0,x1,y1,dh,dv;
  int majorDelta,minorDelta,nn;
  int *majActPtr,*minActPtr,majActDelta,minActDelta;
  int xDir,xdelta,yDir,ydelta;
  int i,d;

    int X()
      {
	return(x0);
      }
    int Y()
      {
	return(y0);
      }
    int STEPS()
      {
	return (d);
      }
    
    BresenhamLine(int xx0, int yy0, int xx1, int yy1)
      {
	x0 = xx0;
	y0 = yy0;
	x1 = xx1;
	y1 = yy1;

	if (x1 < x0)
	  {
	    xDir = -1;
	    xdelta = -1;
	  }
	else
	  {
	    xDir = 1;
	    xdelta = 1;
	  }
	
	if (y1 < y0)
	  {
	    yDir = -1;
	    ydelta = -1;
	  }
	else
	  {
	    yDir = 1;
	    ydelta = 1;
	  }

	dh = abs(x1 - x0);
	dv = abs(y1 - y0);
	if (dh >= dv) 
	  {
	    majorDelta = dh;
	    minorDelta = dv;
	    d = dh + 1;
	    majActPtr = &x0;
	    majActDelta = xdelta;
	    minActPtr = &y0;
	    minActDelta = ydelta;
	  }
	else 
	  {
	    majorDelta = dv;
	    minorDelta = dh;
	    d = dv + 1;
	    majActPtr = &y0;
	    majActDelta = ydelta;
	    minActPtr = &x0;
	    minActDelta = xdelta;
	  }
	nn = 0;
	i  = 0;
      }
    
    int step()
      {
	if (++i < d) 
	  {
	    *majActPtr += majActDelta;
	    nn += minorDelta;
	    if (nn >= majorDelta)
	      {
		*minActPtr += minActDelta;
		nn = nn - majorDelta;
	      }
	    return (1);
	  }
	return (0);
      }
};
#endif













