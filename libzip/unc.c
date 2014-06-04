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
#include <stdlib.h>
#include <memory.h>
#include "inflate.h"
#include "crc.h"

#ifndef MSDOS
//static char rcsid[] = "$Id: unc.c,v 1.1 1994/10/19 16:27:27 brian Exp $";
#endif

int qflag = 0;

#ifndef WIN31
uch slide[WSIZE];
#endif

typedef enum {
  IO_MEM_TO_MEM,
  IO_FILE_TO_MEM,
  IO_FILE_TO_FILE
} IO_MODEL;

static IO_MODEL io_type;

/* output memory buffer */
static unsigned char * dstptr;

/* input memory buffer */
static unsigned char * srcptr;

/* input file buffer */
static FILE * file_in;

/* output file buffer */
static FILE * file_out;

/* count's and sizes */
static long dstcount;
static long dstsize;
static long srccount;
static long srcsize;

/* FUNCTION PROTOTYPES (created by emacs, do not edit) */
long unc_mem_to_mem(unsigned char *dst, long dst_size,
		    unsigned char *src, long src_size);
long unc_file_to_mem(unsigned char *dst, long dst_size,
		     FILE *input, long from, long to);
long unc_file_to_file(FILE *output,
		      FILE *input, long from, long to);
static long do_uncompress(void);
int local_flush(int num_bytes);
int local_nextbyte(void);
static void unc_error(const char * message);
/* END FUNCTION PROTOTYPES (created by emacs, do not edit) */

long unc_mem_to_mem(unsigned char *dst, long dst_size,
		    unsigned char *src, long src_size)
{
  io_type = IO_MEM_TO_MEM;

  srcptr = src;
  srcsize = src_size;
  
  dstptr = dst;
  dstsize = dst_size;
  
  srccount = dstcount = 0;

  return do_uncompress();
}

long unc_file_to_mem(unsigned char *dst, long dst_size,
		     FILE *input, long from, long to)
{
  io_type = IO_FILE_TO_MEM;

  if(to < 0)
    srcsize = -1;
  else
    srcsize = to - from + 1;

  file_in = input;
  fseek(input,from,0L);

  dstptr = dst;
  dstsize = dst_size;
  
  srccount = dstcount = 0;

  return do_uncompress();
}

long unc_file_to_file(FILE *output,
		      FILE *input, long from, long to)
{
  io_type = IO_FILE_TO_FILE;

  if(to < 0)
    srcsize = -1;
  else
    srcsize = to - from + 1;

  fseek(input,from,0L);
  file_in = input;

  file_out = output;
  
  srccount = dstcount = 0;

  return do_uncompress();
}


static long do_uncompress(void)
{
  int i;
  int byte;
  unsigned short method;
  ulg mycrc, theircrc;

  method = 0;
  for(i=0;i<2;i++)
    method = (unsigned short) (method | ((unsigned int)local_nextbyte() << (i*8)));

  theircrc = 0;
  for(i=0;i<4;i++)
    theircrc = theircrc | ((ulg)local_nextbyte() << (i*8));

  switch(method)
    {
    case DEFLATED:
      updcrc(NULL,0);
      
      inflate();
      inflate_free();

      /* just return the crc I've accumulated so far */
      mycrc = updcrc(slide,0);
      break;

    case STORED:
      updcrc(NULL,0);

      i = 0;
      byte = local_nextbyte();
      while(byte != EOF)
	{
	  if(i >= WSIZE)
	    {
	      local_flush(i);
	      i = 0;
	    }
	  
	  slide[i++] = (uch)byte;
	  byte = local_nextbyte();
	}
      
      if(i > 0)
	{
	  local_flush(i);
	}

      mycrc = updcrc(slide,0);
      break;
      default: abort();
    }

  if(theircrc != mycrc)
    {
      dstcount = -1;
    }

  return(dstcount);
}

int local_flush(int num_bytes)
{
  updcrc(slide, (ulg) num_bytes);

  switch(io_type)
    {
    case IO_MEM_TO_MEM:
    case IO_FILE_TO_MEM:
      if(dstcount + num_bytes > dstsize)
	{
	  unc_error("uncompress, destination buffer too small");
	}

      memcpy(dstptr,(char *)slide, (unsigned) num_bytes);
      
      dstptr += num_bytes;
      dstcount += num_bytes;
      break;

    case IO_FILE_TO_FILE:
      if(fwrite(slide, (size_t) num_bytes, 1, file_out) != 1)
	{
	  unc_error("write to disk failed");
	}
      dstcount += num_bytes;
      break;
    }

  return 0;
}

int local_nextbyte(void)
{
  int byte;

  if(srccount == srcsize)
    {
      return EOF;
    }

  srccount++;

  switch(io_type)
    {
    case IO_MEM_TO_MEM:
      byte = (int)*srcptr++;
      break;
    case IO_FILE_TO_MEM:
    case IO_FILE_TO_FILE:
      byte = fgetc(file_in);
      break;
    }

  return byte;
}

__attribute__((noreturn)) static void unc_error(const char * message)
{
#ifdef WIN31
  /* do something for windows */
#else
  fprintf(stderr,"%s\n",message);
  exit(1);
#endif
}
