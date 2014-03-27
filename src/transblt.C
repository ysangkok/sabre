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
/**************************************************************
 * transblt.cpp                                               *
 * A (hopefully) fast way of blitting 'transparent' bitmaps   *
 * (i.e. the cockpit), by mapping out beforehand the sections *
 * which are transparent                                      *
 **************************************************************/
#include <stdio.h>
#include <string.h>
#include "vga_13.h"
#include "grafix.h"
#include "transblt.h"

void trans_table::build_runs(unsigned char *img, int r, int w)
{
  int i,j, rn;
  unsigned char *img_ptr;
  char cur_type;
  trans_image = img;
  image_rows = r;
  image_width = w;
  ntrans_row = -1;
  for (i = 0; i < image_rows; i++)
    {
      img_ptr = trans_image + (image_width * i);
      rn = 0;
      if ((int)(*img_ptr++) == trans_color )
	runs[i][rn].type = TRANSP_TYPE;
      else
	runs[i][rn].type = NTRANSP_TYPE;
      runs[i][rn].len = 1;
//      cur_type = runs[i][rn].type;
      for (j = 1; j < image_width; j++)
	{
	  if ((int)(*img_ptr++) == trans_color )
	    cur_type = TRANSP_TYPE;
	  else
	    cur_type = NTRANSP_TYPE;
	  if (cur_type == runs[i][rn].type)
	    runs[i][rn].len++;
	  else
	    {
	      rn++;
	      runs[i][rn].type = cur_type;
	      runs[i][rn].len  = 1;
	    }

	}
      runs[i][++rn].type = RUN_END;
      if ((ntrans_row == -1 ) && (rn == 1) &&
	  (runs[i][0].type == NTRANSP_TYPE))
	ntrans_row = i;
    }
}

void trans_table::trans_blit(unsigned char *dst, int x, int y,
			     Rect *r, int fast_flag)
{
  unsigned char *dst_ptr,*dst_ptr1;
  unsigned char *src_ptr;
  int i,j;

  dst_ptr = dst + (SCREEN_PITCH * y) + x;
  for (i =0; i < image_rows; i++)
    {
      dst_ptr1 = dst_ptr;
      src_ptr = trans_image + (i * image_width);
      if (r == NULL && fast_flag)
	{
	  if (runs[i][0].type == NTRANSP_TYPE &&
	      runs[i][1].type == RUN_END)
	    {
	      memcpy(dst_ptr,src_ptr,(image_rows - i) * SCREEN_PITCH);
	      return;
	    }
	}
      j = 0;
      while (runs[i][j].type != RUN_END)
	{
	  if (runs[i][j].type == NTRANSP_TYPE)
	    memcpy(dst_ptr1,src_ptr,runs[i][j].len);
	  src_ptr += runs[i][j].len;
	  dst_ptr1 += runs[i][j].len;
	  j++;
	}
      dst_ptr += SCREEN_PITCH;
    }
}
