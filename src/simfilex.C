/*
    SABRE Fighter Plane Simulator 
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "simfile.h"
#include "simfilex.h"

int simfileX::readint(std::istream &is, char tok0, char tok1)
{
  int result;
  char c = 0;

  if (tok0)
    {
      READ_TOKI(tok0,is,c)
	}
  is >> result;
  if (tok1)
    {
      READ_TOK(tok1,is,c)
	}
  return (result);
}

int simfileX::readstr(std::istream &is, char *buff, unsigned int maxlen, char tok0, char tok1)
{
  char c0,c1=0;
  int l;
  char lbuff[BUFSIZ];
  char *cptr;

  if (tok0)
    {
      READ_TOKI(tok0,is,c0);
    }
  is >> lbuff;

  if (tok1)
    {
      cptr = lbuff + strlen(lbuff) - 1;
      c1 = *cptr;
      if (c1 == tok1)
	*cptr = 0;
    }
  if (strlen(lbuff) > static_cast<unsigned>(maxlen - 1))
    lbuff[maxlen-1] = 0;
  strcpy(buff,lbuff);
  l = static_cast<int>(strlen(buff));
  if (buff[l-1] == '\n')
    buff[l-1] = 0;
  if (buff[l-1] == '\r')
    buff[l-1] = 0;
  if (tok1)
    {
      if (c1 != tok1)
	{
	  READ_TOK(tok1,is,c1)
	    }
    }
  l = static_cast<int>(strlen(buff));
  return (l);
}

bool simfileX::isstringinput(char *buff)
{
  char c;
  while ((c = *buff++) != '\0')
    if (!isdigit(c))
      return(1);
  return (0);
}

int simfileX::readinput(std::istream &is, char *buff, unsigned int maxlen, int &i, char tok0, char tok1)
{
  int result = INT_INPUT;

  readstr(is,buff,maxlen,tok0,tok1);
  if (isstringinput(buff))
    result = STR_INPUT;
  else
    i = atoi(buff);
  return (result);
}

int simfileX::readdictinput(std::istream &is, char *buff, unsigned int maxlen, int &val, dict *d, int d_cnt, char tok0, char tok1)
{
  if (readinput(is,buff,maxlen,val,tok0,tok1) == INT_INPUT)
    return(INT_INPUT);
  else 
    return(dictfind(buff,d,d_cnt,val));
}

int simfileX::dictfind(char *buff, dict *d, int d_cnt, int &val)
{
  for (int i=0;i<d_cnt;i++)
    if (!strcmp(buff,d[i].str))
      {
	val = d[i].val;
	return(DICT_STR);
      }
  return (DICT_STR_NF);
}



