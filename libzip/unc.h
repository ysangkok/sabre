/*
    Copyright (C) 1997	COBITE and Dan Hammer

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
#ifndef UNC_H_RCSID
#define UNC_H_RCSID "$Id: unc.h,v 1.1 1994/10/19 16:27:27 brian Exp $"
#ifdef __cplusplus
extern "C" 
{
#endif
  long unc_mem_to_mem(unsigned char *dst, long dst_size,
			   unsigned char *src, long src_size);
  long unc_file_to_mem(unsigned char *dst, long dst_size,
			    FILE *input, long from, long to);
  long unc_file_to_file(FILE *output,
			     FILE *input, long from, long to);
  int local_flush(int num_bytes);
  int local_nextbyte(void);
#ifdef __cplusplus
}
#endif
#endif /* UNC_H_RCSID */
