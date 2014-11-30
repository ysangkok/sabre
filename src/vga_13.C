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
 *           Sabre Fighter Plane Simulator       *
 * File   : vga_13.C                             *
 * Author : Dan Hammer                           *
 * low-level graphics routines using svgalib     *
 *                                               *
 * 12/97 Antti Barck: gdev library               *
 * 04/98 sync with windows vga_13.h              *
 * 12/98 David Vandewalle : SDL port             * 
 *************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <cstddef>
#include <cassert>
#include "defs.h"
#include "grafix.h"
#include "traveler.h"
#include "vga_13.h"
#include "ddaline.h"
#ifdef HAVE_LIBSDL
#include <SDL2/SDL.h>
int FilterEvents(void *userdata, SDL_Event *event);
static SDL_Texture *screen; 
static Uint32* myBuf;
static SDL_Renderer *sdlRenderer;
#else
#include "gdev-svgalib.h"
#include <directfb.h>
typedef struct { uint8_t a,r,g,b;} SDL_Color;
static IDirectFBSurface *primary = NULL;
#define DFBCHECK(x...)                                         \
  {                                                            \
    DFBResult err = x;                                         \
                                                               \
    if (err != DFB_OK)                                         \
      {                                                        \
        fprintf( stderr, "%s <%d>:\n\t", __FILE__, __LINE__ ); \
        DirectFBError( #x, err );                         \
      }                                                        \
  }
#endif
static SDL_Color *colors;

/*
  Define our own "vgamodes"
 */
#define G320x200x256  0
#define G640x480x256  1
#define G800x600x256  2
#define G1024x768x256 3

/* 
   These are now private to vga_13 module.
   Use lock_xbuff() to get pointer to 
   current screen buffer.
   */
static unsigned char *xbuffer;
static unsigned char *buffer_ptr;
static unsigned char *screen_ptr;

float         aspect_ratio    = 1.0;
unsigned int  SCREEN_WIDTH    = 320; 
unsigned int  SCREEN_PITCH    = 320;
unsigned int  SCREEN_HEIGHT   = 200;
unsigned int  MXSCREEN_WIDTH  = 320;
unsigned int  MXSCREEN_HEIGHT = 200;
unsigned int  MAX_X           = 319;
unsigned int  MAX_Y           = 199;
unsigned int  VGAMODE         = G320x200x256;
const unsigned int PALETTE_SIZE = 768;
const unsigned int N_COLORS     = 256;

extern int window_height, window_width;
int           window_width      = 0;
int           window_height      = 0;

//static int           wx0,wy0,wx1,wy1;
SimFont       *g_font = NULL;
Rect          cliprect(0,0,319,199);

#ifdef HAVE_LIBVGA
void upload_palette(int, int);
void upload_palette(int start, int end) {
   DFBSurfacePixelFormat  format;
   IDirectFBPalette* ret_interface;

   DFBCHECK(primary->GetPixelFormat(primary, &format));
   if (format != DSPF_LUT8) abort();
   DFBCHECK(primary->GetPalette(primary, &ret_interface));
   DFBCHECK(ret_interface->SetEntries(ret_interface, (const DFBColor*) colors, /*num_entries*/(end==-1 || start==-1 ? N_COLORS : end-start), /*offset*/start==-1 ? 0 : start));
}
#endif

void init_vga_13(void)
{
  int dimx,dimy;

  /* switch on vgamode to get x,y dims */
  switch (VGAMODE)
    {
    case G320x200x256:
      dimx = 320;
      dimy = 200;
      break;

    case G640x480x256:
      dimx = 640;
      dimy = 480;
      break;

    case G800x600x256:
      dimx = 800;
      dimy = 600;
      break;

    case G1024x768x256:
      dimx = 1024;
      dimy = 768;
      break;

    default:
      printf("svgalib mode %d not allowable, defaulting to G320x200x256\n",
	     VGAMODE);
      VGAMODE = G320x200x256;
      dimx = 320;
      dimy = 200;
      break;
    }

#ifdef HAVE_LIBSDL
  // maybe now would be a good time to do some SDL init?
  // sure, why not!
  // This is where to add SDL sound init later if needed ...
  
  if(SDL_Init(SDL_INIT_VIDEO) < 0)
     {
     fprintf(stderr,"Error during SDL init. SDL_Get_Error(): %s\n"
                    "Exiting to OS ...\n",
             SDL_GetError());     
     exit(0);        
     } 
  SDL_SetEventFilter(FilterEvents, nullptr);
  SDL_EventState(SDL_MOUSEMOTION, SDL_ENABLE);
#else
#endif

//
//  Setup the viewing window size ...
//
#ifdef HAVE_LIBSDL // viewing windows are kinda stupid under SDL. I cut em out.
                   // maybe hack through the rest of the code later, and hack
                   // em all out for speed.
  /* dh
   * 
   window_width  = dimx;
   window_height = dimy;
   */
  if (window_width != 0)
    dimx = window_width;
  if (window_height != 0)
    dimy = window_height;
  printf("%d %d %d %d\n",
	 window_width, window_height, dimx, dimy);
#else
  if (window_width == 0)
    window_width = dimx;
  if (window_height == 0)
    window_height = dimy;
#endif

//
// Next setup the video mode ...
//


   xbuffer    = (unsigned char *) calloc(dimx*dimy, sizeof(char));
   screen_ptr = xbuffer;
   buffer_ptr = xbuffer;
#ifdef HAVE_LIBSDL
// this is where we try to get our video mode

   window_width = dimx;
   window_height = dimy;

// register the SDL cleanup function ...
   atexit(SDL_Quit);

// then  setup the display ...

//   SDL_Surface *screen; is define above ...
SDL_Window *sdlWindow;
SDL_CreateWindowAndRenderer(dimx, dimy, 0, &sdlWindow, &sdlRenderer);
myBuf=new Uint32[dimx*dimy*4];
screen = SDL_CreateTexture(sdlRenderer,
                               SDL_PIXELFORMAT_ARGB8888,
                               SDL_TEXTUREACCESS_STREAMING,
                               dimx, dimy);
   if (screen == NULL) 
      { 
      fprintf(stderr, "Couldn't set SDL video mode: %s\n",
              SDL_GetError());
      exit(1);   
      }
SDL_SetWindowTitle(sdlWindow, "Sabre - SDL Version");
      
// now setup all of the Sabre stuff to point to our new buffer ...
	 
   SCREEN_WIDTH = dimx;
   SCREEN_HEIGHT = dimy;
   MXSCREEN_WIDTH = dimx;
   MXSCREEN_HEIGHT = dimy;
   MAX_X = SCREEN_WIDTH - 1;
   MAX_Y = SCREEN_HEIGHT - 1;
   cliprect.topLeft.x = 0;
   cliprect.topLeft.y = 0;
   cliprect.botRight.x = MAX_X;
   cliprect.botRight.y = MAX_Y;
   aspect_ratio = calc_aspect_ratio(MXSCREEN_WIDTH,MXSCREEN_HEIGHT);
   SCREEN_PITCH = SCREEN_WIDTH;
#else
  DFBCHECK(DirectFBCreate (&dfb));
  DFBCHECK(dfb->SetCooperativeLevel (dfb, DFSCL_NORMAL));
  DFBCHECK(dfb->SetVideoMode(dfb, dimx, dimy, 8));

  DFBSurfaceDescription   dsc;
  dsc.flags = DFBSurfaceDescriptionFlags(DSDESC_CAPS | DSDESC_PIXELFORMAT);
  dsc.caps  = DSCAPS_PRIMARY;
  dsc.pixelformat = DSPF_LUT8;
  DFBCHECK(dfb->CreateSurface( dfb, &dsc, &primary ));

/*
  IDirectFBDisplayLayer* layer;
  dfb->GetDisplayLayer(dfb, DLID_PRIMARY, &layer);
  DFBWindowDescription wdesc;
  IDirectFBWindow* window;
  wdesc.flags = DFBWindowDescriptionFlags(DWDESC_SURFACE_CAPS | DWDESC_PIXELFORMAT); // | DWDESC_WIDTH | DWDESC_HEIGHT 
  //wdesc.caps = DSCAPS_PRIMARY;
  //wdesc.width = width;
  //wdesc.height = height;
  wdesc.pixelformat = DSPF_ARGB;
  wdesc.surface_caps = DSCAPS_PRIMARY;
  DFBCHECK(layer->CreateWindow(layer, &wdesc, &window));
  DFBCHECK(window->GetSurface(window, &primary));
*/

  DFBCHECK(primary->GetSize (primary, &SCREEN_WIDTH, &SCREEN_HEIGHT));
  DFBCHECK(primary->GetSize (primary, &MXSCREEN_WIDTH, &MXSCREEN_HEIGHT));
  MAX_X = SCREEN_WIDTH - 1;
  MAX_Y = SCREEN_HEIGHT - 1;
  cliprect.topLeft.x = 0;
  cliprect.topLeft.y = 0;
  cliprect.botRight.x = MAX_X;
  cliprect.botRight.y = MAX_Y;
  SCREEN_PITCH = SCREEN_WIDTH;
/*
  int ret_x, ret_y;
  DFBRectangle ret_rect;
  primary->GetPosition(primary, &ret_x, &ret_y);
  primary->GetVisibleRectangle(primary, &ret_rect);
  printf("POSITION!! %d %d SCRN W/H %d %d VISIBLE RECT %d %d\n", ret_x, ret_y, SCREEN_WIDTH, SCREEN_HEIGHT, ret_rect.w, ret_rect.h);
*/
#endif
}

void restorecrtmode()
{
#ifdef HAVE_LIBSDL
   //  we are in X11, never touched the CRT mode to begin with. 
   //  soooooo do nothing as the atexit() stuff will do our cleanup.
#else
#endif
}

float calc_aspect_ratio(float width, float height)
{
  float ideal_vert;
  ideal_vert = width * 2.8f / 4.0f;
  return (height / ideal_vert);
}

void blit_buff()
{
#ifdef HAVE_LIBSDL
   assert(SCREEN_WIDTH != 0);
   for (int j=0; j<SCREEN_HEIGHT; j++) {
     for (int i=0; i<SCREEN_WIDTH; i++) {
       unsigned char idx = screen_ptr[j*SCREEN_WIDTH + i];
       int r = colors[idx].r, g = colors[idx].g, b = colors[idx].b;
       //printf("%d %d %d %d\n", idx, r, g, b);
       myBuf[j*SCREEN_WIDTH + i] = colors[idx].a << 24 | r << 16 | g << 8 | b << 0;
     }
   }
   SDL_UpdateTexture(screen, NULL, myBuf, SCREEN_WIDTH * 4);
   SDL_RenderClear(sdlRenderer);
   SDL_RenderCopy(sdlRenderer, screen, NULL, NULL);
   SDL_RenderPresent(sdlRenderer);
   //SDL_UpdateRect(screen, 0, 0, 0, 0);
#else

   u8* data;
   int pitch;

   DFBCHECK(primary->Lock( primary, DSLF_WRITE, (void**) &data, &pitch ));
   for (int j=0; j<SCREEN_HEIGHT; j++) {
     u8* dst = data + j * pitch;
     for (int i=0; i<SCREEN_WIDTH; i++) {
       unsigned char idx = screen_ptr[j*SCREEN_WIDTH + i];
       dst[i] = idx;
     }
   }
   primary->Unlock(primary);
   primary->Flip (primary, NULL, DSFLIP_NONE);
#endif
}

void clear_scr(int color)
{
  clear_scr(color,MAX_Y);
}

#ifdef HAVE_LIBSDL
void clear_scr(int color, int)
{
assert(color < 256);
SDL_SetRenderDrawColor(sdlRenderer, colors[color].r, colors[color].b, colors[color].g, colors[color].a);
SDL_RenderClear(sdlRenderer);
SDL_RenderPresent(sdlRenderer);
}
#else
void clear_scr(int color, int)
{
  primary->SetColor (primary, colors[color].r, colors[color].b, colors[color].g, colors[color].a);
  primary->FillRectangle (primary, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}
#endif

void fill_rect(Rect &r, int color)
{
  if (valid_rect(r))       
    {
      Rect r0 = r;
      cliprect2rect(cliprect,r0);
      //FillRect.x = (Sint16) r0.left();
      //FillRect.y = (Sint16) r0.top();
      //FillRect.w = (Sint16) RWIDTH(r0);
      //FillRect.h = (Sint16) RHEIGHT(r0);
      //SDL_FillRect(screen, &FillRect, (Uint32) color);   
      for (int j1=r0.top(); j1<r0.bottom(); j1++)
        for (int i1=r0.left(); i1<r0.right(); i1++)
          putpixel(i1, j1, color);
    }
}

void putpixel(int x, int y, int color)
{
  if (x < SCREEN_WIDTH && x >= 0 && y < SCREEN_HEIGHT && y >= 0)
    *(buffer_ptr + (y * SCREEN_PITCH) + x) = (char ) color;
}

void h_line(int x, int y, int len, int color)
{
  unsigned char *bf = buffer_ptr + (y * SCREEN_PITCH) + x;
  while (len--)
    *bf++ = (unsigned char) color;
}

void v_line(int x, int y, int len, int color)
{
  unsigned char *bf = buffer_ptr + (y * SCREEN_PITCH) + x;
  while(len--)
    {
      *bf = (unsigned char) color;
      bf += SCREEN_PITCH;
    }
}

void set_rgb_value(int color, char red, char green, char blue)
{
assert(color < 256);
if (!colors) colors = new SDL_Color[256];
colors[color].r = uint8_t(red << 2);
colors[color].g = uint8_t(green << 2);
colors[color].b = uint8_t(blue << 2);
colors[color].a = 255;
#ifdef HAVE_LIBVGA
upload_palette(-1,-1);
#endif
}

void get_rgb_value(int color, char *red, char *green, char *blue)
{
assert(color < 256);
// this gets a RGB tuple from the current palette
*red   = colors[color].r;
*green = colors[color].g;
*blue  = colors[color].b;
}

void set_palette(int startcolor, int endcolor, char *palette)
{
// this is a palette set function that reads tupples of 3 bytes in RGB order
// and sets them into the palette at the appropriate slot.

// Gonna have to set cursor remap in here!
for(int x=startcolor;x<endcolor;x++)
   {
   colors[x].r = *(palette++);
   colors[x].g = *(palette++);
   colors[x].b = *(palette++);
   colors[x].a = 255;
   }
#ifdef HAVE_LIBVGA
upload_palette(startcolor, endcolor);
#endif
}

void get_palette(int startcolor, int endcolor, char *palette)
{
// this gets a block of palette entries in one call.
for(int x=startcolor;x<endcolor;x++)
   {
   *(palette++) = colors[x].r;
   *(palette++) = colors[x].g;
   *(palette++) = colors[x].b;
   }
}

void fade_palette_out(int startcolor, int endcolor, p_callback *pb)
{
  char rgb[PALETTE_SIZE];
  int i, j, max;
  get_palette(0,256,rgb);
  for (max = 63; max >= 0; max--)
    {
      if (!pb(max))
	break;
      for (i = startcolor; i <= endcolor; i++)
	{
	  /*
	   * Make each red, green, blue <= max
	   */
	  j = i * 3;
	  if (rgb[j] > max)
	    rgb[j] = char(max);
	  if (rgb[j+1] > max)
	    rgb[j+1] = char(max);
	  if (rgb[j+2] > max)
	    rgb[j+2] = char(max);
	}
      set_palette(startcolor,endcolor,rgb);
    }
}

void clr_palette(int start, int number)
{
  int i;
  if ((start>256) | (start<0) | ((start+number)>256))
    return;
  for (i=start;i<(start+number);i++)
    set_rgb_value(i,0,0,0);
}

extern int ch_clip(int *, Rect *);
void mline(int, int, int, int, char);

void b_linedraw(int x1, int y1, int x2, int y2, int color, Rect *bounds)
{
  if (bounds)
    {
      int points[4];
      points[0] = x1;
      points[1] = y1;
      points[2] = x2;
      points[3] = y2;
      if (!ch_clip(points,bounds))
	return;
      x1 = points[0];
      y1 = points[1];
      x2 = points[2];
      y2 = points[3];
    }
  mline(x1,y1,x2,y2,(char) color);
}

void mline(int x0, int y0, int x1, int y1, char color)
{
  DDALine dline(x0,y0,x1,y1);
  do
    {
      putpixel(dline.X(),dline.Y(),color);
    }
  while (dline.step());
}

#ifdef HAVE_LIBSDL
int FilterEvents(void *, SDL_Event *event)
{
  if(event->type == SDL_KEYDOWN)
    return 1;
  return 0;
}
#endif

/**********************************************************
 * From windows version (DirectX)                         *
 **********************************************************/

unsigned char *lock_xbuff()
{
  return (xbuffer);
}

void free_xbuff() { }
void vga13_suspend() { }
void vga13_resume() { }
void vga13_drawprep();
void vga13_drawprep() { }
void vga13_begin_scene() { }
void vga13_end_scene() { }
