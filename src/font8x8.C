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
/*************************************************************
 * font8x8.cpp                                               *
 *************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "simerr.h"
#include "grafix.h"
#include "vga_13.h"
#ifndef SABREWIN
#include "fontdev.h"
#endif
#include "font8x8.h"

extern char *build_libpath(char *);


void SimFont::put_string(char *str, int x, int y, int color)
{
  char c;
  while ((c = *str++) != '\0')
    {
      put_char(c,x,y,color,option);
      x += put_width;
    }
}

void SimFont::font_sprintf(int x, int y, int color, int opt, char *format, ...)
{
  char buff[120];
  va_list ap;

  set_option(opt);
  va_start(ap,format);
  vsprintf(buff,format,ap);
  va_end(ap);
  put_string(buff,x,y,color);
}

Font8x8::Font8x8(char *s)
  :title(s)
{
  char buf[80];
  FILE *file;
  int a;
  char *path;

  width = height = 8;
  put_width = put_height = 8;

  sprintf(buf,"%s.fnt",title);
  path = build_libpath(buf);
  if ((file=fopen(path,"rb"))==NULL)
    {
      fprintf(stderr,"Font8x8 : Unable to open %s for read\n",path);
      exit(1);
    }

  for (a=0;a<256;a++)
    fread(font[a],8,1,file);

  fclose(file);
}

void Font8x8::put_char(unsigned char c, int x, int y, int color,
		       int dim)
{
  unsigned char *buffer_ptr;
  buffer_ptr = lock_xbuff();
  if (!buffer_ptr)
	return;

  dim = 8;
  byte *byt_ptr = font[(unsigned int)c];
  unsigned char *bf = (unsigned char *) buffer_ptr + (y * SCREEN_PITCH);
  for (int i=0;i<dim;i++)
    {
      unsigned char *bf1 = bf + x;
      unsigned char b = *byt_ptr;
      for (int j=0;j<dim;j++)
	{
	  if (!is_visible(cliprect,x+j,y+i))
	    goto label1;
	  switch (option)
	    {
	    case NORMAL:
	    default:
	      if (b & 1)
		*bf1 = (char) color;
	      break;

	    case INVERSE:
	      if (!(b & 1))
		*bf1 = (char ) color;
	      break;

	    }
	label1:
	  bf1++;
	  b >>= 1;
	}
      bf += SCREEN_PITCH;
      byt_ptr++;
    }
  free_xbuff();
}

#ifndef SABREWIN
ConsoleFont::ConsoleFont(char *fname)
#else
ConsoleFont::ConsoleFont(char *)
#endif
{
#ifndef SABREWIN
  char *path;
  fdev = new fontdev;
  MYCHECK(fdev != NULL);
  path = build_libpath(fname);
  fdev->load(path);
  width = fdev->getdimx();
  height = fdev->getdimy();
  put_width = width;
  put_height = height;
#endif
}

ConsoleFont::~ConsoleFont()
{
#ifndef SABREWIN
  if (fdev != NULL)
    delete fdev;
#endif
}

/* from hello.C prtxy() */
#ifndef SABREWIN
void ConsoleFont::put_char(unsigned char c, int x, int y, int color,
			   int )
#else
void ConsoleFont::put_char(unsigned char , int , int , int , int )
#endif
{
#ifndef SABREWIN
  unsigned char *dest = lock_xbuff() + x + y * SCREEN_PITCH;
  unsigned char *src = (unsigned char *) fdev->getfbp(c);
  int i,j;
  int xx;
  for (j=0; j<height; j++, dest += (SCREEN_WIDTH - width) )
    {
      xx = x;
      for (i=0; i<width;i++,src++,dest++)
	{
	  if (*src && is_visible(cliprect,xx,y))
	    *dest = color & *src;
	  xx++;
	}
      y++;
    }
#endif
}
