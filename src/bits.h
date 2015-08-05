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
#ifndef BITS_H
#define BITS_H

#include "grafix.h"

void trans_blit(int x1, int y1, int x2, int y2, 
		unsigned char *src, unsigned char *);

void scale_pixels(unsigned char *source, int source_width, int source_height,
		  int dest_x, int dest_y, int dest_width,
		  int dest_height, Rect *bounds,
		  int trans_color = -1);

void expand_pixels( unsigned char *source, int source_width,
		    int source_height,
		    int dest_x, int dest_y, int dest_width,
		    int dest_height, Rect *bounds,
		    int trans_color = -1);

void compress_pixels(unsigned char *source, int source_width,
		     int source_height,
		     int dest_x, int dest_y, int dest_width,
		     int dest_height, Rect *bounds,
		     int trans_color = -1);

void copy_row(unsigned char *src, int src_w,
	      unsigned char *dest, int min_x, int max_x,
	      Rect *bounds,int trans_color = -1);

void expand_row(unsigned char *src, int src_w,
		unsigned char *dest, int min_x, int max_x,
		int cols, int r, Rect *bounds,
		int trans_color = -1);

void compress_row(unsigned char *src, int src_w,
		  unsigned char *dest, int min_x, int max_x,
		  int cols, int r, Rect *bounds,
		  int trans_color = -1);

void mask_scale_pixels(unsigned char *source, int source_width, int source_height,
		       int dest_x, int dest_y, int dest_width,
		       int dest_height,
		       int mask_min_y, int mask_max_y,
		       int *mask_ledge, int *mask_redge,
		       int trans_color = -1);

void mask_expand_pixels( unsigned char *source, int source_width,
			 int source_height,
			 int dest_x, int dest_y, int dest_width,
			 int dest_height,
			 int mask_min_y, int mask_max_y,
			 int *mask_ledge, int *mask_redge,
			 int trans_color = -1);

void mask_compress_pixels(unsigned char *source, int source_width,
			  int source_height,
			  int dest_x, int dest_y, int dest_width,
			  int dest_height, int mask_min_y, int mask_max_y,
			  int *mask_ledge, int *mask_redge,
			  int trans_color = -1);

int rset_ptr(int & cur_x, int & w, Rect *bounds);

#endif
