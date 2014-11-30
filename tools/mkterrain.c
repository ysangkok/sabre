/*
    Sabre Flight Simulator 
    Copyright (C) 1997	Dan Hammer

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
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include "swap.h"

static long textr_width;
static long textr_height;
static int nrows;
static int ncols;
static int ntextr = -1;
static char *pcx_file = NULL;
static char *out_file = NULL;
static float max_colval;
static float min_colval;
static float max_zval = 4000.0;
static float floor_val = 0.0;
static int color_counts[256];
static int ncolors;

typedef struct terrain_points_struct
{
  float q1;
  float q3;
  float q5;
  float q7;
  float p1;
  float p2;
  float p3;
  float p4;
} terrain_points;

static terrain_points tpoints[33][33];

typedef struct rgb_struct
{
  unsigned char r;
  unsigned char g;
  unsigned char b;
} rgb ;
static rgb rgbs[256];
static int found_palette;

typedef struct PCX_HEADER {
  int8_t  manufacturer;
  int8_t  version;
  int8_t  encoding;
  int8_t  bits_per_pixel;
  int16_t xmin, ymin;
  int16_t xmax, ymax;
  int16_t hres, vres;
  int8_t  palette16[48];
  int8_t  reserved;
  int8_t  color_planes;
  int16_t bytes_per_line;
  int16_t palette_type;
  int8_t  filler[58];
} pcx_header;

static struct IMG {
  unsigned char  *buffer;
  unsigned int xsize;
  unsigned int ysize;
} image;

void get_color_range(void);
void error_exit(int err, const char *format, ...);
void calc_terrain(long i, long j);
void normalize();
void set_floor();
void write_terrain(FILE *f, long i, long j, int x, int y, int n, int);
float calcZ(unsigned char color);
void make_terrains(char *);
void loadpcx(char *);
int count_colors();

int main(int argc, char *argv[])
{
  //int tmp;
  fprintf(stderr,"mkterrain  06/24/97\n");
  if (argc < 5)
    {
      fprintf(stderr,"mterrain <pcx_file> <terrain_file>"
	     " <image_width> <image_height> [max_zva]\n");
      return 1;
    }
  pcx_file = argv[1];
  out_file = argv[2];
  textr_width = atoi(argv[3]);
  textr_height = atoi(argv[4]);

  if (argc == 6)
    max_zval = (float) atof(argv[5]);
  if (argc == 7)
    floor_val = (float) atof(argv[6]);
  fprintf(stderr," Input File: %s\n"
	 "Output File: %s\n"
	 "      width: %ld\n"
	 "     height: %ld\n"
         "   max_zval: %5.3f\n"
	 "  floor_val: %5.3f\n",
	 pcx_file,out_file,textr_width,textr_height,max_zval,floor_val);
  loadpcx(pcx_file);
  nrows = (int) (image.ysize / (unsigned) textr_width);
  ncols = (int) (image.xsize / (unsigned) textr_height);
  ntextr = nrows * ncols;
  if (ntextr > 32 * 32)
    {
      error_exit(1,"%d exceeds %d maximum subdivisions\n",
		 ntextr, 32 * 32);
    }
  fprintf(stderr,"Image xsize: %d\n"
         "Image ysize: %d\n"
         "  ntextures: %d\n",
	 image.xsize,
	 image.ysize,
	 ntextr);
  count_colors();
  get_color_range();
  fprintf(stderr,"max_colval = %3.0f, min_colval = %3.0f\n",
	 max_colval,min_colval);
  make_terrains(out_file);
}


float calcZ(unsigned char color)
{
  float result;
  float col,r1;
  //float r2;
  int i;
  i = (int) color;
  col = (float) rgbs[i].b;
  r1 = (col - min_colval) / (max_colval - min_colval);
  result = r1 * max_zval; 
  return (result);
}

/*******************************************************
 * After much pain and agony, please remember that     *
 * positive i moves UP, positive j moves RIGHT (like   *
 * x,y on the world grid.                              *
 *                                                     *
 * But since the bitmap's coordinants are left-handed  *
 * (y increasing as we move down) we must 'flip' the   *
 * pixels                                              *
 *******************************************************/
void calc_terrain(long i, long j)
{
  unsigned char  *bptr;
  float q1,q3,q5,q7;

  /* get to row nrows - i in the bitmap */
  bptr = image.buffer + (j * textr_width) + ((nrows - 1 - i) * ((long)image.xsize) * textr_height);
  /* now get to the bottom  of this portion */
  bptr += ((long)image.xsize) * (textr_height - 1);
  
  
  if ((q1 = tpoints[i][j].q1) < 0.0)
    {
      q1 = calcZ(*(bptr - ((unsigned) (textr_height-1) * image.xsize)));
      tpoints[i][j].q1 = q1;
      tpoints[i+1][j].q7 = q1;
      if (j > 0)
	tpoints[i+1][j-1].q5 = q1;
      if (j > 0)
	tpoints[i][j-1].q3 = q1;
    }

  if ((q3 = tpoints[i][j].q3) < 0.0)
    {
      q3 = calcZ(*(bptr + textr_width - 1 - ((unsigned) (textr_height-1) * image.xsize)));      
      tpoints[i][j].q3 = q3;
      
      tpoints[i+1][j].q5 = q3;
      tpoints[i+1][j+1].q7 = q3;
      tpoints[i][j+1].q1 = q3;
    }

  if ((q5 = tpoints[i][j].q5) < 0.0)
    {
      q5 = calcZ(*(bptr + textr_width - 1));
      tpoints[i][j].q5 = q5;
      
      tpoints[i][j+1].q7 = q5;
      if (i > 0)
	{
	  tpoints[i-1][j].q3 = q5;
	  tpoints[i-1][j+1].q1 = q5;
	}
    }

  if ((q7 = tpoints[i][j].q7) < 0.0)
    {
      q7 = calcZ(*bptr);
      tpoints[i][j].q7 = q7;
      if (i > 0 )
	{
	  tpoints[i-1][j].q1 = q7;
	  if (j > 0)
	    tpoints[i-1][j-1].q3 = q7;
	}
      if (j > 0)
	tpoints[i][j-1].q5 = q7;
    }

  tpoints[i][j].p4 = calcZ(*(bptr + ((textr_width-1) / 4) - ((unsigned) textr_height / 4 * image.xsize)));
  tpoints[i][j].p3 = calcZ(*(bptr + ((textr_width-1) - (textr_width / 4))
	       - ((unsigned) textr_height / 4 * image.xsize) ));
  tpoints[i][j].p2 = calcZ(*(bptr + ((textr_width-1) - (textr_width / 4))
	       - ((unsigned) ((textr_height-1) - (textr_height / 4)) * image.xsize) ));
  tpoints[i][j].p1 = calcZ(*(bptr + (textr_width / 4)
	       - ((unsigned) ((textr_height-1) - (textr_height / 4)) * image.xsize) ));
  
}

void write_terrain(FILE *f,long i,long j,int x,int y,int n, int quad)
{
  fprintf(f,"row %ld col %ld\n",
	  i,j);
  fprintf(f,"[ < %d %d 0> ",x,y);
  fprintf(f,"%d %d 1 1 ",quad,n);
  fprintf(f,"%5.2f %5.2f %5.2f %5.2f  %5.2f %5.2f %5.2f %5.2f ]\n",
	    tpoints[i][j].q1,tpoints[i][j].q3,tpoints[i][j].q5,
	    tpoints[i][j].q7,tpoints[i][j].p1,tpoints[i][j].p2,
	    tpoints[i][j].p3,tpoints[i][j].p4);
}

#define FN0(x) { \
  (x) -= floor_val; \
		      if ((x) < 0.0) \
				       (x) = 0.0; \
						    }
void set_floor(void);
void set_floor()
{
  long i,j;
  for (i=0;i<nrows;i++)
    for (j=0;j<ncols;j++)
      {
	FN0(tpoints[i][j].q1)
	FN0(tpoints[i][j].q3)
	FN0(tpoints[i][j].q5)
	FN0(tpoints[i][j].q7)
	FN0(tpoints[i][j].p1)
	FN0(tpoints[i][j].p2)
	FN0(tpoints[i][j].p3)
	FN0(tpoints[i][j].p4)
      }
}

#define FN1(x) { \
  if ((x) < minval) \
		      minval = x; \
				    if ((x) > maxval) \
							maxval = x; \
								      }
     
#define FN2(x) { \
  (x) -= minval; \
		   }
void normalize(void);
void normalize()
{
  float minval;
  float maxval;
  long i,j;

  fprintf(stderr,"Normalizing\n");
  minval = (float) max_zval * 10.0f;
  maxval = -1.0;

  for (i=0;i<nrows;i++)
    for (j=0;j<ncols;j++)
      {
	FN1(tpoints[i][j].q1)
	FN1(tpoints[i][j].q3)
	FN1(tpoints[i][j].q5)
	FN1(tpoints[i][j].q7)
	FN1(tpoints[i][j].p1)
	FN1(tpoints[i][j].p2)
	FN1(tpoints[i][j].p3)
	FN1(tpoints[i][j].p4)
      }
  
  fprintf(stderr,"max z: %5.2f, min z: %5.2f\n",
	  maxval,minval);

  for (i=0;i<nrows;i++)
    for (j=0;j<ncols;j++)
      {
	FN2(tpoints[i][j].q1)
	FN2(tpoints[i][j].q3)
	FN2(tpoints[i][j].q5)
	FN2(tpoints[i][j].q7)
	FN2(tpoints[i][j].p1)
	FN2(tpoints[i][j].p2)
	FN2(tpoints[i][j].p3)
	FN2(tpoints[i][j].p4)
      }
}

void make_terrains(char *path)
{
  FILE *f;
  long i,j;
  //char id_str[9];
  int n = 0;
  int x,y;
  int q;

  for (i=0;i<nrows+1;i++)
    for (j=0;j<ncols+1;j++)
      {
	tpoints[i][j].q1 =
	  tpoints[i][j].q3 = 
	  tpoints[i][j].q5 =
	  tpoints[i][j].q7 = 
	  tpoints[i][j].p1 =
	  tpoints[i][j].p2 =
	  tpoints[i][j].p3 = 
	  tpoints[i][j].p4 =
	  -1.0;
      }

  
  for (i=0;i<nrows;i++)
    for (j=0;j<ncols;j++)
      calc_terrain(i,j);

  normalize();
  if (floor_val != 0.0)
    set_floor();

  if (*path == '-')
    f = stdout;
  else
    {
      if ((f = fopen(path,"w")) == NULL)
	error_exit(1,"Couldn't open %s",path);
    }

  fprintf(f,"%d\n",ntextr);
  y = (int) (-nrows / 2.0);
  for (i=0; i<nrows; i++)
    {
      x = (int) (-ncols / 2.0);
      for (j=0;j<ncols;j++)
	{
	  if (x > 0 && y > 0)
	    q = 0;
	  else if (x < 0 && y >0)
	    q = 1;
	  else if (x < 0 && y < 0)
	    q = 2;
	  else
	    q = 3;
	  write_terrain(f,i,j,x,y,n,q);
	  n++;
	  x++;
	}
      y++;
    }
  fclose(f);
}

void loadpcx(char * filename)
{
  FILE *infile;
  unsigned char  * ImagePtr;
  unsigned int x, i=0;
  unsigned int Points;
  int c;
  pcx_header pcxhead;

  found_palette = 0;
  if ((infile=fopen(filename,"rb"))==NULL)
    error_exit(1,"Couldn't Open %s",filename);
  fseek(infile,0L,SEEK_SET);
  if (!fread(&pcxhead,sizeof(pcx_header),1,infile)) abort();
  pcxhead.xmin = (short) ltohs((unsigned short) pcxhead.xmin);
  pcxhead.xmax = (short) ltohs((unsigned short) pcxhead.xmax);
  pcxhead.ymin = (short) ltohs((unsigned short) pcxhead.ymin);
  pcxhead.ymax = (short) ltohs((unsigned short) pcxhead.ymax);
  pcxhead.hres = (short) ltohs((unsigned short) pcxhead.hres);
  pcxhead.vres = (short) ltohs((unsigned short) pcxhead.vres);
  image.xsize = (unsigned) ((pcxhead.xmax-pcxhead.xmin) + 1);
  image.ysize = (unsigned) ((pcxhead.ymax-pcxhead.ymin) + 1);
  Points = image.xsize * image.ysize;
  image.buffer = (unsigned char  *) malloc(Points);
  if(image.buffer==NULL)
    error_exit(1,"Failed to allocate %d bytes",Points);
  ImagePtr=image.buffer;
  for (i=0;i<Points;++i)
    {
      c=fgetc(infile) & 0xff;
      if((c & 0xc0) == 0xc0)
	{
	  x=c & 0x3f;
	  c=fgetc(infile);
	  while(x--)
	    {
	      *(ImagePtr++) = (unsigned char) c;
	      i++;
	    }
	  i--;
	}
      else
	{
	  *(ImagePtr++) = (unsigned char) c;
	}
    }
  c = fgetc(infile);
  if (((int)c) == 12)
    {
      fprintf(stderr,"Found extended palette\n");
      found_palette = 1;
      for (i=0;i<256;i++)
	if (!fread(&rgbs[i],3,1,infile)) abort();
    }
  else
    error_exit(1,"Expected an extended palette\n");
  fclose(infile);
}

void error_exit(int err, const char *format, ...)
{
  va_list ap;
  va_start(ap,format);
  vfprintf(stderr,format,ap);
  va_end(ap);
  fprintf(stderr,"\n");
  if (err > 0)
    {
      fprintf(stderr,"Exiting ...\n");
      exit(err);
    }
}

int count_colors(void);
int count_colors()
{
  int i;
  int points;
  unsigned char  *bfptr;

  fprintf(stderr,"counting colors ... \n");
  for (i=0;i<256;i++)
    color_counts[i] = 0;
  ncolors = 0;

  points = (int) (image.xsize * image.ysize);
  bfptr = image.buffer;
  while (points--)
    {
      int n = (int) *bfptr++;
      if (color_counts[n] == 0)
	ncolors++;
      color_counts[n]++;
    }
  fprintf(stderr,"%d colors present\n",ncolors);
  return ncolors;
}

void get_color_range()
{
  float col;
  int i;
  max_colval = -1.0;
  min_colval = 63.0 * 4.0;
  
  for (i=0;i<256;i++)
    {
      if (color_counts[i] > 0)
	{
	  col = (float) rgbs[i].b;
	  if (col > max_colval)
	    max_colval = col;
	  if (col < min_colval)
	    min_colval = col;
	}
    }
}
