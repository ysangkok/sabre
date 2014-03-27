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
/*****************************************************
 * waypoint.h                                        *
 *****************************************************/
#ifndef __waypoint_h
#define __waypoint_h

enum waypoint_modes 
{
  mmtakeoff, mlanding, mnothing, mpursuit,
  mjink,     mmaneuver,   masis, mcap, 
  mintercept,mescort, mstrike, mdrone
};

enum maneuvers { mbreak_turn = 1, mimmelman, msplit_s,
		 mbarrel_roll, mroll_out, mtakeoff,
		 mclimb, mdescend, mturn };

class WayPoint
{
public:
  int mode;
  int mode_x;
  int mode_y;
  R_3DPoint location;
  float     average_speed;
  char id[17];
  WayPoint *prev;
  WayPoint *next;

  WayPoint()
  {
    average_speed = 0.0;
    next = NULL;
    prev = NULL;
	 id[0] = 0;
  }

  void copy(const WayPoint &wp)
  {
	 mode = wp.mode;
	 mode_x = wp.mode_x;
	 mode_y = wp.mode_y;
	 location = wp.location;
	 average_speed = wp.average_speed;
	 strcpy(id,wp.id);
  }

	WayPoint(const WayPoint &wp)
	{
		copy(wp);
	}

	const WayPoint & operator =(const WayPoint &wp)
	{
		copy(wp);
		return *this;
	}

  void read(istream &);
  friend istream &operator >> (istream & is, WayPoint &wp)
    {
      wp.read(is);
      return(is);
    }
  void write(ostream &);
  friend ostream &operator << (ostream &os, WayPoint &wp)
    {
      wp.write(os);
      return(os);
    }

  static void readWayPoints(istream &is);
  static void writeWayPoints(ostream &os);
  static void readWayPointFile(char *);
  static void writeWayPointFile(char *);
  static WayPoint *readWayPointChain(istream &is, int &);
  static WayPoint **gwaypoints;
  static int        n_waypoints;
  static WayPoint *getWayPoint(char *);
};

#endif

