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
 *           Sabre Fighter Plane Simulator       *
 * Version: 0.1                                  *
 * File   : bits.C                               *
 * Date   : March, 1997                          *
 * Author : Dan Hammer                           *
 *************************************************/
#include <string.h>
#include "vga_13.h"
#include "port_3d.h"
#include "rtkey.h"
#include "simerr.h"
#include "bits.h"

//#define LOW_VALUE -MAXINT
//#define HIGH_VALUE MAXINT

void trans_blit(int x1, int y1, int x2, int y2, 
		unsigned char *src, unsigned char *)
{
  int w = x2 - x1 + 1;
  int h = y2 - y1 + 1;
  scale_pixels(src,w,h,x1,y1,w,h,&Port_3D::screen,0);
}

int set_h(int &y_start, int &height, Rect *bounds);
int set_ptr(unsigned char *( & dest), int & cur_x, int & w,
	    Rect *bounds);
// Set y_start, height equal to that
//
int set_h(int &y_start, int &height, Rect *bounds)
{
  if (height <= 0)
    return (0);
  if (y_start > bounds->bottom())
    {
      height = 0;
      return (0);
    }

  int yd = y_start + height - 1;
  if (yd > bounds->bottom())
    height -= yd - bounds->bottom();
  return (height);
}

// Set the destination pointer, cur x position, and width
// to that clipped by bounds.
int set_ptr(unsigned char *( & dest), int & cur_x, int & w,
	    Rect *bounds)
{
  if (w <= 0)
    return (0);
  if (cur_x > bounds->right())
    return (0);
  if (cur_x < bounds->left())
    {
      dest += bounds->left() - cur_x;
      w -= bounds->left() - cur_x;
      cur_x = bounds->left();
    }
  int dx = cur_x + w - 1;
  if (dx > bounds->right())
    {
      dx = bounds->right();
      w = dx - cur_x + 1;
    }
  return(w);
}

// Like above, 'cept skip left clip check
int rset_ptr(int & cur_x, int & w, Rect *bounds)
{
  if (w <= 0)
    return (0);
  if (cur_x > bounds->right())
    return (0);
  int dx = cur_x + w - 1;
  if (dx > bounds->right())
    {
      dx = bounds->right();
      w = dx - cur_x + 1;
    }
  return(w);
}


void copy_row(unsigned char *src, int src_w,
	      unsigned char *dest,
	      int min_x, int max_x,
	      Rect *bounds, int trans_color)
{
  int cols_used = 0;
  int cur_x;
  int w;

  dest += min_x;
  cur_x = min_x;

  w = max_x - min_x + 1;
  if (set_ptr(dest,cur_x,w,bounds))
    {
      // If we were clipped on the left,
      // move the source map pointer
      if (cur_x > min_x)
	{
	  src += cur_x - min_x;
	  cols_used += cur_x - min_x;
	}
      if (trans_color == -1)
	memcpy(dest,src,static_cast<size_t>(w));
      else
	{
	  while (w-- && (cols_used <= src_w))
	    {
	      unsigned char c = *src++;
	      if (c != static_cast<unsigned char>(trans_color))
		*dest++ = c;
	      else
		dest++;
	      cur_x++;
	      cols_used++;
	    }
	}
    }
}

void expand_row(unsigned char *src, int src_w,
		unsigned char *dest, int min_x, int max_x,
		int cols, int r, Rect *bounds, int trans_color)
{
  R_KEY_BEGIN(72)
    int cur_x;
  int q = src_w;
  int d = 0;
  int cols_used = 0;
  int w,w2;
  unsigned char *dst;

  if (cols <= 0)
    return;
  if (max_x < bounds->left())
    return;
  if (max_x > bounds->right())
    max_x = bounds->right();
  if (min_x + (max_x - min_x) < bounds->left())
    return;

  cur_x = min_x;
  dest += min_x;

  while ((cur_x <= max_x) && (cols_used < src_w))
    {
      w = cols;

      d += r;
      if (d >= q)
	{
	  w++;
	  d = d - q;
	}

      w2 = max_x - cur_x + 1;
      if (w > w2)
	w = w2;

      if (rset_ptr(cur_x,w,bounds))
	{
	  dst = dest;
	  int putw = w;
	  int tmp = bounds->left() - cur_x;
	  if (tmp <= 0)
	    {
	      if (trans_color == -1)
		memset(dst,*src,static_cast<size_t>(putw));
	      else if (*src != static_cast<unsigned char>(trans_color))
		memset(dst,*src,static_cast<size_t>(putw));
	    }
	  else
	    {
	      int cx = cur_x;
	      for (int i=0;i<w;i++)
		{
		  if (cx >= bounds->left())
		    {
		      if (trans_color == -1)
			*dst = *src;
		      else if (*src != static_cast<unsigned char>(trans_color))
			*dst = *src;
		    }
		  dst++;
		  cx++;
		}
	    }
	}

      dest += w;
      cur_x += w;
      src++;
      cols_used++;
    }
  R_KEY_END
    }

void compress_row(unsigned char *src, int src_w,
		  unsigned char *dest, int min_x, int max_x,
		  int cols, int r, Rect *bounds, int trans_color)
{
  R_KEY_BEGIN(74)
    int cur_x;
  int cols_used = 0;
  int q = (max_x - min_x) + 1;
  int d = 0;
  int w;

  if (cols <= 0)
    return;
  if (max_x < bounds->left())
    return;
  if (max_x > bounds->right())
    max_x = bounds->right();
  if (min_x + (max_x - min_x) < bounds->left())
    return;

  cur_x = min_x;
  dest += min_x;

  while ((cur_x <= max_x) && (cols_used <= src_w))
    {
      if (cur_x >= bounds->left() && cur_x <= bounds->right())
	{
	  if (trans_color == -1)
	    *dest++ = *src;
	  else if (*src != static_cast<unsigned char>(trans_color))
	    *dest++ = *src;
	  else
	    dest++;
	}
      else
	dest++;
      cur_x++;
      w = cols;
      d += r;
      if (d >= q)
	{
	  w++;
	  d = d - q;
	}
      cols_used += w;
      src += w;
    }
  R_KEY_END
    }

void expand_pixels(unsigned char *source, int source_width, int source_height,
		   int dest_x, int dest_y, int dest_width,
		   int dest_height, Rect *bounds, int trans_color)
{
  R_KEY_BEGIN(75)
    int cols;
  int rows;
  int rows_put;
  int rows_used;
  int cur_y;
  int q;
  int d;
  int r;
  int h;
  unsigned char *bptr;
  unsigned char *buffer_ptr;
  
  MYCHECK(source_height > 0);
  MYCHECK(source_width > 0);

  if (dest_height <= 0)
    return;
  if (dest_y > bounds->bottom())
    return;


  rows = dest_height / source_height;
  // Compute vertical remainder
  r = dest_height - (rows * source_height);
  q = source_height;
  d = 0;
  // Clip to bottom
  if (dest_y + (dest_height - 1) > bounds->bottom())
    dest_height = bounds->bottom() - dest_y + 1;
  if (dest_y + dest_height < bounds->top())
    return;

  // Compute horizontal multiplier
  cols = dest_width / source_width;
  // Compute horizontal remainder
  int cr = dest_width - (cols * source_width);


  rows_put = 0;
  rows_used = 0;
  cur_y = dest_y;

  buffer_ptr = lock_xbuff();
  if (!buffer_ptr)
	return;
  bptr = buffer_ptr + cur_y * SCREEN_PITCH;

  while ((rows_put < dest_height) && (rows_used < source_height))
    {
      h = rows;
      d += r;
      if (d >= q)
	{
	  h++;
	  d = d - q;
	}
      for (int i=0;i<h;i++)
	{
	  if (cur_y >= bounds->top() && cur_y <= bounds->bottom())
	    expand_row(source,
		       source_width,
		       bptr,
		       dest_x,
		       dest_x + dest_width - 1,
		       cols,
		       cr,
		       bounds,
		       trans_color);
	  rows_put++;
	  cur_y++;
	  bptr += SCREEN_PITCH;
	  if (rows_put > dest_height)
	    break;
	}
      source += source_width;
      rows_used++;
    }

  free_xbuff();
  R_KEY_END
    }

void compress_pixels(unsigned char *source, int source_width, int source_height,
		     int dest_x, int dest_y, int dest_width,
		     int dest_height, Rect *bounds, int trans_color)
{
  R_KEY_BEGIN(76)
    int cols;
  int cr;
  int rows;
  int rows_put;
  int rows_used;
  int cur_y;
  int r,q,d,h;
  unsigned char *bptr;
  unsigned char *buffer_ptr;

  MYCHECK(source_height > 0);
  MYCHECK(source_width > 0);

  if (dest_height <= 0)
    return;
  if (dest_y > bounds->bottom())
    return;

  rows = source_height / dest_height;
  // Compute vertical remainder
  r = source_height - (rows * dest_height);
  q = dest_height;
  d = 0;

  // Clip to bottom
  if (dest_y + (dest_height - 1) > bounds->bottom())
    dest_height = bounds->bottom() - dest_y + 1;
  if (dest_y + dest_height < bounds->top())
    return;

  // Compute horizontal multiplier & remainder
  cols = source_width / dest_width;
  cr = source_width - (cols * dest_width);

  rows_put = 0;
  rows_used = 0;
  cur_y = dest_y;

  buffer_ptr = lock_xbuff();
  if (!buffer_ptr)
	return;

  bptr = buffer_ptr + cur_y * SCREEN_PITCH;

  while ((rows_put < dest_height) && (rows_used < source_height))
    {
      if (cur_y >= bounds->top() && cur_y <= bounds->bottom())
	compress_row(source,
		     source_width,
		     bptr,
		     dest_x,
		     dest_x + dest_width - 1,
		     cols,
		     cr,
		     bounds,
		     trans_color);
      bptr += SCREEN_PITCH;
      rows_put++;
      cur_y++;
      h = rows;
      d += r;
      if (d >= q)
	{
	  h++;
	  d = d - q;
	}
      rows_used += h;
      source += (source_width * h);
      if (rows_used > source_height)
	return;
    }
  R_KEY_END
	free_xbuff();
    }

void scale_pixels(unsigned char *source, int source_width, int source_height,
		  int dest_x, int dest_y, int dest_width,
		  int dest_height, Rect *bounds,int trans_color)
{
  R_KEY_BEGIN(88)
    if (source_width < dest_width)
      expand_pixels(source,source_width,source_height,dest_x,dest_y,
		    dest_width,dest_height,bounds,trans_color);
    else if (source_width > dest_width)
      compress_pixels(source,source_width,source_height,dest_x,dest_y,
		      dest_width,dest_height,bounds,trans_color);
    else
      {
	unsigned char *buffer_ptr;
   buffer_ptr = lock_xbuff();
	  if (!buffer_ptr)
		return;

	int y = dest_y;
	if (set_h(dest_y,source_height,bounds))
	  {
	    if (dest_y > y)
	      source += source_width * (dest_y - y);
	    unsigned char *bptr = buffer_ptr + SCREEN_PITCH * dest_y;
	    for (int i=0;i<source_height;i++)
	      {
		if (dest_y >= bounds->top())
		  {
		    copy_row(source,
			     source_width,
			     bptr,
			     dest_x,
			     dest_x + dest_width - 1,
			     bounds,
			     trans_color);
		  }
		source += source_width;
		dest_y++;
		bptr += SCREEN_PITCH;
	      }
	  }
      }
	free_xbuff();
  R_KEY_END
    }

void mask_expand_pixels(unsigned char *source, int source_width,
			int source_height,
			int dest_x, int dest_y, int dest_width,
			int dest_height,
			int mask_min_y, int mask_max_y,
			int *mask_ledge, int *mask_redge,
			int trans_color)
{
  R_KEY_BEGIN(375)
    int cols;
  int rows;
  int rows_put;
  int rows_used;
  int cur_y;
  int q;
  int d;
  int r;
  int h;
  unsigned char *bptr;
  unsigned char *buffer_ptr;

  Rect bounds;

  MYCHECK(source_height > 0);
  MYCHECK(source_width > 0);

  if (dest_height <= 0)
    return;
  if (dest_y > mask_max_y)
    return;

  rows = dest_height / source_height;
  // Compute vertical remainder
  r = dest_height - (rows * source_height);
  q = source_height;
  d = 0;
  // Clip to bottom
  if (dest_y + (dest_height - 1) > mask_max_y)
    dest_height = mask_max_y - dest_y + 1;
  if (dest_y + dest_height < mask_min_y)
    return;

  bounds.topLeft.y = mask_min_y;
  bounds.botRight.y = mask_max_y;

  // Compute horizontal multiplier
  cols = dest_width / source_width;
  // Compute horizontal remainder
  int cr = dest_width - (cols * source_width);


  rows_put = 0;
  rows_used = 0;
  cur_y = dest_y;

  buffer_ptr = lock_xbuff();
  if (!buffer_ptr)
	return;

  bptr = buffer_ptr + cur_y * SCREEN_PITCH;

  while ((rows_put < dest_height) && (rows_used < source_height))
    {
      h = rows;
      d += r;
      if (d >= q)
	{
	  h++;
	  d = d - q;
	}
      for (int i=0;i<h;i++)
	{
	  if (cur_y >= mask_min_y && cur_y <= mask_max_y)
	    {
	      bounds.topLeft.x = mask_ledge[cur_y];
	      bounds.botRight.x = mask_redge[cur_y];
	      expand_row(source,
			 source_width,
			 bptr,
			 dest_x,
			 dest_x + dest_width - 1,
			 cols,
			 cr,
			 &bounds,
			 trans_color);
	    }
	  rows_put++;
	  cur_y++;
	  bptr += SCREEN_PITCH;
	  if (rows_put > dest_height)
	    break;
	}
      source += source_width;
      rows_used++;
    }
  free_xbuff();
  R_KEY_END
    }


void mask_compress_pixels(unsigned char *source, int source_width,
			  int source_height,
			  int dest_x, int dest_y, int dest_width,
			  int dest_height,
			  int mask_min_y,
			  int mask_max_y,
			  int *mask_ledge,
			  int *mask_redge,
			  int trans_color)
{
  R_KEY_BEGIN(376)
    int cols;
  int cr;
  int rows;
  int rows_put;
  int rows_used;
  int cur_y;
  int r,q,d,h;
  unsigned char *bptr;
  unsigned char *buffer_ptr;
  Rect bounds;

  MYCHECK(source_height > 0);
  MYCHECK(source_width > 0);

  if (dest_height <= 0)
    return;
  if (dest_y > mask_max_y)
    return;

  bounds.topLeft.y = mask_min_y;
  bounds.botRight.y = mask_max_y;

  rows = source_height / dest_height;
  // Compute vertical remainder
  r = source_height - (rows * dest_height);
  q = dest_height;
  d = 0;

  // Clip to bottom
  if (dest_y + (dest_height - 1) > mask_max_y)
    dest_height = mask_max_y - dest_y + 1;
  if (dest_y + dest_height < mask_min_y)
    return;

  buffer_ptr = lock_xbuff();
  if (!buffer_ptr)
	return;

  // Compute horizontal multiplier & remainder
  cols = source_width / dest_width;
  cr = source_width - (cols * dest_width);

  rows_put = 0;
  rows_used = 0;
  cur_y = dest_y;
  bptr = buffer_ptr + cur_y * SCREEN_PITCH;

  while ((rows_put < dest_height) && (rows_used < source_height))
    {
      if (cur_y >= mask_min_y && cur_y <= mask_max_y)
	{
	  bounds.topLeft.x = mask_ledge[cur_y];
	  bounds.botRight.x = mask_redge[cur_y];
	  compress_row(source,
		       source_width,
		       bptr,
		       dest_x,
		       dest_x + dest_width - 1,
		       cols,
		       cr,
		       &bounds,
		       trans_color);
	}
      bptr += SCREEN_PITCH;
      rows_put++;
      cur_y++;
      h = rows;
      d += r;
      if (d >= q)
	{
	  h++;
	  d = d - q;
	}
      rows_used += h;
      source += (source_width * h);
      if (rows_used > source_height)
	return;
    }
  free_xbuff();
  R_KEY_END
    }

void mask_scale_pixels(unsigned char *source, int source_width, int source_height,
		       int dest_x, int dest_y, int dest_width,
		       int dest_height,
		       int mask_min_y, int mask_max_y,
		       int *mask_ledge, int *mask_redge,
		       int trans_color)
{
  R_KEY_BEGIN(388)
    if (source_width < dest_width)
      mask_expand_pixels(source,source_width,source_height,dest_x,dest_y,
			 dest_width,dest_height,mask_min_y,mask_max_y,
			 mask_ledge,mask_redge,trans_color);
    else if (source_width > dest_width)
      mask_compress_pixels(source,source_width,source_height,dest_x,dest_y,
			   dest_width,dest_height,
			   mask_min_y,mask_max_y,
			   mask_ledge,mask_redge,trans_color);
    else
      {
	unsigned char *buffer_ptr;
	int y = dest_y;
	Rect bounds;
	bounds.topLeft.y = mask_min_y;
	bounds.botRight.y = mask_max_y;
	  buffer_ptr = lock_xbuff();
	if (!buffer_ptr)
		return;

	if (set_h(dest_y,source_height,&bounds))
	  {
	    if (dest_y > y)
	      source += source_width * (dest_y - y);
	    unsigned char *bptr = buffer_ptr + SCREEN_PITCH * dest_y;
	    for (int i=0;i<source_height;i++)
	      {
		if (dest_y >= mask_min_y)
		  {
		    bounds.topLeft.x = mask_ledge[dest_y];
		    bounds.botRight.x = mask_redge[dest_y];
		    copy_row(source,
			     source_width,
			     bptr,
			     dest_x,
			     dest_x + dest_width - 1,
			     &bounds,
			     trans_color);
		  }
		source += source_width;
		dest_y++;
		bptr += SCREEN_PITCH;
	      }
	  }
	free_xbuff();
      }
  R_KEY_END
    }
