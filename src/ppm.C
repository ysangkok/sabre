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
 * File   : ppm.C                                *
 * Date   : March, 1997                          *
 * Author : Dan Hammer                           *
 * Write out the buffer as an ascii ppm file     *
 *************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include "vga_13.h"

static int fcount = 0;

void buffer2ppm()
{
  char buff[32];
  unsigned char *pptr;
  char r,g,b;
  int tk;
  unsigned char *buffer_ptr;

  buffer_ptr = lock_xbuff();
  if (!buffer_ptr)
	return;
  sprintf(buff,"sbr%04d.ppm",fcount++);
  std::ofstream os(buff);
  if (os)
    {
      os << "P3\n";
      os << "# CREATOR: Sabre Fighter Plane Simulator Version " << VERSION << " " << __DATE__ << std::endl;
      os << SCREEN_WIDTH << " " << SCREEN_HEIGHT << "\n";
      os << 63 << '\n';
      pptr = buffer_ptr;
      tk = 0;
      for (int i=0;i<SCREEN_WIDTH*SCREEN_HEIGHT;i++)
	{
	  get_rgb_value(*pptr++,&r,&g,&b);
	  sprintf(buff, "%3d %3d %3d ", (int)r,(int)g,(int)b);
	  os << buff;
	  tk++;
	  if (tk == 4)
	    {
	      os << "\n";
	      tk = 0;
	    }
	}
    }
  free_xbuff();
}
