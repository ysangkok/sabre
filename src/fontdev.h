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

// Mon Dec 22 15:58:37 EET 1997 (proff@alf.melmac)
// created file fontdev.h

#ifndef __fontdev_h
#define __fontdev_h

#ifdef HAVE_LIBVGA
#include "gdev.h" // we will need this later on
#endif

class fontdev {
   unsigned char *fbp;
   unsigned int dimx, dimy;
   unsigned int mindimx, mindimy;
 public:
   fontdev();
   ~fontdev();
   int load( const char *fn );
   int save( const char *fn );
   unsigned int getdimx( void ) { return dimx; }
   unsigned int getdimy( void ) { return dimy; }
   unsigned int getmindimx( void ) { return mindimx; }
   unsigned int getmindimy( void ) { return mindimy; }
   bool setcell( unsigned int _dimx, unsigned int _dimy );
   void calcmin( void );
   void *getfbp( unsigned int c );
   enum { up, right, left, down, turn_right, turn_left };
   void rotate( unsigned int what, int where );
   void shift( int what, int where );
};

extern const char **fontpaths;

#endif // __fontdev_h
