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
#include "gdev.h"

gdev *G = new gdev; // "There can be only one." (Sean Connery, 'Highlander')

gdev::gdev()
{
   gflags=0;
   vbuf=NULL;
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

int gdev::open( int /* width */, int /* height*/, int /* vicinity*/, int /* wwidth*/, int /* wheight*/ )
{
   fprintf( stderr, "gdev: call to abstract class gdev in %s!\n", __FILE__ );
   return -1;
}

// --
// -- method setview( x0, y0, x1, y1 ) 
// --
// -- returns the value of setting viewport ranging upper left corner (x0,y0)
// -- to lower right corner (x1,y1) in device coordinates
// --

int gdev::setview( int /* x0*/, int /* y0*/, int /* x1*/, int /* y1*/ )
{
   return -1;
}

// --
// -- method close( ) 
// --
// -- returns the value of closing graphics device
// -- no parameters are used nor accepted
// --

int gdev::close( void )
{
   return -1;
}

// --
// -- method clear( fill, flags ) 
// --
// -- returns the value of clearing device/viewport by color palette index c
// -- first bit of flags (=1) clears viewport, second bit of flags (=2) clears
// -- graphics device
// --

int gdev::clear( int /* fill*/, int /* flags*/ )
{
   return -1;
}

// --
// -- method update( flags ) 
// --
// -- returns the value of updating device from viewport framebuffer
// --

int gdev::update( int /* partial */, int /* x0 */, int /* y0 */, int /* x1 */, int /* y1 */ )
{
   return -1;
}

// --
// -- method rect( x, y, w, h, c, fill ) 
// --
// -- returns the value of drawing rectangle ranging upper left corner (x0,y0)
// -- to lower right corner (x1,y1) in viewport coordinates
// --

int gdev::rect( int /* x*/, int /* y*/,  int /* w*/, int /* h*/, int /* c*/, int /* fill*/ )
{
   return -1;
}

// --
// -- method line( x0, y0, x1, y1, c, *fill )
// --
// -- returns the result of line draw from (x0,y0) to (x1,y1) in viewpoint
// -- coordinates using colour palette index c and fill pattern in (non-zero)
// -- pointer fill (if fill is omitted fill is considered as zero)
// --

int gdev::line( int /* x0*/, int /* y0*/, int /* x1*/, int /* y1*/, int /* c*/, void * /* fill */ )
{
   return -1;
}

// --
// -- method event( wait )
// --
// -- returns pending event from input device or graphics system
// -- if wait is set to nonzero, method will block until some event
// -- occurs (if wait is omitted the value is zero ie. nonblocking)
// --

gevent *gdev::event( int /*wait*/ )
{
   return NULL;
}

rgbdev8 *RGB8 = new rgbdev8;


int rgbdev8::get( int , char * , char *  , char * ) 
{
   return 0;
}

int rgbdev8::set( int , char , char , char  ) 
{
   return 0;
}

int rgbdev8::get( void * , int , int  ) 
{
   return 0;
}

int rgbdev8::set( void * , int , int  ) 
{
   return 0;
}
