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

/***************************************************************
 * Important to reverse the y directions to match the world    *
 * system where y increases in the 'up' direction              *
 ***************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

#define MAXLEN 70
#define NO_ERR 0
#define OUTFILE_ERR 1
#define INFILE_ERR 2
#define BUFFERGET_ERR 3

int color_counts[256];
int ncolors;
long textr_width;
long textr_height;
int nrows;
int ncols;
int ntextr = -1;
int textr_trans = 0;
int reverse_y = 0;
char *pcx_file = NULL;
char *out_file = NULL;
char *palette_file = NULL;

char *id = NULL;
int renumber = -1;

typedef struct rgb_struct
{
  unsigned char r;
  unsigned char g;
  unsigned char b;
} _rgb ;

typedef struct rgb_info_struct
{
  _rgb rgb;
  int  color;
  int  count;
  int  mapped_color;
} rgb_info;

rgb_info rgb_infos[256];
rgb_info remap_infos[256];

int found_palette;

typedef struct PCX_HEADER {
  char manufacturer;
  char version;
  char encoding;
  char bits_per_pixel;
  short  xmin,ymin;
  short  xmax,ymax;
  short  hres;
  short  vres;
  char palette16[48];
  char reserved;
  char color_planes;
  short  bytes_per_line;
  short  palette_type;
  char filler[58];
} pcx_header;

struct IMG {
  unsigned char  *buffer;
  unsigned int xsize;
  unsigned int ysize;
} image;

int count_colors(void);
void remap_colors(void);
void error_exit(int err, char *format, ...);
void write_tmap(FILE *f, char *id, long i, long j);
void write_tmap_rv(FILE *f, char *id, long i, long j);
char *format_byte(int );
void make_maps(char *);
void loadpcx(char *);
void loadpal(char *);
int  maprgb(rgb_info *rinfo, rgb_info *map, int n);
float calc_rgb_distance(_rgb *rgb0, _rgb *rgb1);

void main(int argc, char *argv[])
{
  int tmp;
  fprintf(stderr,"rmptextr  07/19/97\n");
  if (argc < 9)
    {
      fprintf(stderr,"rmptextr <pcx_file> <output_file> <palette_file> <id>"
	     " <image_width> <image_height> <trans> <reverse_y>\n");
      return;
    }
  pcx_file    =  argv[1];
  out_file =     argv[2];
  palette_file = argv[3];
  id       =     argv[4];
  textr_width =  atoi(argv[5]);
  textr_height = atoi(argv[6]);
  textr_trans  = atoi(argv[7]);
  reverse_y    = atoi(argv[8]);
  fprintf(stderr," Input File: %s\n"
	 "Output File: %s\n"
         "Palett File: %s\n"
	 "      width: %d\n"
	 "     height: %d\n"
	 "      trans: %d\n"
	 "  reverse_y: %d\n",
	 pcx_file,out_file,palette_file,textr_width,textr_height,textr_trans,reverse_y);
  loadpcx(pcx_file);
  loadpal(palette_file);
  nrows = image.ysize / textr_height;
  ncols = image.xsize / textr_width;
  ntextr = nrows * ncols;
  fprintf(stderr,"Image xsize: %d\n"
         "Image ysize: %d\n"
         "  ntextures: %d\n",
	 image.xsize,
	 image.ysize,
	 ntextr);
  count_colors();
  remap_colors();
  make_maps(out_file);
}

char *format_byte(int byte)
{
  static char return_buff[6];
  sprintf(return_buff,"%03u",byte);
  return (return_buff);
}

int count_colors()
{
  int i;
  ncolors = 0;
  for (i=0;i<256;i++)
    if (rgb_infos[i].count > 0)
      ncolors++;
}

float calc_rgb_distance(_rgb *rgb0, _rgb *rgb1)
{
  float result;
  float dr,dg,db;

  dr = (float) (rgb0->r - rgb1->r);
  dg = (float) (rgb0->g - rgb1->g);
  db = (float) (rgb0->b - rgb1->b);

  result = sqrt((dr * dr) + (dg * dg) + (db * db));
  return (result);
}


int  maprgb(rgb_info *rinfo, rgb_info *map, int n)
{
  int i;
  int tmp_rd,tmp_gd,tmp_bd;
  int result;
  float mindist;
  float mc;

  if ((rinfo->rgb.r == 0) && (rinfo->rgb.g == 0) && (rinfo->rgb.b == 0)
      && (textr_trans != 0))
    return (0);

  result = -1;

  mindist = sqrt((255 * 255) + (255 * 255) + (255 * 255)) + 100;
 
  for (i=1;i<n;i++)
    {
      mc = calc_rgb_distance(&rinfo->rgb,&map[i].rgb);
      if (mc < mindist)
	{
	  result = i;
	  mindist = mc;
	}
    }

  return (result);
}

void remap_colors()
{
  int i;
  int m;
  for (i=1;i<256;i++)
    {
      if (rgb_infos[i].count > 0 && i != textr_trans)
	{
	  m = maprgb(&rgb_infos[i],remap_infos,256);
	  if (m < 0)
	    error_exit(1,"bad map value returned: %d",m);
	  rgb_infos[i].mapped_color = m;
	  fprintf(stderr,"(%d %d %d %d)==>(%d %d %d %d)\n",
		  i,
		  rgb_infos[i].rgb.r / 4,
		  rgb_infos[i].rgb.g / 4,
		  rgb_infos[i].rgb.b / 4,
		  m,
		  remap_infos[m].rgb.r / 4,
		  remap_infos[m].rgb.g / 4,
		  remap_infos[m].rgb.b / 4);
	}
    }
}

void write_tmap_rv(FILE *f, char *id_str, long i, long j)
{
  unsigned char  *bfptr,*bfptr2;
  int ii,jj;
  int cc;

  fprintf(f,"{\n");
  fprintf(f,"%s\n",id_str);
  fprintf(f,"%d %d %d\n",textr_width,textr_height,textr_trans);
  bfptr = image.buffer + (j * textr_width) + (i * ((long)image.xsize) * textr_height); 
  for (ii=0;ii<textr_height;ii++)
    {
      bfptr2 = bfptr;
      fprintf(f,"\t");
      for (jj=0;jj<textr_width;jj++)
	{
	  cc = (int) *bfptr2++;
	  
	  fprintf(f,"%s ", 
		  format_byte(rgb_infos[cc].mapped_color));
	}
      fprintf(f,"\n");
      bfptr += image.xsize;
    }
  fprintf(f,"}\n");
}

void write_tmap(FILE *f, char *id_str, long i, long j)
{
  unsigned char  *bfptr,*bfptr2;
  int ii,jj;
  int cc;

  fprintf(f,"{\n");
  fprintf(f,"%s\n",id_str);
  fprintf(f,"%d %d %d\n",textr_width,textr_height,textr_trans);
  bfptr = image.buffer + (j * textr_width) + ((i+1) * ((long)image.xsize) * textr_height); 
  bfptr -= image.xsize;
  for (ii=0;ii<textr_height;ii++)
    {
      bfptr2 = bfptr;
      fprintf(f,"\t");
      for (jj=0;jj<textr_width;jj++)
	{
	  cc = (int) *bfptr2++;
	  
	  fprintf(f,"%s ", 
		  format_byte(rgb_infos[cc].mapped_color));
	}
      fprintf(f,"\n");
      bfptr -= image.xsize;
    }
  fprintf(f,"}\n");
}


void make_maps(char *path)
{
  FILE *f;
  long i,j;
  char id_str[9];
  int n = 0;
  
  if (*path == '-')
    f = stdout;
  else
    {
      if ((f = fopen(path,"w")) == NULL)
	error_exit(1,"Couldn't open %s",path);
    }

  /* Make texture maps */
  fprintf(f,"%d\n",ntextr);
  
  /* 
     The reverse_y flag really means preserve the
     original direction of the bitmap, where y  
     increases as you move down. The normal 
     conversion reverses this to match the
     standard coordinant system
     */
  if (reverse_y)
    {
      for (i=0;i<nrows,n < ntextr;i++)
	{
	  for (j=0;j<ncols;j++)
	    {
	      sprintf(id_str,"%s%02d%02d",
		      id,i,j);
	      fprintf(stderr,"texture: %s\n",id_str);
	      write_tmap_rv(f,id_str,i,j);
	      n++;
	      if (n >= ntextr)
		break;
	    }
	}
    }
  else
    {
      for (i=nrows-1;i>=0,n < ntextr;i--)
	{
	  for (j=0;j<ncols;j++)
	    {
	      sprintf(id_str,"%s%02d%02d",
		      id,i,j);
	      fprintf(stderr,"texture: %s\n",id_str);
	      write_tmap(f,id_str,i,j);
	      n++;
	      if (n >= ntextr)
		break;
	    }
	}
    }
}

void loadpcx(char * filename)
{
  FILE *infile;
  char  * ImagePtr;
  unsigned int x, i=0;
  unsigned int Points;
  int c;
  pcx_header pcxhead;

  fprintf(stderr,"loading pcx ...\n");
  for (i=0;i<256;i++)
    rgb_infos[i].count = 0;

  found_palette = 0;
  if ((infile=fopen(filename,"rb"))==NULL)
    error_exit(1,"Couldn't Open %s",filename);
  fseek(infile,0L,SEEK_SET);
  fread(&pcxhead,sizeof(pcx_header),1,infile);
  image.xsize = (pcxhead.xmax-pcxhead.xmin) + 1;
  image.ysize = (pcxhead.ymax-pcxhead.ymin) + 1;
  fprintf(stderr,"xsize: %d, ysize %d\n",
	  image.xsize,
	  image.ysize);
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
	      rgb_infos[c].count++;
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
      fprintf(stderr,"Found extended palette\n");
      found_palette = 1;
      for (i=0;i<256;i++)
	{
	  fread(&rgb_infos[i].rgb,3,1,infile);
	  rgb_infos[i].color = i;
	  rgb_infos[i].mapped_color = i;
	}
    }
  fclose(infile);
}

void loadpal(char *path)
{
  char buff[BUFSIZ];
  FILE *f;
  unsigned int idx,r,g,b;
  char *cptr;

  if ((f = fopen(path,"r")) == NULL)
    error_exit(1,"couldn't read %s\n",path);

  while(fgets(buff,sizeof(buff),f))
    {
      if (cptr = strchr(buff,'('))
	{
	  idx = atoi(strtok(cptr,"( "));
	  r   = atoi(strtok(NULL," "));
	  g   = atoi(strtok(NULL," "));
	  b   = atoi(strtok(NULL,") "));
	  /*
	  fprintf(stderr,"( %d %d %d %d )\n",
		  idx,r,g,b);
		  */
	  remap_infos[idx].rgb.r = r * 4;
	  remap_infos[idx].rgb.g = g * 4;
	  remap_infos[idx].rgb.b = b * 4;
	}
    }
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


