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
/*************************************************
 *           Sabre Fighter Plane Simulator       *
 * Version: 0.1                                  *
 * File   : waypoint.C                           *
 * Date   : January, 1998                        *
 * Author : Dan Hammer                           *
 *************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>
#include <fstream.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <values.h>
#include <stdarg.h>
#include "defs.h"
#include "sim.h"
#include "simerr.h"
#include "simfile.h"
#include "vmath.h"
#include "waypoint.h"

WayPoint ** WayPoint::gwaypoints = NULL;
int WayPoint::n_waypoints = 0;

void WayPoint::read(istream &is)
{
  char c = ' ';
  READ_TOK('(',is,c)
    is >> mode >> mode_x >> mode_y ;
  is >> location >> average_speed >> c;
  location *= world_scale;
  READ_TOK(')',is,c)
    average_speed = kts2fps(average_speed);
}

void WayPoint::write(ostream &os)
{
  os << "(\n" << mode << " " <<  mode_x << " " << mode_y << "\n";
  os << location << " " << fps2kts(average_speed) << "\n";
  os << ")\n";
}

void WayPoint::readWayPoints(istream &is)
{
  int i;
  int n;
  gwaypoints = NULL;
  is  >> n_waypoints;
  if (n_waypoints > 0)
    {
      if (gwaypoints != NULL)
	delete gwaypoints;
      gwaypoints = new WayPoint *[n_waypoints];
      MYCHECK(gwaypoints != NULL);
      for (i=0;i<n_waypoints;i++)
	gwaypoints[i] = readWayPointChain(is,n);
    }
}

WayPoint *WayPoint::readWayPointChain(istream &is, int &n)
{
  char c;
  WayPoint *result = NULL;
  READ_TOKI('{',is ,c);
  is  >> n;
  result = new WayPoint[n];
  MYCHECK(result != NULL);
  for (int j=0;j<n;j++)
    {
      is  >> result[j];
      if (j > 0)
	{
	  result[j].prev = &result[j-1];
	  result[j-1].next = &result[j];
	}
    }
  is  >> c;
  READ_TOK('}',is ,c);
  return (result);
}

void WayPoint::readWayPointFile(char *path)
{
  ifstream is;
  if (open_is(is,path))
    readWayPoints(is);
}

void WayPoint::writeWayPoints(ostream &)
{

}

void WayPoint::writeWayPointFile(char *path)
{
  ofstream os;
  if (open_libos(os,path))
    writeWayPoints(os);
}


WayPoint *WayPoint::getWayPoint(char *idd)
{
  WayPoint *result = NULL;
  for (int i=0;i<n_waypoints;i++)
    {
      WayPoint *wp = gwaypoints[i];
      while (wp != NULL)
	{
	  if (!strcmp(wp->id,idd))
	    {
	      result = wp;
	      break;
	    }
	  wp = wp->next;
	}
    }
  return (result);
}
