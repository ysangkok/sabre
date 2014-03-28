// -*- mode: C; -*-
// jed does not understand .C

//
// Minefields:
// 
// This code is mine. You may use, modify or redistribute this code 
// in any APPROPRIATE manner you may choose. 
//  
// If you use, modify or redistribute this code, I am NOT responsible
// for ANY issue raised. 
// 
// ( If you modify or remove this note, this code is no longer mine. ) 
// 
// proff@iki.fi 
// 
//

// Thu Dec 18 00:19:40 EET 1997 (proff@alf.melmac)
// created file gdev.C

// Current version: 0.3.0 beta #15
// Last Modified:

//
// This simplified version was originally designed for Dan Hammer
// to use in his SABRE Fighter Plane Simulator
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vga.h>
#include <vgagl.h>
#include <vgamouse.h>

#include "gdev-svgalib.h"

class gdev_svgalib:public gdev {
 public:
    int open( int width=320, int height=200, int vicinity=8, int wwidth=-1, int wheight=-1 );
    int setview( int x0, int y0, int x1, int y1 );
    int close( void );
    int update( int partial=0, int x0=-1, int y0=-1, int x1=-1, int y1=-1 );
   
    int clear( int c=0, int flags=1 );
    int pixel( int x, int y, int c=-1 );
    int rect( int x, int y, int w, int h, int c, int fill=1 );
    int line( int x0, int y0, int x1, int y1, int c, void *fill=0 );
   
    gevent *event( int wait=0 );
};

void assign_gdev_svgalib( void )
{
   vga_disabledriverreport();
   G = new gdev_svgalib;
}

static int mouse_events=0;

void start_mouse_gdev_svgalib( void )
{
   mouse_events=1;
   vga_setmousesupport(1);
}

// --
// -- method open( width, height, vicinity, wwidth, wheight ) 
// --
// -- returns the result of opening the graphics device in resolution
// -- width * height * vicinity (if parameters are omitted values
// -- 320x200x8bpp are used) returns non-zero value when open fails
// -- NB: it is hazardous to call gdev methods on unopened device
// -- so it user is responsible to check whether this call succeed 
// -- or not
// --

#define MOODE vga_getmodeinfo(vga_mode)

static int SCAN_MOODE( int width, int height, int vicinity )
{  
int vga_mode, colors=(1<<vicinity);
   for( vga_mode=1; vga_mode<GLASTMODE; vga_mode++ )
     if(width==MOODE->width && 
	 height==MOODE->height && colors==MOODE->colors)
       break; 
   vga_mode=vga_hasmode(vga_mode)?vga_mode:0;
   return vga_mode;
}


int gdev_svgalib::open( int width, int height, int vicinity, int wwidth, int wheight )
{
   if( gflags & graphics_on ) {
      // you must close it first
      return -1;
   }
   
   int vga_mode;
   if( ( vga_mode = SCAN_MOODE(width,height,vicinity) ) == 0 )
     return -1;

   int rc=vga_setmode(vga_mode);
   gl_setcontextvga(vga_mode);
   
   if( !rc ) {
      gflags   |= graphics_on;
      dimx	= vga_getxdim();	
      dimy	= vga_getydim();
      
      // Dan Hammer:
      // make window_width & window_height are within bounds
      
      // proff:
      // and deal defaults as well

      if( wwidth == -1 || wwidth > dimx )
	wwidth = dimx;
      if( wheight == -1 || wheight > dimy )
	wheight = dimy;

      // 1:1 viewport
        
      if( wwidth == dimx && wheight == dimy ) {   
	setview(0,0,dimx,dimy);
      } else {
	 // center requested window
	 int wx0,wy0,wx1,wy1;
	 wx0 = (dimx - wwidth) / 2;
	 wy0 = (dimy - wheight) / 2;
	 wx1 = wx0 + wwidth;
	 wy1 = wy0 + wheight;
	 setview(wx0,wy0,wx1,wy1);	     
      }
   }
   return rc;
}

// --
// -- method setview( x0, y0, x1, y1 ) 
// --
// -- returns the value of setting viewport ranging upper left corner (x0,y0)
// -- to lower right corner (x1,y1) in device coordinates
// --

int gdev_svgalib::setview( int x0, int y0, int x1, int y1 )
{
   vx=x0;
   vy=y0;
   vdimx=x1-x0;
   vdimy=y1-y0;
   
   // clear allocated vbuf if any
   if( vbuf )
     delete vbuf;
   
   if( dimx==vdimx && dimy==vdimy ) {
      gflags|=fulldevice;
   }
   else {
      gflags&=~fulldevice;
   }
   vbuf=new unsigned char[vdimx*vdimy];
   unsigned char *p=vbuf;
   vbufend=p+(vdimx*vdimy);
   return 0;
}

// --
// -- method close( ) 
// --
// -- returns the value of closing graphics device
// -- no parameters are used nor accepted
// --

int gdev_svgalib::close( void )
{
   if( !( gflags & graphics_on ) ) {
      // you must open it first
      return -1;
   }
   int rc=vga_setmode(TEXT);
   if( !rc ) {
      gflags&=~graphics_on;
      // clear allocated vbuf if any
      if( vbuf ) {
	 delete vbuf;
	 vbuf=NULL;
      }
   }
   return rc;   
}

// --
// -- method clear( fill, flags ) 
// --
// -- returns the value of clearing device/viewport by color palette index c
// -- first bit of flags (=1) clears viewport, second bit of flags (=2) clears
// -- graphics device
// --

int gdev_svgalib::clear( int fill, int flags )
{
   if( gflags & fulldevice ) {
      memset( vbuf, fill, vdimx*vdimy );
   } else {
      if( flags & 1 )
	memset( vbuf, fill, vdimx*vdimy );
      if( flags & 2 )
	gl_fillbox( 0, 0, dimx, dimy, fill ); // this updates by itself
   }
   return 0;
}

// --
// -- method update( flags ) 
// --
// -- returns the value of updating device from viewport framebuffer
// --

int gdev_svgalib::update( int partial, int x0, int y0, int x1, int y1 )
{
   if( partial ) {
      if( x1 > dimx )
	x1=dimx;
      if( y1 > dimy )
	y1=dimy;
      gl_putboxpart( x0, y0, x1-x0, y1-y0, vdimx, vdimy, vbuf, x0, y0 );
   } else {
      gl_putbox( vx, vy, vdimx, vdimy, vbuf );
   }
   return 0;
}

// --
// -- method rect( x, y, w, h, c, fill ) 
// --
// -- returns the value of drawing rectangle ranging upper left corner (x0,y0)
// -- to lower right corner (x1,y1) in viewport coordinates
// --

int gdev_svgalib::rect( int x, int y,  int w, int h, int c, int fill )
{
   if( fill ) {
      int i;
      for( i=0; i<h; i++, y++ ) {
	 unsigned char *ofs=vbuf+y*vdimx+x;
	 memset(ofs,c,w);
      }
   }
   return 0;
}

// --
// -- method ( x, y, c ) 
// --
// -- returns the color palette index for viewport position (x,y)
// -- if color palette index c is not -1 viewport position (x,y) is
// -- set to c
// --

inline int gdev_svgalib::pixel( int x, int y, int c )
{
   unsigned char *ofs=vbuf+x+y*vdimx;
   if( c != -1 )
     *ofs=c;
   return *ofs;
}

// --
// -- method line( x0, y0, x1, y1, c, *fill )
// --
// -- returns the result of line draw from (x0,y0) to (x1,y1) in viewpoint
// -- coordinates using colour palette index c and fill pattern in (non-zero)
// -- pointer fill (if fill is omitted fill is considered as zero)
// --

int gdev_svgalib::line( int x0, int y0, int x1, int y1, int c, void * /* fill */ )
{
   //
   // this DDA algorithm may contain bug
   // 
   
    int dx   = abs(x1 - x0);
    int dy   = abs(y1 - y0);

    int pixcnt, stepping, step_up, step_dn, x_step, x_back, y_step, y_back;

    int l=x0, t=y0, r=x1, b=y1;

    if( dx >= dy ) {

        pixcnt = dx + 1;
        stepping=(2*dy)-dx;
        step_up= 2 * dy;
        step_dn= 2 * (dy-dx);
        x_step = 1;
        x_back = 1;
        y_step = 1;
        y_back = 0;

    } else {

        pixcnt = dy + 1;
        stepping=(2*dx)-dy;
        step_up= 2 * dx;
        step_dn= 2 * (dx-dy);
        x_step = 1;
        x_back = 0;
        y_step = 1;
        y_back = 1;

    }

    if( x1 < x0 ) {
        l=x1;
        r=x0;
        x_step = -x_step;
        x_back = -x_back;
    }

    if( y1 < y0 ) {
        t=y1;
        b=y0;
        y_step = -y_step;
        y_back = -y_back;
    }

    // draw pixels

    for( int i = 0; i < pixcnt; i++ ) {
        pixel( x0, y0, c );
        if( stepping < 0 ) {
            stepping += step_up;
            x0  += x_back;
            y0  += y_back;
        } else {
            stepping += step_dn;
            x0  += x_step;
            y0  += y_step;
        }
    }

   return 0;
}

// --
// -- method event( wait )
// --
// -- returns pending event from input device or graphics system
// -- if wait is set to nonzero, method will block until some event
// -- occurs (if wait is omitted the value is zero ie. nonblocking)
// --

static gevent ge_queue[256], // circular queue
             *ge_queue_end  = ge_queue+255,
             *ge_queue_head = ge_queue,
             *ge_queue_tail = ge_queue;

  //
  // I hope these are now okay
  // The problem still lies in the overflow of event queue
  // -- you cannot, say, beep for it, especially if the beep
  //    device event gets overflowed ;-)
  //

static int INSERT_TO_GE_QUEUE( gevent *g )
{
   gevent *p=ge_queue_tail;
   ge_queue_tail->type=g->type;
   ge_queue_tail->data=g->data;
   ge_queue_tail++;
   if(ge_queue_tail==ge_queue_end)	
     ge_queue_tail=ge_queue;
   if( ge_queue_tail==ge_queue_head)
     ge_queue_tail=p; // overflow!     
   return p==ge_queue_tail;
}

static gevent_data gd_queue[128],
             *gd_queue_end  = gd_queue+127,
             *gd_queue_tail = gd_queue;

static int UPDATE_GE_QUEUE( int wait )
{
   static int buttons;
   static int mx=0, my=0;
   int c;
   gevent g;
   if( ( c = vga_getkey() ) != 0 ) {
      g.type=gevent::pressed|(c&gevent::datamask);
      INSERT_TO_GE_QUEUE( &g );
      g.type|=gevent::released;
      g.type^=gevent::pressed;
      INSERT_TO_GE_QUEUE( &g );
   }
   
   if( mouse_events ) {
      // mouse stuff
      mouse_update();
      int new_mx = mouse_getx();
      int new_my = mouse_gety();
      int new_buttons=mouse_getbutton();
   
      gevent_data *gd=gd_queue_tail;
      g.data=gd;
      gd_queue_tail++;
      if( gd_queue_tail == gd_queue_end )
	gd_queue_tail=gd_queue;
      
      gd->flags=0;
      gd->x=new_mx;
      gd->y=new_my;
      
      if( new_buttons != buttons ) {
	 // buttons changed
	 c=1;
	 if( new_buttons & MOUSE_LEFTBUTTON ) {
	    gd->flags|=1;
	    g.type=(gevent::pressed|gevent::device_serial_mouse|gevent::has_data);
	    INSERT_TO_GE_QUEUE( &g );
	 } else {
	    if( buttons & MOUSE_LEFTBUTTON ) {
	       gd->flags|=1;
	       g.type=(gevent::released|gevent::device_serial_mouse|gevent::has_data);
	       INSERT_TO_GE_QUEUE( &g );
	    }	    
         }
	 
	 gd=gd_queue_tail;
	 g.data=gd;
	 gd_queue_tail++;      
	 if( gd_queue_tail == gd_queue_end )
	   gd_queue_tail=gd_queue;
	 
	 gd->flags=0;
	 gd->x=new_mx;
	 gd->y=new_my;

	 if( new_buttons & MOUSE_RIGHTBUTTON ) {
	    g.type=(gevent::pressed|gevent::device_serial_mouse|gevent::has_data);
	    gd->flags|=4;
	    INSERT_TO_GE_QUEUE( &g );
	 } else {
	    if( buttons & MOUSE_RIGHTBUTTON ) {
	       gd->flags|=4;
	       g.type=(gevent::released|gevent::device_serial_mouse|gevent::has_data);
	       INSERT_TO_GE_QUEUE( &g );
	    }
	 }
	 buttons=new_buttons;
      } else
      if( new_mx != mx || new_my != my ) {
	 mx=new_mx;
	 my=new_my;
	 g.type=(gevent::has_data|gevent::device_serial_mouse);
	 INSERT_TO_GE_QUEUE( &g );
	 c=1;
      }
   }
							    
   if( wait && !c )
      c=1;
   return c==0;
}

static gevent *EXTRACT_GE_FROM_QUEUE( void )
{
   gevent *p=NULL;
   if( ge_queue_head != ge_queue_tail ) { 
      p=ge_queue_head;
      ge_queue_head++;
      if(ge_queue_head==ge_queue_end)
	ge_queue_head=ge_queue;
   }
   return p;
}

gevent *gdev_svgalib::event( int wait )
{
   gevent *ge=EXTRACT_GE_FROM_QUEUE();
   if( !ge ) {
      do {
	 wait = UPDATE_GE_QUEUE( wait );
      } while( wait );
      ge = EXTRACT_GE_FROM_QUEUE();
   }
   return ge;
}

class rgbdev8_svgalib:public rgbdev8 {
 public:
   int get( int c, char *r, char *g, char *b ); 
   int set( int c, char r, char g, char b ); 
   int get( void *pal, int start=0, int end=255 ); 
   int set( void *pal, int start=0, int end=255 ); 
};

void assign_rgbdev8_svgalib( void )
{
   RGB8 = new rgbdev8_svgalib;
}

int rgbdev8_svgalib::get( int c, char *red , char *green, char *blue ) 
{
   int r, g, b;
   gl_getpalettecolor( c, &r, &g, &b );
   *red=r;
   *green=g;
   *blue=b;
   return 0;
}

int rgbdev8_svgalib::set( int c, char red , char green, char blue  ) 
{
   gl_setpalettecolor( c, red, green, blue );
   return 0;
}

int rgbdev8_svgalib::get( void *pal , int s, int e  ) 
{
   gl_getpalettecolors( s, e-s+1, pal );
   return 0;
}

int rgbdev8_svgalib::set( void *pal , int s, int e ) 
{
   gl_setpalettecolors( s, e-s+1, pal );
   return 0;
}
