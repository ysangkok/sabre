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
 * File   : cockpit.C                            *
 * Date   : March, 1997                          *
 * Author : Dan Hammer                           *
 *************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <values.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <iostream>
#include <fstream>
#include "defs.h"
#include "grafix.h"
#include "vga_13.h"
#include "traveler.h"
#include "pen.h"
#include "convpoly.h"
#include "vmath.h"
#include "port_3d.h"
#include "cpoly.h"
#include "font8x8.h"
#include "flight.h"
#include "rotate.h"
#include "colorspc.h"
#include "simfile.h"
#include "bits.h"
#include "instrmnt.h"
#include "transblt.h"
#include "txtrmap.h"
#include "cockpit.h"

Cockpit::Cockpit(unsigned char *byts,
	  unsigned int xs,
	  unsigned int ys,
	  int tclr,
	  char *ipath)
  : cbytes(byts),
    x_size(xs),
    y_size(ys),
    trans_color(tclr)
{
  t_table = new trans_table();
  t_table->trans_color = trans_color;
  t_table->build_runs(cbytes,y_size,x_size);
  y_clip = static_cast<unsigned int>(static_cast<int>(SCREEN_HEIGHT - y_size) + t_table->ntrans_row);
  ipanel = &instrument_panel;
  cockpit_y = SCREEN_HEIGHT - y_size;
  cockpit_x = 0;
  ::cockpit_y = cockpit_y;
  ::cockpit_x = cockpit_x;
  setup_instruments(ipath,*ipanel);
}

Cockpit::Cockpit(const char *tmap_id, const char *ipath)
{
  TextrMap *tmap = map_man->get_map_ptr(tmap_id);
  if (tmap == NULL)
    {
      fprintf(stderr,"Couldn't find cockpit, tmap id %s\n",
	      tmap_id);
      exit(1);
    }
  cbytes = tmap->getBytes();
  x_size = tmap->map_w;
  y_size = tmap->map_h;
  trans_color = tmap->trans_colr;

  t_table = new trans_table();
  t_table->trans_color = trans_color;
  t_table->build_runs(cbytes,y_size,x_size);
  y_clip = static_cast<unsigned int>(static_cast<int>(SCREEN_HEIGHT - y_size) + t_table->ntrans_row);
  ipanel = &instrument_panel;
  cockpit_y = SCREEN_HEIGHT - y_size;
  cockpit_x = 0;
  ::cockpit_y = cockpit_y;
  ::cockpit_x = cockpit_x;
  setup_instruments(ipath,*ipanel);
}

Cockpit::~Cockpit()
{
  if (t_table)
    delete t_table;
}

void Cockpit::draw(Flight &flight)
{
  unsigned char *buffer_ptr;
  if (SCREEN_WIDTH==320 && SCREEN_HEIGHT==200)
  //  if (VGAMODE == G320x200x256)
    {
      ipanel->underpaint(flight);
	  buffer_ptr = lock_xbuff();
	  if (!buffer_ptr)
		return;
      t_table->trans_blit(buffer_ptr,cockpit_x,cockpit_y,NULL,0);
	  free_xbuff();
      ipanel->display(flight);
	}
}
