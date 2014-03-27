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

// Mon Dec 29 16:40:01 EET 1997 (proff@alf.melmac)
// created file fontutils.C

// Current version:
// Last modified:

#include <stdio.h>
#include <stdarg.h>
#include "fontutils.h"

void gputc( fontdev *f, int x, int y, int c, int ascii )
{
   unsigned char *dest=(unsigned char *)G->getvbuf( ), 
                  *end=(unsigned char *)G->getvbuf(1);
   dest+=(x+y*G->getdimx());
   int fdimx=f->getdimx(), fdimy=f->getdimy(); 
   unsigned char *src=(unsigned char *)f->getfbp(ascii);
   int i, j, dstep=(G->getdimx()-fdimx);
   for( j=0; j<fdimy; j++, dest+=dstep ) {
      for( i=0; i<fdimx; i++, src++, dest++ ) {
	 if( dest >= end )
	   goto _abort_gputc;
	 if( *src ) {
	    *dest=c&*src;  
	 }	 
      }
   }
   
   _abort_gputc:
   return;
}

void gputs( fontdev *f, int x, int y, int c, char *s )
{
   unsigned char *dest=(unsigned char *)G->getvbuf( ),
                  *end=(unsigned char *)G->getvbuf(1);
   dest+=(x+y*G->getdimx());
   int fdimx=f->getdimx(), fdimy=f->getdimy(); 
   for( ; *s; s++ ) {
      unsigned char *src=(unsigned char *)f->getfbp((unsigned char)(*s));
      int i, j, dstep=(G->getdimx()-fdimx);
      for( j=0; j<fdimy; j++, dest+=dstep ) {
	 for( i=0; i<fdimx; i++, src++, dest++ ) {
	    if( dest >= end )
	      goto _abort_gputs;
	    if( *src ) {
	       *dest=c&*src;
	    }
	 }
      }
      dest-=(fdimy*G->getdimx()-fdimx);
      dest++;
   }
   
   _abort_gputs:
   return;
}

void gprintf( fontdev *f, int x, int y, int c, char *fmt, ... )
{
   char buf[4096]; // ?!

   va_list ap;
   va_start(ap,fmt);
   vsprintf(buf,fmt,ap);
   va_end(ap);

   gputs( f, x, y, c, buf );
}
