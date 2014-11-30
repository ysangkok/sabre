/*
    Sabre Fighter Plane Simulator 
    Copyright (c) 1997 Dan Hammer
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
 * File   : simfile.C                            *
 * Date   : March, 1997                          *
 * Author : Dan Hammer                           *
 *************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include "vmath.h"
#include "defs.h"
#include "simerr.h"
#include "colorspc.h"
#include "txtrmap.h"
#include "simsnd.h"
#include "simfile.h"

extern const char *lib_path;
const char *current_path = "";
const char *file_context = "";
FILE *simlog = stdout;

const char *build_libpath(const char *filename)
{
  static char tmppath[200];
  int l = (int) strlen(filename);
  int l2 = (int) strlen(lib_path);
  if ((l + l2 + 2) > 200)
    error_jump("Can't build path, length too long: %s %s",lib_path,filename);
  sprintf(tmppath,"%s/%s",lib_path,filename);
  l = (int) strlen(tmppath);
  if (tmppath[l-1] == '\n' || tmppath[l-1]=='\r')
    tmppath[l-1] = 0;
  return(tmppath);
}

void read_texture_file(const char *path)
{
  std::ifstream is;

  if (map_man != NULL)
    {
      if (open_is(is,path))
	{
	  current_path = path;
	  is >> *map_man;
	  is.close();
	}
    }
}

void read_palette_file(const char *path)
{
  color_spec cs;
  std::ifstream is;
  current_path = path;
  if (open_is(is,path))
    {
      while (is)
	is >> cs;
      is.close();
    }
}

void read_sound_file(const char *path)
{
  std::ifstream is;
  int		nsounds;
  int		gotline;
  char		buff[BUFSIZ];
  char		*cptr;

  current_path = path;
  if (open_is(is,path))
    {
      gotline = get_line(is,buff,sizeof(buff));
      if (gotline)
	{
	  nsounds = atoi(buff);
	  for (int i=0;i<nsounds;i++)
	    {
	      if (!is)
		break;
	      gotline = get_line(is,buff,sizeof(buff));
	      if (gotline)
		{
		  cptr = strchr(buff,',');
		  if (cptr != NULL)
		    {
		      *cptr++ = 0;
		      sound_load_wav(build_libpath(buff),cptr);
		    }
		}
	    }
	}
    }
}

int get_line(std::istream &is, char *buff, int size)
{
  int l;
  while (is)
    {
      is.getline(buff,size);
      if (is)
	{
	  if (buff[0] != '*' && buff[0] != '\x0')
	    {
	      l = (int) strlen(buff);
	      if (buff[l-1] == '\n')
		buff[l-1] = 0;
	      l = (int) strlen(buff);
	      if (buff[l-1] == '\r')
		{
		  buff[l-1] = 0;
		  sim_printf("warning: %s contains DOS cr/lf\n");
		}
	      return (int) (strlen(buff));
	    }
	}
    }
  return 0;
}

int open_is(std::ifstream &is, const char *path)
{
  const char *lpath = build_libpath(path);
  current_path = lpath;
  sim_printf("Opening \"%s\" for reading\n",
	     lpath);
  is.open(lpath);
  if (!is.good())
    error_jump("Unable to Open \"%s\" For Reading\n",lpath);
  return (is.good());
}


int open_os(std::ofstream &os, const char *path)
{
  os.open(path);
  if (!os.good())
    error_jump("Unable to Open %s For Writing\n",path);
  return (os.good());
}

int open_libos(std::ofstream &os, const char *path)
{
  const char *lpath = build_libpath(path);
  os.open(lpath);
  if (!os.good())
    error_jump("Unable to Open %s For Writing\n",lpath);
  current_path = lpath;
  return (os.good());
}

unsigned int read_uint(std::istream &is)
{
  char c;
  unsigned int result = 0;
  READ_TOKI('(',is,c);
  is >> result;
  READ_TOK(')',is,c);
  return (result);
}

int read_int(std::istream &is)
{
  char c;
  int result = 0;
  READ_TOKI('(',is,c);
  is >> result;
  READ_TOK(')',is,c);
  return (result);
}

int __cdecl sim_printf(const char *format, ...)
{
  int r;
  va_list ap;
  va_start(ap,format);
  r = vfprintf(simlog,format,ap);
  va_end(ap);
  fflush(simlog);
  return (r);
}
