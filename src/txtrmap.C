/*
    Sabre Flight Simulator 
    Copyright (C) 1997	Dan Hammer

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
 *           Sabre Flight Simulator              *
 * File   : txtrmap.C                            *
 * Date   : March, 1997                          *
 * Author : Dan Hammer                           *
 *          08/20/97                             *
 *          Antti Barck                          *
 *          Use unsigned int when read/writing   *
 *          compressed files to make compatible  *
 *          with 64bit machines                  *
 *************************************************/
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <limits.h>
#include <values.h>
#include <string.h>
#include <endian.h>
#include "swap.h"
#include "defs.h"
#include "grafix.h"
#include "vga_13.h"
#include "vmath.h"
#include "simerr.h"
#include "simfile.h"
#include "colormap.h"
#include "txtrmap.h"
#include "comp.h"
#include "unc.h"

TextrMap nullmap;
TextrMap_Manager *map_man = NULL;
int TextrMap_Manager::reserved = 64;

void TextrMap::copy(TextrMap &tm)
{
	bytes = tm.bytes;
	size = tm.size;
	cbytes = tm.cbytes;
	csize = tm.csize;
	map_w = tm.map_w;
	map_h = tm.map_h;
	trans_colr = tm.trans_colr;
	delf = 1;
	tm.delf = 0;
	flags = tm.flags;
	memcpy(id,tm.id,sizeof(id));
	if (tm.cmaps != NULL)
	{
		cmaps = new ColorMap[N_COLORS];
		MYCHECK(cmaps != NULL);
		for (int i=0;i<N_COLORS;i++)
			cmaps[i] = tm.cmaps[i];
	}
	if (tm.mc != NULL && tm.nm > 0)
	{
		nm = tm.nm;
		mc = new int[nm];
		MYCHECK(mc != NULL);
		for (int i=0;i<nm;i++)
			mc[i] = tm.mc[i];
	}
}

void TextrMap::read(std::istream &is)
{
char *cptr;
int ii;
char c = ' ';

	memset(id,' ',sizeof(id));
	READ_TOK('{',is,c)
	is >> id;
	cptr = id + strlen(id) - 1;
	c = *cptr;
	if (c == '*' || c == '@')
	{
		is >> trans_colr;
		*cptr = '\0';
		if (c == '@')
			read_compressed(0);
		else
			read_compressed(1);
	}
	else
	{
		is >>  map_w >> map_h >> trans_colr;
		id[31] = 0;
		int nbytes = map_w * map_h;
		if (bytes != NULL)
			delete [] bytes;
		bytes = new unsigned char[nbytes];
		MYCHECK(bytes != NULL);
		delf = 1;
		for (int i=0;i<nbytes;i++)
		{
			is >> ii;
			bytes[i] = static_cast<unsigned char>(ii);
		}
	}

	is >> c;
	if (c == '*')
	{
		is >> nm;
		cmaps = new ColorMap[N_COLORS];
		MYCHECK(cmaps != NULL);
		mc = new int[nm];
		MYCHECK(mc != NULL);
		ColorMap tmp;
		for (int i=0;i<nm;i++)
		{
			is >> tmp;
			mc[i] = tmp.id_color;
			cmaps[tmp.id_color] = tmp;
		}
	}
	READ_TOK('}',is,c);
}

void TextrMap::write(std::ostream &os)
{
  os << "{\n" << id << "*\n" << trans_colr << "\n";
  compress();
  os << "}\n";
}

std::istream & operator >>(std::istream &is, TextrMap &tm)
{
  tm.read(is);
  return (is);
}

std::ostream & operator <<(std::ostream &os, TextrMap &tm);
std::ostream & operator <<(std::ostream &os, TextrMap &tm)
{
  tm.write(os);
  return (os);
}

void TextrMap::compress()
{
unsigned int  n;
uint32_t tgtsize;
uint32_t srcsize;
char     *tgt;
char     *fname;
std::ofstream os;

	if (bytes != NULL)
	{
		tgtsize = static_cast<uint32_t>(map_w * map_h);
		srcsize = tgtsize;
		tgt = new char[tgtsize];
		if ((n = memcompress(tgt,tgtsize,reinterpret_cast<char *>(bytes),srcsize)) != 0)
		{
			fname = new char[strlen(id) + 10];
			sprintf(fname,"tzp/%s.tzp",id);
			if (open_libos(os,fname))
			{
				uint32_t tmp;
				tmp = htole32(static_cast<uint32_t>(map_w));
				os.write(reinterpret_cast<char *>(&tmp),sizeof(tmp));
				tmp = htole32(static_cast<uint32_t>(map_h));
				os.write(reinterpret_cast<char *>(&tmp),sizeof(tmp));
				tmp = htole32(static_cast<uint32_t>(n));
				os.write(reinterpret_cast<char *>(&tmp),sizeof(tmp));
				os.write(tgt,n);
			}
			delete [] fname;
		}
		else
			printf("compression error: %du\n",n);
		delete [] tgt;
	}
}

void TextrMap::read_compressed(int cflg)
{
uint32_t          n;
FILE              *f;
int               nread;
const char              *path;
const char              *open_params;

{
char              *fname;
	fname = new char[strlen(id) + 10];
	sprintf(fname,"tzp/%s.tzp",id);
	open_params = "r";
	path = build_libpath(fname);
	delete[] fname;
}
	if ((f = fopen(path,open_params)) != NULL)
	{
		if (!fread(&map_w,sizeof(map_w),1,f)) abort();
		map_w = ltohl(map_w);
		if (!fread(&map_h,sizeof(map_h),1,f)) abort();
		map_h = ltohl(map_h);
		if (!fread(&n,sizeof(n),1,f)) abort();
		n = htole32(n);
		size = map_w * map_h;
		csize = n;
		cbytes = new unsigned char[csize];
		MYCHECK(cbytes != NULL);
		nread = static_cast<int>(fread(cbytes,1,static_cast<size_t>(csize),f));
		if (ferror(f))
		{
			error_jump("TextrMap: error reading from file %s %d bytes\n",
			 path,nread);
			delete [] cbytes;
			cbytes = NULL;
			return;
		}
		fclose(f);
		if (bytes != NULL)
		{
			delete [] bytes;
			bytes = NULL;
		}
		if (!cflg)
			getBytes(0);
	}
}

unsigned char *TextrMap::getBytes(int cflg)
{
long r;

	if (bytes != NULL)
		return (bytes);
	if (cbytes == NULL)
	{
		error_jump("TextrMap:: cbytes are NULL\n");
		return (NULL);
	}
	bytes = new unsigned char[size];
	MYCHECK(bytes != NULL);
	r = unc_mem_to_mem(bytes,size,cbytes,csize);
	if (r <= 0)
	{
		error_jump("TextrMap %s: decompression error\n", id);
		delete [] bytes;
		bytes = NULL;
	}
	else if (!cflg)
	{
		delete [] cbytes;
		cbytes = NULL;
	}
	return (bytes);
}

void TextrMap::clearBytes()
{
  if (bytes != NULL)
    {
      delete [] bytes;
      bytes = NULL;
    }
}

void TextrMap::setColorMaps(float r)
{
  if (cmaps != NULL)
    for (int i=0;i<nm;i++)
      cmaps[mc[i]].calcMappedColor(r);
}

void TextrMap::setColorMaps(int base, int range, int col)
{
  if (cmaps != NULL)
    for (int i=0;i<nm;i++)
      cmaps[mc[i]].calcMappedColor(base,range,col);
}

/*************************************************************
 * TextrMap_Manager methods                                  *
 *************************************************************/
TextrMap_Manager::TextrMap_Manager(int n)
{
  n_maps = n;
  tmaps = new TextrMap[static_cast<unsigned int>(n_maps)];
  MYCHECK(tmaps != NULL);
  nxt = 0;
}

int TextrMap_Manager::add_map(TextrMap &tm)
{
  if (nxt < n_maps - 1)
    {
      tmaps[nxt++] = tm;
      return 1;
    }
  else
    return 0;
}

void TextrMap_Manager::read(std::istream &is)
{
int n;

	if (tmaps == NULL)
	{
		is >> n_maps;
		if (n_maps <= 0)
			return;
		tmaps = new TextrMap[static_cast<unsigned int>(n_maps)];
		MYCHECK(tmaps != NULL);
		for (int i=0;i<n_maps;i++)
			is >> tmaps[i];
			nxt = n_maps - 1;
	}
	else
	{
		is >> n;
		for (int i=0;i<n;i++)
		{
			TextrMap tmp;
			if (nxt < n_maps - 1)
			{
				is >> tmaps[nxt];
				nxt++;
			}
			else
				is >> tmp;
		}
	}
}

std::istream &operator >>(std::istream &is, TextrMap_Manager &tm)
{
  tm.read(is);
  return (is);
}

void TextrMap_Manager::read_file(const char *path)
{
  std::ifstream is;
  if (open_is(is,path))
    is >> *this;
}

void TextrMap_Manager::write(std::ostream &os)
{
  os << nxt << '\n';
  for (int i=0;i<nxt;i++)
    os << tmaps[i] << '\n';
}


std::ostream &operator <<(std::ostream &os, TextrMap_Manager &tm)
{
  tm.write(os);
  return (os);
}

void TextrMap_Manager::write_file(const char *path)
{
  std::ofstream os;
  if (open_libos(os,path))
    write(os);
}


TextrMap *TextrMap_Manager::get_map_ptr(const char *id)
{
	TextrMap *result = NULL;
	for (int i=0;i<n_maps;i++)
	{
		if (!strcmp(id,tmaps[i].id))
		{
			result = &tmaps[i];
			break;
		}
	}
	return (result);
}
