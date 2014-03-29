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
/************************************************************
 * traveler.cpp                                             *
 ************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "defs.h"
#include "grafix.h"
#include "traveler.h"

LineTraveler::LineTraveler(Point a, Point b, int d)
					  :here(a),there(b)
{
	 if (d <= 0)
		  d = 1;
	 delta = d;
	 int dh = AbsInt((b.x - a.x)) / delta;
	 int dv = AbsInt((b.y - a.y)) / delta;
	 if (dh >= dv) {
		  majorDelta = dh;
		  minorDelta = dv;
		  majAct = 1;
	 }
	 else {
		  majorDelta = dv;
		  minorDelta = dh;
		  majAct = 0;
	 }
	 if (b.x < a.x)
	 {
		xDir = -1;
		xdelta = -delta;
	 }
	 else
	 {
		xDir = 1;
		xdelta = delta;
	 }
	 if (b.y < a.y)
	 {
		yDir = -1;
		ydelta = -delta;
	 }
	 else
	 {
		yDir = 1;
		ydelta = delta;
	 }
	 n = 0;
}

LineTraveler::~LineTraveler() = default;

void LineTraveler::NewDelta(int nDelta)
{
	 delta = nDelta;
	 int dh = AbsInt((there.x - here.x)) / delta;
	 int dv = AbsInt((there.y - here.y)) / delta;
	 if (dh >= dv) {
		  majorDelta = dh;
		  minorDelta = dv;
		  majAct = 1;
	 }
	 else {
		  majorDelta = dv;
		  minorDelta = dh;
		  majAct = 0;
	 }
	 if (xDir == 1)
		xdelta = delta;
	 else
		xdelta = -delta;
	 if (yDir == 1)
		ydelta = delta;
	 else
		ydelta = -delta;
}

void LineTraveler::NextStep()
{
	 if (majAct)
		  here.x += xdelta;
	 else
		  here.y += ydelta;
	 n += minorDelta;
	 if (n >= majorDelta) {
		  if (majAct)
				here.y += ydelta;
		  else
				here.x += xdelta;
		  n = n - majorDelta;
	 }
}

Boolean LineTraveler::Done()
{
	 if (here == there)
		  return true;
	 else if (delta == 1)
		  return false;
	 switch (yDir) {
		  case 1:
		  if (here.y >= there.y)
				return true;
		  break;
		  case -1:
		  if (here.y <= there.y)
				return true;
		  break;
	 }
	 switch (xDir) {
		  case 1:
		  if (here.x >= there.x)
				return true;
		  break;

		  case -1:
		  if (here.x <= there.x)
				return true;
		  break;
	 }
	 return false;
}




