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
#include "swap.h"

#define MAXLEN 70
#define NO_ERR 0
#define OUTFILE_ERR 1
#define INFILE_ERR 2
#define BUFFERGET_ERR 3

int color_counts[256];
int ncolors;
int textr_width;
int textr_height;
int ntextr;
int textr_trans = -1;
char *pcx_file = NULL;
char *out_file = NULL;
int renumber = -1;

typedef struct rgb_struct
{
  unsigned char r;
  unsigned char g;
  unsigned char b;
} rgb ;

rgb rgbs[256];
int found_palette;

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

struct IMG {
  unsigned char  *buffer;
  unsigned int xsize;
  unsigned int ysize;
} image;

int count_colors(void);
void renumber_image(int);
void error_exit(int err, char *format, ...);
void write_tmap(FILE *f, int which);
char *format_byte(unsigned char );
void make_maps(char *);
void loadpcx(char *);

void main(int argc, char *argv[])
{
  printf("MKTEXTR  02/11/97\n");
  if (argc < 6)
    {
      printf("MKTEXTR <pcx_file> <map_file>"
	     " <image_width> <image_height> <n_images>"
	     " [renumber] [trans_color]\n");
      return;
    }
  pcx_file = argv[1];
  out_file = argv[2];
  textr_width = atoi(argv[3]);
  textr_height = atoi(argv[4]);
  ntextr = atoi(argv[5]);
  if (argc >= 7)
    renumber = atoi(argv[6]);
  if (argc == 8)
    textr_trans = atoi(argv[7]);
  printf(" Input File: %s\n"
	 "Output File: %s\n"
	 "      width: %d\n"
	 "     height: %d\n"
	 "      count: %d\n",
	 pcx_file,out_file,textr_width,textr_height,ntextr);
  if (renumber != -1)
    printf("   renumber: %d\n",renumber);
  if (textr_trans != -1)
    printf("      trans: %d\n",textr_trans);

  loadpcx(pcx_file);
  count_colors();
  if (renumber != -1)
    renumber_image(renumber);
  make_maps(out_file);
}

char *format_byte(unsigned char c)
{
  static char return_buff[6];
  sprintf(return_buff,"%03u",(unsigned int) c);
  /*
    char bff[6];
    int l;
    sprintf(bff,"%02X",c);
    l = strlen(bff);
    if (l > 2)
    strcpy(return_buff,bff + l - 2);
    else
    strcpy(return_buff,bff);
    */
  return (return_buff);
}

int count_colors()
{
  int i;
  int points;
  unsigned char  *bfptr;

  for (i=0;i<256;i++)
    color_counts[i] = 0;
  ncolors = 0;

  points = image.xsize * image.ysize;
  bfptr = image.buffer;
  while (points--)
    {
      int n = (int) *bfptr++;
      if (color_counts[n] == 0)
	ncolors++;
      color_counts[n]++;
    }
  return ncolors;
}

void renumber_image(int start)
{
  int points;
  int i,n;
  unsigned char  *bfptr;
  rgb rgbx[256];
  int ccx[256];

  if (start < 0 || start > 255)
    error_exit(1,"%d: Invalid renumber",start);
  if (start + ncolors - 1 > 255)
    error_exit(1,"%d + %d colors exceeds 255",
	       start,ncolors);

  for (i=0;i<256;i++)
    ccx[i] = color_counts[i];

  for (i=0;i<256;i++)
    {
      if (ccx[i] > 0)
	{
	  points = image.xsize * image.ysize;
	  bfptr = image.buffer;
	  color_counts[i] = 0;
	  color_counts[start] = ccx[i];
	  rgbx[start] = rgbs[i];
	  while (points--)
	    {
	      n = (int) *bfptr;
	      if (n==i)
		*bfptr = (unsigned char) start;
	      bfptr++;
	    }
	  start++;
	}
    }

  for (i=0;i<256;i++)
    rgbs[i] = rgbx[i];
}

void write_tmap(FILE *f, int which)
{
  int i,j;
  unsigned char  *bfptr;
  fprintf(f,"{\n");
  fprintf(f,"%d %d %d\n",textr_width,textr_height,textr_trans);
  for (i=0;i<textr_height;i++)
    {
      bfptr = image.buffer + (image.xsize * i) + (which * textr_width);
      fprintf(f,"\t");
      for (j=0;j<textr_width;j++)
	fprintf(f,"%s ", format_byte(*bfptr++));
      fprintf(f,"\n");
    }
  fprintf(f,"}\n");
}

void make_maps(char *path)
{
  FILE *f;
  int i;

  if ((f = fopen(path,"w")) == NULL)
    error_exit(1,"Couldn't open %s",path);

  for (i=0;i<ntextr;i++)
    write_tmap(f,i);

  if (found_palette)
    {
      for (i=0;i<256;i++)
	if (color_counts[i] > 0) 
	  fprintf(f,"\t(%d %d %d %d)\n",
		  i,rgbs[i].r / 4, rgbs[i].g / 4, rgbs[i].b /4 );
	}

    }
  printf("\ndone\n");
  fclose(f);
}

void loadpcx(char * filename)
{
  FILE *infile;
  char  * ImagePtr;
  unsigned int x, i=0;
  unsigned int Points;
  int c;
  pcx_header pcxhead;

  found_palette = 0;
  if ((infile=fopen(filename,"rb"))==NULL)
    error_exit(1,"Couldn't Open %s",filename);
  fseek(infile,0L,SEEK_SET);
  fread(&pcxhead,sizeof(pcx_header),1,infile);
  pcxhead.xmin = ltohs(pcxhead.xmin);
  pcxhead.xmax = ltohs(pcxhead.xmax);
  pcxhead.ymin = ltohs(pcxhead.ymin);
  pcxhead.ymax = ltohs(pcxhead.ymax);
  pcxhead.hres = ltohs(pcxhead.hres);
  pcxhead.vres = ltohs(pcxhead.vres);
  image.xsize = (pcxhead.xmax-pcxhead.xmin) + 1;
  image.ysize = (pcxhead.ymax-pcxhead.ymin) + 1;
  Points = image.xsize * image.ysize;
  image.buffer = (char  *) malloc(Points);
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
	      *(ImagePtr++)=c;
	      i++;
	    }
	  i--;
	}
      else
	{
	  *(ImagePtr++)=c;
	}
    }
  c = fgetc(infile);
  if (((int)c) == 12)
    {
      printf("Found extended palette\n");
      found_palette = 1;
      for (i=0;i<256;i++)
	fread(&rgbs[i],3,1,infile);
    }
  fclose(infile);
}

void error_exit(int err, char *format, ...)
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
