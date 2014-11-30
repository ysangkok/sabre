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
 * File   : bits.h                               *
 * Date   : March, 1997                          *
 * Author : Dan Hammer                           *
 *************************************************/
#ifndef __bits_h
#define __bits_h

#include "grafix.h"

void trans_blit(size_t x1, size_t y1, size_t x2, size_t y2, 
		unsigned char *src, unsigned char *);

void scale_pixels(unsigned char *source, size_t source_width, size_t source_height,
		  size_t dest_x, size_t dest_y, size_t dest_width,
		  size_t dest_height, Rect *bounds,
		  int trans_color = -1);

void expand_pixels( unsigned char *source, size_t source_width,
		    size_t source_height,
		    size_t dest_x, size_t dest_y, size_t dest_width,
		    size_t dest_height, Rect *bounds,
		    int trans_color = -1);

void compress_pixels(unsigned char *source, size_t source_width,
		     size_t source_height,
		     size_t dest_x, size_t dest_y, size_t dest_width,
		     size_t dest_height, Rect *bounds,
		     int trans_color = -1);

void copy_row(unsigned char *src, size_t src_w,
	      unsigned char *dest, size_t min_x, size_t max_x,
	      Rect *bounds,int trans_color = -1);

void expand_row(unsigned char *src, size_t src_w,
		unsigned char *dest, size_t min_x, size_t max_x,
		size_t cols, size_t r, Rect *bounds,
		int trans_color = -1);

void compress_row(unsigned char *src, size_t src_w,
		  unsigned char *dest, size_t min_x, size_t max_x,
		  size_t cols, size_t r, Rect *bounds,
		  int trans_color = -1);

void mask_scale_pixels(unsigned char *source, size_t source_width, size_t source_height,
		       size_t dest_x, size_t dest_y, size_t dest_width,
		       size_t dest_height,
		       size_t mask_min_y, size_t mask_max_y,
		       size_t *mask_ledge, size_t *mask_redge,
		       int trans_color = -1);

void mask_expand_pixels( unsigned char *source, size_t source_width,
			 size_t source_height,
			 size_t dest_x, size_t dest_y, size_t dest_width,
			 size_t dest_height,
			 size_t mask_min_y, size_t mask_max_y,
			 size_t *mask_ledge, size_t *mask_redge,
			 int trans_color = -1);

void mask_compress_pixels(unsigned char *source, size_t source_width,
			  size_t source_height,
			  size_t dest_x, size_t dest_y, size_t dest_width,
			  size_t dest_height, size_t mask_min_y, size_t mask_max_y,
			  size_t *mask_ledge, size_t *mask_redge,
			  int trans_color = -1);

size_t rset_ptr(size_t & cur_x, size_t & w, Rect *bounds);

#endif
