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

#ifdef SABREWIN
int kbdin = 0;
#else
#include <stdio.h>
#include <ctype.h>
#include "pc_keys.h"
#include "kbdhit.h"


/*
 Note: 12/14/97
 Use of vga_getkey for Version 0.2.2
 Replace with getchar() if strange
 or unexpected results are occurring
 */

/* 12/98 David Vandewalle: SDL port */

#ifdef HAVE_LIBSDL
#include <SDL/SDL.h>
#else
#include "vga.h"
#endif

// #define TESTME


#ifdef HAVE_LIBSDL
// Q&D hack, sorry
static const int sdl_to_standard[256] =
{   0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
   16,'i','m','l','j', 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
   32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
   48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
   64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
   80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
   96, 97, 98, 99,100,101,102,103,104,105,106,107,108,109,110,111,
  112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,
  128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
  144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,
  160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,
  176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,
  192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,
  208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,
  224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,
  240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255
};
#endif


KBHit kbhit;
int   KBHit::kbdin = 0;
KBHit::KBHit()
{

  tcgetattr(0,&oldios);
  newios = oldios;

  newios.c_lflag &= ~(ICANON | ECHO);
  newios.c_iflag &= ~(ISTRIP | IGNCR | ICRNL | INLCR | IXOFF | IXON);
  newios.c_cc[VMIN] = 0;	/* Making these 0 seems to have the */
  newios.c_cc[VTIME] = 0;	/* desired effect. */

  tcsetattr(0,TCSANOW,&newios);
  FD_ZERO(&rfds);
  FD_SET(0,&rfds);

}

int KBHit::kbdhit()
{
  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 0;
  FD_ZERO(&rfds);
  FD_SET(0,&rfds);
  return(select(1,&rfds,NULL,NULL,&tv));
}

int KBHit::getch()
{
#ifdef HAVE_LIBSDL
  SDL_Event event;
#endif

  unsigned char c;

#ifdef HAVE_LIBSDL
  if(SDL_PollEvent(&event))
#else  
    if (kbdhit())
#endif
      {
#ifdef HAVE_LIBSDL
	c=event.key.keysym.sym;
	c = sdl_to_standard[c];
#else
	c = vga_getkey();
#endif      
	if (c==ESC)
	  {
#ifdef HAVE_LIBSDL
	    c=event.key.keysym.sym;
#else
	    c = vga_getkey();
#endif     
	    if (c=='[')
#ifdef HAVE_LIBSDL 
	      c=event.key.keysym.sym;
#else
	    c = vga_getkey();
#endif
	    else
	      {
		putchar(c);
		c = ESC;
	      }
	  }
	tcflush(0,TCIFLUSH);
	kbdin = (int) c;
	return ((int)c);
      }
    else
      {
	kbdin = 0;
	return (0);
      }
}

KBHit::~KBHit()
{
  tcsetattr(0,TCSAFLUSH,&oldios);
}

#endif // SABREWIN
