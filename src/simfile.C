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
#include <iostream.h>
#include <fstream.h>
#include <math.h>
#include "vmath.h"
#include "defs.h"
#include "simerr.h"
#include "colorspc.h"
#include "txtrmap.h"
#include "simsnd.h"
#include "simfile.h"

extern char *lib_path;
char *current_path = "";
char *file_context = "";
FILE *simlog = stdout;

char *build_libpath(char *filename)
{
  static char tmppath[200];
  int l = strlen(filename);
  int l2 = strlen(lib_path);
  if ((l + l2 + 2) > 200)
    error_jump("Can't build path, length too long: %s %s",lib_path,filename);
#ifdef SABREWIN
  sprintf(tmppath,"%s\\%s",lib_path,filename);
#else
  sprintf(tmppath,"%s/%s",lib_path,filename);
#endif
  l = strlen(tmppath);
  if (tmppath[l-1] == '\n' || tmppath[l-1]=='\r')
    tmppath[l-1] = 0;
  return(tmppath);
}

void read_texture_file(char *path)
{
  ifstream is;
#ifdef SABREWIN
  if (map_man)
    delete map_man;
  map_man = new TextrMap_Manager();
#endif

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

void read_palette_file(char *path)
{
  color_spec cs;
  ifstream is;
  current_path = path;
  if (open_is(is,path))
    {
      while (is)
	is >> cs;
      is.close();
    }
}

void read_sound_file(char *path)
{
  ifstream is;
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

int get_line(istream &is, char *buff, int size)
{
  int l;
  while (is)
    {
      is.getline(buff,size);
      if (is)
	{
	  if (buff[0] != '*' && buff[0] != '\x0')
	    {
	      l = strlen(buff);
	      if (buff[l-1] == '\n')
		buff[l-1] = 0;
	      l = strlen(buff);
	      if (buff[l-1] == '\r')
		{
		  buff[l-1] = 0;
		  sim_printf("warning: %s contains DOS cr/lf\n");
		}
	      return (strlen(buff));
	    }
	}
    }
  return 0;
}

int open_is(ifstream &is, char *path)
{
  char *lpath = build_libpath(path);
  current_path = lpath;
  sim_printf("Opening \"%s\" for reading\n",
	     lpath);
  is.open(lpath);
  if (!is.good())
    error_jump("Unable to Open \"%s\" For Reading\n",lpath);
  return (is.good());
}


int open_os(ofstream &os, char *path)
{
  os.open(path);
  if (!os.good())
    error_jump("Unable to Open %s For Writing\n",path);
  return (os.good());
}

int open_libos(ofstream &os, char *path)
{
  char *lpath = build_libpath(path);
  os.open(lpath);
  if (!os.good())
    error_jump("Unable to Open %s For Writing\n",lpath);
  current_path = lpath;
  return (os.good());
}

int read_int(istream &is)
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




