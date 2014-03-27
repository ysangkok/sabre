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
/*****************************************************
 * flite.h                                           *
 *****************************************************/
#ifndef __flite_h
#define __flite_h

class Pilot;
class WayPoint;
/*
  A formation of one or more planes ... 
  it's called a "Flight" in air-combat terms, but
  there's already a class called Flight, so ...
 */
class Flite
{
public:
  Pilot     *lead;
  Pilot     **wingmen;
  int       nwingmen;
  WayPoint  *waypoints;
  int       nwaypoints;
  enum  { FINGER_FOUR, V_STACK, H_STACK, C_STACK,
	  LADDER, WEDGE, VIC, ECHELON };
  char      id[17];

  Flite()
    {
      lead = NULL;
      wingmen = NULL;
      waypoints = NULL;
      nwingmen = 0;
      nwaypoints = 0;
    }
  
  ~Flite()
    {
      if (wingmen)
	delete wingmen;
      if (waypoints)
	delete [] waypoints;
    }

  /*
  void copy(const Flite &);
  
  Flite(const Flite &flt)
    {
      copy(flt);
    }

  Flite & operator =(const Flite &flt)
    {
      copy(flt);
      return(*this);
    }
    */

  void read(istream &);
  void write(ostream &);

  friend istream &operator >>(istream &is, Flite &flt)
    {
      flt.read(is);
      return(is);
    }

  friend ostream &operator <<(ostream &os, Flite &flt)
    {
      flt.write(os);
      return(os);
    }
};



#endif
