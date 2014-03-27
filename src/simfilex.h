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
#ifndef __simfilex_h
#define __simfilex_h

#include "simfile.h"

class simfileX
{
public:
enum { INT_INPUT, STR_INPUT, DICT_STR, DICT_STR_NF };
typedef struct dict_str
{
  char *str;
  int  val;
} dict;
static int readint(std::istream &is, char tok0 = 0, char tok1 = 0);
static int readstr(std::istream &is, char *bff, int maxlen, char tok0 = 0, 
		   char tok1 = 0);
static int readinput(std::istream &is, char *buff, int maxlen, int &i, 
	      char tok0 = 0,
	      char tok1 = 0);
static int readdictinput(std::istream &is, char *buff, int maxlen, int &i,
			dict *d, int d_cnt,
			char tok0 = 0,
			char tok1 = 0);
static int dictfind(char *buff, dict *d, int d_cnt, int &);
static int isstringinput(char *bff);
};

#endif
