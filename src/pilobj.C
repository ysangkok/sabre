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
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <values.h>
#include <stdarg.h>
#include "defs.h"
#include "sim.h"
#include "simfile.h"
#include "simsnd.h"
#include "vmath.h"
#include "port_3d.h"
#include "cpoly.h"
#include "copoly.h"
#include "flight.h"
#include "pilobj.h"

std::istream &operator >>(std::istream &is, Pilot_Params &ps)
{
	char c= ' ';
	READ_TOK('(',is,c);
	is >> ps.id;
	is >> ps.skill_level >> ps.affiliation >> ps.update_time;
	is >> ps.v_acquire_range;
	is >> ps.hitScaler;
	is >> ps.shootRadius;
	is >> ps.posG >> ps.negG;
	is >> ps.log;
	is >> c;
	READ_TOK(')',is,c);
	ps.v_acquire_range = m2f(ps.v_acquire_range);
	ps.shootRadius = ps.shootRadius *_degree;
	return is;
}

void PilotMessage::update()
{
	if (!flag)
	return;
	elapsed_time += time_frame;
	if (elapsed_time >= time_limit)
	{
		flag = 0;
		mssg = NULL;
	}
}

int PilotMessage::set_message(char *mss, int priority, Pilot *fr)
{
	if (!flag || flag <= priority)
	{
		elapsed_time = 0.0;
		flag = priority;
		mssg = mss;
		frm = fr;
		return (1);
	}
	else
		return (0);
}


