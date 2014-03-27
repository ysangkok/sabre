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
 *           Sabre Fighter Plane Simulator              *
 * Version: 0.1                                  *
 * File   : siminc.h                             *
 * Date   : March, 1997                          *
 * Author : Dan Hammer                           *
 * Include files                                 *
 *************************************************/

/* macros & file handling includes */
#include "simerr.h"
#include "sim.h"
#include "simfile.h"

/* color, font includes */
#include "colorspc.h"
#include "font8x8.h"

/* basic graphics includes */
#include "defs.h"
#include "grafix.h"
#include "colormap.h"
#include "vga_13.h"
#include "traveler.h"
#include "pen.h"
#include "transblt.h"
#include "vmath.h"
#include "port_3d.h"
#include "clip.h"
#include "target.h"
#include "cpoly.h"
#include "obj_3d.h"
#include "group_3d.h"
#include "copoly.h"
#include "txtrmap.h"
#include "bits.h"
#include "convpoly.h"
#include "rendpoly.h"
#include "rndrpoly.h"
#include "rndzpoly.h"
#include "zview.h"
#include "rotate.h"

/* flight sim includes */
#include "dvmath.h"
#include "flight.h"
#include "fltlite.h"
#include "viewobj.h"
#include "moveable.h"
#include "unguided.h"
#include "fltzview.h"
#include "weapons.h"
#include "pilot.h"
#include "fltmngr.h"
#include "grndunit.h"

/* flight display includes */
#include "instrmnt.h"
#include "cockpit.h"
#include "hud.h"

/* input includes */
#include "input.h"
#include "pc_keys.h"
#include "kbdhit.h"
#include "key_map.h"
#include "siminput.h"
#include "portkey.h"

/* and whatever's left */
#include "earth.h"
#include "plltt.h"
#include "mytimer.h"
#include "globals.h"


