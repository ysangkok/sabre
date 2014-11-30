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

// Thu Dec 18 00:19:47 EET 1997 (proff@alf.melmac)
// created file gdev.h

#ifndef __gdev_h
#define __gdev_h

#ifdef HAVE_LIBVGA
#include <directfb.h>

extern IDirectFB *dfb;
#endif

struct gevent {
   enum {
      click_mask		= 0xC0000000, // if not, there is motion
      released		     	= 0x80000000, // on/off (like key-on/key-off)
      pressed			= 0x40000000,
      has_data	 	    	= 0x20000000, // data located in pointer
      device_id 	      	= 0x1FFF0000, // bits 28..16 for device id
        device_kbd_pc	      	= 0x00000000, // sequentally read pc keyboard
        device_kbd_pc_raw    	= 0x00010000, // parallel read pc keyboard
        device_serial_mouse   	= 0x00020000, // serial mouse
        device_analog_joystick	= 0x00030000, // analog joystick
        device_graphics_card   	= 0x10000000, // 4096 devices reserved for
                                              // graphics system
      datamask		      	= 0x0000FFFF  // bits 15..0 for data
   };
   unsigned int type;
   void *data;
};

// generic data struture ( nice for mouse and stuffo )
struct gevent_data {
   unsigned int flags;
   int x, y, z;
};

class gdev {
 protected:
   unsigned int dimx, dimy, dimz;
   unsigned int vdimx, vdimy, vx, vy;
   unsigned char *vbuf, *vbufend;
   enum { 
      fulldevice	=0x00000001,
      graphics_on	=0x00000002
   };
   unsigned int gflags;
 public:
   gdev();
   virtual ~gdev() = default;
   virtual int open( int width=320, int height=200, int vicinity=8, int wwidth=-1, int wheight=-1 );
   virtual int setview( int x0, int y0, int x1, int y1 );
   virtual int close( void );
   virtual int update( int partial=0, int x0=-1, int y0=-1, int x1=-1, int y1=-1 );
   
           unsigned int getdimx( int flags=0 ) { return flags&fulldevice?dimx:vdimx; }
           unsigned int getdimy( int flags=0 ) { return flags&fulldevice?dimy:vdimy; }
   void *getvbuf( int end=0 ) { return end?vbufend:vbuf; }

   virtual int clear( int c=0, int flags=1 );
   virtual int rect( int x, int y, int w, int h, int c, int fill=1 );
   virtual int line( int x0, int y0, int x1, int y1, int c, void *fill=0 );
   
   virtual gevent *event( int wait=0 );
};

class rgbdev8 {
 public:
   virtual ~rgbdev8() = default;
   virtual int get( int c, char *r, char *g, char *b ); 
   virtual int set( int c, char r, char g, char b ); 
   virtual int get( void *pal, int start=0, int end=255 ); 
   virtual int set( void *pal, int start=0, int end=255 ); 
};

extern gdev *G;
extern rgbdev8 *RGB8;

#endif // __gdev_h
