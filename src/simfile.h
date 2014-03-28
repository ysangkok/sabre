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
#ifndef __simfile_h
#define __simfile_h
#include "sim.h"
#include "simerr.h"
#include <iostream>
#include <fstream>

void read_palette_file(const char *);
void read_texture_file(const char *);
void read_sound_file(const char *);
int get_line(std::istream &, char *, int);
int open_is(std::ifstream &, const char *);
int open_os(std::ofstream &, const char *);
int open_libos(std::ofstream &, const char *);
const char *build_libpath(const char *);
int read_int(std::istream &);

extern const char *current_path;
extern const char *file_context;
extern int cur_line;

#define READ_TOKI(tok,is,c) c = '\0'; while ((is) && ((c) != (tok))) \
(is) >> (c); \
if (!(is)) error_jump("%s %s: Unexpected end-of-file", current_path, file_context);

#define READ_TOK(tok,is,c)  while ((is) && ((c) != (tok))) \
(is) >> (c); \
if (!(is)) error_jump("%s %s: Unexpected end-of-file", current_path, file_context);

#define READ_TOKX(tok,bt,is,c)  while ((is) && ((c) != (tok)) && ((c) != (bt))) \
(is) >> (c); \
if ((c) == (bt)) \
	error_jump("%s %s: Unexpected %c",current_path, file_context,bt); \
else \
if (!(is)) error_jump("%s %s: Unexpected end-of-file", current_path, file_context);

extern FILE * simlog;
int __cdecl sim_printf(const char *format, ...);

#endif
