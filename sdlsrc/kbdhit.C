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
#define HAVE_LIBSDL

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

  char c;

#ifdef HAVE_LIBSDL
  if(SDL_PollEvent(&event))
#else  
    if (kbdhit())
#endif
      {
#ifdef HAVE_LIBSDL
	c=SDL_SymToASCII(&(event.key.keysym),NULL);
#else
	c = vga_getkey();
#endif      
	if (c==ESC)
	  {
#ifdef HAVE_LIBSDL
	    c=SDL_SymToASCII(&(event.key.keysym),NULL);
#else
	    c = vga_getkey();
#endif     
	    if (c=='[')
#ifdef HAVE_LIBSDL 
	      c=SDL_SymToASCII(&(event.key.keysym),NULL);
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
