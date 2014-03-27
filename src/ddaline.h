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
 * ddaline.h                                                *
 ************************************************************/
#ifndef __ddaline_h
#define __ddaline_h

/*
 Line drawing class
 Adapted from Antti Barck's gdev::line() 
 */

class DDALine
{
public:
    int x0,y0,x1,y1;
    int dx;
    int dy;
    int pixcnt, 
        stepping, 
        step_up, 
        step_dn, 
        x_step, 
        x_back, 
        y_step, 
        y_back;
    int i;

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
	return (pixcnt);
      }
    
    DDALine(int xx0, int yy0, int xx1, int yy1)
      {
	x0 = xx0;
	y0 = yy0;
	x1 = xx1;
	y1 = yy1;
	dx = abs(x1 - x0);
	dy = abs(y1 - y0);
	if( dx >= dy ) 
	  {
	    pixcnt = dx + 1;
	    stepping=(2*dy)-dx;
	    step_up= 2 * dy;
	    step_dn= 2 * (dy-dx);
	    x_step = 1;
	    x_back = 1;
	    y_step = 1;
	    y_back = 0;
	  } 
	else 
	  {
	    pixcnt = dy + 1;
	    stepping=(2*dx)-dy;
	    step_up= 2 * dx;
	    step_dn= 2 * (dx-dy);
	    x_step = 1;
	    x_back = 0;
	    y_step = 1;
	    y_back = 1;
	  }
	
	if( x1 < x0 ) 
	  {
	    x_step = -x_step;
	    x_back = -x_back;
	  }
	
	if( y1 < y0 ) 
	  {
	    y_step = -y_step;
	    y_back = -y_back;
	  }
	i = 0;
      }
    
    int step()
      {
	if (++i < pixcnt) 
	  {
	    if( stepping < 0 ) 
	      {
		stepping += step_up;
		x0  += x_back;
		y0  += y_back;
	      } 
	    else 
	      {
		stepping += step_dn;
		x0  += x_step;
		y0  += y_step;
	      }
	    return (1);
	  }
	return (0);
      }
};
#endif













