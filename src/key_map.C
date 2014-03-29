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
#include <stdio.h>
#include <string.h>
#include "key_map.h"
#include "pc_keys.h"

static kmp_t low_kmp[] =
{
	{ "NULL", (int) NULL },
	{ "BEL", BEL },
	{ "BS" , BS  },
	{ "LF" , LF  },
	{ "FF",  FF  },
	{ "CR",  CR  },
	{ "ESC", ESC },
	{ NULL, 0 }
};

static kmp_t funct_kmp[] =
{
	{ "S_TAB", S_TAB },
	{ "F1", F1 },
	{ "F2", F2 },
	{ "F3", F3 },
	{ "F4", F4 },
	{ "F5", F5 },
	{ "F6", F6 },
	{ "F7", F7 },
	{ "F8", F8 },
	{ "F9", F9 },
	{ "F10", F10 },
	{ "NUMLOCK", NUMLOCK },
	{ "SCROLLOCK", SCROLLOCK },
	{ "HOME", HOME },
	{ "UP", UP },
	{ "PG_UP", PG_UP },
	{ "MINUS", MINUS },
	{ "LEFT", LEFT },
	{ "FIVE", FIVE },
	{ "RIGHT", RIGHT },
	{ "PLUS", PLUS },
	{ "END", END },
	{ "DOWN", DOWN },
	{ "PG_DOWN", PG_DOWN },
	{ "INS", INS },
	{ "DEL",DEL },
	{ "ECHO", ECHO },
	{ "C_LEF", C_LEFT },
	{ "C_RIGHT", C_RIGHT },
	{ "C_END", C_END },
	{ "C_PG_DOWN",C_PG_DOWN },
	{ "C_HOME", C_HOME },
	{ NULL, 0 }
};

int kmp_str(char *s, int &f);
int kmp_str(char *s, int &f)
{
	int i = 0;
	while (low_kmp[i].str != NULL)
	{
		if (!strcmp(s,low_kmp[i].str))
		{
			f = 0;
			return (low_kmp[i].k);
		}
		i++;
	}
	i = 0;
	while (funct_kmp[i].str != NULL)
	{
		if (!strcmp(s,funct_kmp[i].str))
		{
			f = 1;
			return (funct_kmp[i].k);
		}
		i++;
	}
	return 0;
}
