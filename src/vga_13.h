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
/*************************************************
 *           SABRE Fighter Plane Simulator       *
 * File   : vga_13.h                             *
 * Author : Dan Hammer                           *
 * Low level graphics routines header.           *
 *************************************************/
#ifndef __vga_13_h
#define __vga_13_h

#include "grafix.h"

class SimFont;

extern float aspect_ratio;               /* correction for y / x pixel ratio   */
extern int   VGAMODE;                    /* 320x200x8, etc                     */
extern int   SCREEN_WIDTH;               /* logical width of rendering screen  */
extern int   SCREEN_HEIGHT;              /* logical height of rendering screen */
extern int   MXSCREEN_WIDTH;             /* width of video screen              */
extern int   MXSCREEN_HEIGHT;            /* height of video screen             */
extern int   SCREEN_PITCH;               /* actual byte width of screen        */
extern int   MAX_X;                      /* usually SCREEN_WIDTH - 1           */
extern int   MAX_Y;                      /* usually SCREEN_HEIGHT - 1          */
extern const int   N_COLORS;                   /* 256 for now                        */
extern const int   PALETTE_SIZE;               /* 256 * 3                            */

extern SimFont      *g_font;
extern Rect         cliprect;

typedef int ( p_callback)(int);

void init_vga_13(void);
void restorecrtmode(void);
void putpixel(int x, int y, int color);
void read_palette(int start, int number, char * palette);
void clr_palette(int start, int number);
float calc_aspect_ratio(float h, float w);
void set_rgb_value(int color, char red, char green, char blue);
void get_rgb_value(int color, char *red, char *green, char *blue);
void set_palette(int startcolor, int endcolor, char *palette);
void get_palette(int startcolor, int endcolor, char *palette);
void fade_palette_out(int startcolor, int endcolor, p_callback *pb);

void clear_scr(int color, int row);
void clear_scr(int color);
void blit_buff();
void h_line(int x, int y, int len, int color);
void v_line(int x, int y, int len, int color);
void fill_rect(Rect &r, int color);

/* 
   These functions are for compatibility with
   DirectX or other packages
*/
unsigned char *lock_xbuff();
void free_xbuff();
void vga13_suspend();
void vga13_resume();
void vga13_begin_scene();
void vga13_end_scene();

#ifndef __grafix_h
struct Rect;
#endif

void b_linedraw(int x1, int y1, int x2, int y2, int color, Rect *bounds);

#endif

