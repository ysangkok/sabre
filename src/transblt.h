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
/**************************************************************
 * transblt.h                                                 *
 **************************************************************/
#define MAX_RUNS 24

#define TRANSP_TYPE '0'
#define NTRANSP_TYPE '1'
#define RUN_END ' '
typedef struct run_str
{
  char type;
  int len;
} run;

class trans_table
{
public:
  unsigned char *trans_image;
  unsigned int image_rows, image_width;
  int trans_color;
  int ntrans_row;
  run runs[200][MAX_RUNS + 2];

  trans_table() : trans_image(NULL),
    image_rows(0),
    image_width(0),
    trans_color(0)
    { }

  void build_runs(unsigned char *img, unsigned int r, unsigned int w);
  void trans_blit(unsigned char *dst,
		  unsigned int x, unsigned int y, Rect *bnds = NULL,
		  bool fast_flag = 0);
};


