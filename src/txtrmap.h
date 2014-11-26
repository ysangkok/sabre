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
 *        Sabre Fighter Plane Simulator          *
 * File   : txtrmap.h                            *
 * Date   : March, 1997                          *
 * Author : Dan Hammer                           *
 *************************************************/
#ifndef __txtrmap_h
#define __txtrmap_h

#ifndef __colormap_h
#include "colormap.h"
#endif

#include <stdint.h>

#ifdef USES_DDRAW
#include <ddraw.h>
#include <d3d.h>
#endif

class TextrMap
{
private:
	unsigned char *bytes;
public:
	unsigned char *cbytes;
	int32_t map_w;
	int32_t map_h;
	int trans_colr;
	int delf;
	char id[32];
	long size;
	long csize;
	long flags;
	ColorMap *cmaps;
	int      *mc;
	int      nm;

	TextrMap()
	{
		bytes = NULL;
		cbytes = NULL;
		map_w = map_h = 0;
		delf = 1;
		trans_colr = -1;
		id[0] = 0;
		flags = 0;
		cmaps = NULL;
		mc = NULL;
		nm = 0;
	}

	TextrMap(unsigned char *bts, int mw, int mh, int tc, int del, int flgs = 0)
	{
		bytes = bts;
		cbytes = NULL;
		csize = 0;
		map_w = mw;
		map_h = mh;
		size = map_w * map_h;
		trans_colr = tc;
		delf = del;
		id[0] = 0;
		flags = flgs;
		cmaps = NULL;
		mc = NULL;
		nm = 0;
	}

	void copy(TextrMap &tm);

	TextrMap(TextrMap &tm)
	{
		copy(tm);
	}

	TextrMap &operator =(TextrMap &tm)
	{
		copy(tm);
		return(*this);
	}

	~TextrMap()
	{
		if (delf)
		{
			if (bytes != NULL)
				delete [] bytes;
			if (cbytes != NULL)
				delete [] cbytes;
		}
		if (cmaps != NULL)
			delete [] cmaps;
		if (mc)
			delete [] mc;
	}

	friend std::istream &operator >>(std::istream &is, TextrMap &tm);
	friend std::ostream &operator >>(std::ostream &os, TextrMap &tm);
	void read(std::istream &is);
	void write(std::ostream &os);
	void compress();
	void read_compressed(int = 0);
	unsigned char *getBytes(int = 0);
	void clearBytes();
	void setColorMaps(float r);
	void setColorMaps(int,int,int);
	int  getColorMap(int col)
	{
		if (cmaps && cmaps[col].id_color > -1)
			return (cmaps[col].mapped_color);
		else
			return (col);
	}
	int getPixelColor(int u, int v, int fill_color = -1)
	{
		int result;
		if (
		(u > map_w - 1) ||
		(v > map_h - 1) ||
		(u < 0)         ||
		(v < 0) 
		)
			result = fill_color;
		else
			result = *(bytes + (v * map_w) + u);
		if (result == trans_colr)
			result = fill_color;
		else if (cmaps)
			result = getColorMap(result);
		return result;
	}
};

extern TextrMap nullmap;

class TextrMap_Manager
{
public:
	unsigned int n_maps;
	unsigned int nxt;
	static int reserved;

	TextrMap *tmaps;
	TextrMap_Manager()
	{
		n_maps = 0;
		nxt = 0;
		tmaps = NULL;
	}
	TextrMap_Manager(int n);

	~TextrMap_Manager()
	{
		if (tmaps != NULL)
			delete [] tmaps;
		tmaps = NULL;
	}

	int add_map(TextrMap &);

	void read(std::istream &is);
	void read_file(const char *);
	friend std::istream &operator >>(std::istream &is, TextrMap_Manager &);

	void write(std::ostream &os);
	void write_file(const char *);
	friend std::ostream &operator <<(std::ostream &os, TextrMap_Manager &);


	TextrMap &get_map(int n)
	{
		if (tmaps != NULL && n >= 0 && static_cast<unsigned int>(n) < n_maps)
			return (tmaps[n]);
		else
			return (nullmap);
	}

	TextrMap *get_map_ptr(int n)
	{
		if (tmaps != NULL && n >= 0 && static_cast<unsigned int>(n) < n_maps)
			return (& tmaps[n]);
		else
			return (NULL);
	}

	TextrMap *get_map_ptr(const char *id);

	int getReserved()
	{
		return (reserved);
	}
};

extern TextrMap_Manager *map_man;

#endif
