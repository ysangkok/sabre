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

// Mon Dec 22 16:05:04 EET 1997 (proff@alf.melmac)
// created file fontdev.C

// Current version: gdev-0.3.0 beta #14
// Last modified:

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include "fontdev.h"

static char *_locate( const char *fmt, ... )
{
   static char fn[4096]; // ?!

   va_list ap;
   va_start(ap,fmt);
   vsprintf(fn,fmt,ap);
   va_end(ap);

   if( access( fn, R_OK ) ) {
      perror( fn );
      return NULL;
   }
   return fn;
}

#define SYSFONTPATH "/usr/lib/kbd/consolefonts"

static const char *_fontpaths[] = {
   SYSFONTPATH,
   NULL
};

const char **fontpaths=_fontpaths;

static const char *locate( const char *fn )
{
   const char *_fn;     
   if( (_fn=_locate( fn )) == NULL ) {
      if( strchr( fn, '/' ) == NULL ) {
	 const char **v=fontpaths;
	 while( *v ) {
	    if( (_fn=_locate( "%s/%s", *v, fn )) != NULL )
	      break;
	    v++;
	 }
      }
   }
   return _fn;
}

fontdev::fontdev()
{
}

fontdev::~fontdev()
{
   if( fbp )
     delete fbp;
}

int fontdev::load( const char *fn )
{
   if( (fn=locate(fn) ) == NULL )
     return 1;
   FILE *fp=fopen( fn, "rb" );
   if( !fp ) {
      perror( fn );
      return 1;
   }
   unsigned char buf[4096];
   // (try to) read 256 8x16 cells
   size_t scans=fread( buf, 1, 4096, fp );
   fclose( fp );
   // analyze fontset
   if( scans > 2048 )
     dimy=16;
   else
     dimy=8;
   unsigned char *p=buf, mask=0;
   size_t i; int j;
   for( i=0; i<scans; i++, p++ ) {
      mask|=*p;
   }
   for( dimx=8; dimx>0; dimx--, mask>>=1 )
     if( mask & 1 )
       break;
   // locate font space
   dimy-=7;
   if( dimy<4 )
     dimy=4; // this is ridiculous
   for( ; dimy<16; dimy++ ) {
      p=buf+(dimy*32);
      for( j=0; j<dimy; j++, p++ ) {
	 if( *p )
	   break;
      }
      if( j == dimy )
	break;
   }
   fprintf( stderr, "found %dx%d font in %s\n", dimx, dimy, fn );
   mindimx=dimx; mindimy=dimy;
   // expand font to row-major bitmap
   fbp=new unsigned char[8*16*256];
   p=static_cast<unsigned char *>(fbp);
   memset( p, 0, 8*16*256 );
   unsigned char endmask=0x80>>dimx;
   for( i=0; i<scans; i++ ) {
      for( mask=0x80; mask>endmask; mask>>=1, p++ ) {
	 if( buf[i]&mask ) {
	    *p=0xff;  
	 }
      }
   }
   return 0;
}

int fontdev::save( const char *fn )
{
   FILE *fp=fopen( fn, "wb" );
   if( !fp ) {
      perror( fn );
      return 1;
   }
   calcmin();
   if( dimx!=mindimx||dimy!=mindimy )
     setcell( mindimx, mindimy );
   // there must be better way to do this loop!
   unsigned char *p=static_cast<unsigned char *>(fbp);
   int i, j, k, count=0;
   for( j=0; j<256; j++ ) { // for each character
      for( i=0; i<dimy; i++ ) { // for each scan
	 char c=0;
	 for( k=0; k<dimx; k++ ) { // for each bit
	    if( p[j*dimy*dimx+i*dimx+k] )
	      c|=(0x80>>k);
	 }
	 fputc( c, fp );
	 count++;
      }
   }

   int fill=4096-count;
   if( fill > 2047 )
     fill-=2048;
   if( fill )
     while( --fill )
       fputc( 0, fp );
   
   fclose( fp );
   return 0;
}

void *fontdev::getfbp( unsigned int c )
{
   unsigned char *p=static_cast<unsigned char *>(fbp);
   p+=c*static_cast<unsigned int>(dimx*dimy);
   return p;
}

void fontdev::calcmin( void )
{
   // analyze width & height
   int i, j, k, widthbuf[8], heightbuf[16];
   memset( widthbuf, 0, sizeof(widthbuf));
   memset( heightbuf, 0, sizeof(heightbuf));
   
   unsigned char *p=static_cast<unsigned char *>(fbp);
   for( j=0; j<256; j++ ) { // for each character
      for( i=0; i<dimy; i++ ) { // for each scan
	 for( k=0; k<dimx; k++ ) { // for each bit
	    if( p[j*dimy*dimx+i*dimx+k] ) {
	       widthbuf[k]++;
	       heightbuf[i]++;
	    }
	 }	 
      }      
   }

   for( i=dimy; i>0; i-- ) { // for each scan
      if( heightbuf[i-1] )
	break;
   }
   mindimy=i;
   for( i=dimx; i>0; i-- ) { // for each bit
      if( widthbuf[i-1] )
	break;
   }
   mindimx=i;
}
   
bool fontdev::setcell( int _dimx, int _dimy )
{
   if( dimx==_dimx && dimy==_dimy )
     return 0;
   unsigned char *newfbp=new unsigned char[_dimx*_dimy*256], *p=static_cast<unsigned char *>(fbp);
   memset( newfbp, 0, static_cast<size_t>(_dimx*_dimy*256) );
   // there must be better way to do this loop!
   int i, j, k;
   for( j=0; j<256; j++ ) { // for each character
      for( i=0; i<dimy; i++ ) { // for each scan
	 for( k=0; k<dimx; k++ ) { // for each bit
	    newfbp[j*_dimy*_dimx+i*_dimx+k]=p[j*dimy*dimx+i*dimx+k];
	 }
      }      
   }
   memcpy( fbp, newfbp, static_cast<size_t>(_dimx*_dimy*256) );
   delete[] newfbp;
   dimx=_dimx;
   dimy=_dimy;
   calcmin();
   return 0;
}

void fontdev::rotate( int what, int where )
{
   unsigned char *hold, *p;
   int i, j;
   switch( where ) {
    case up:
      hold=new unsigned char[dimx];
      p=static_cast<unsigned char *>(getfbp(static_cast<unsigned int>(what)));
      memcpy( hold, p, static_cast<size_t>(dimx) );
      memcpy( p, p+dimx, static_cast<size_t>((dimy-1)*dimx) );
      memcpy( p+(dimy-1)*dimx, hold, static_cast<size_t>(dimx) );
      delete[] hold;
      break;
    case right: 
      hold=new unsigned char[dimy];
      p=static_cast<unsigned char *>(getfbp(static_cast<unsigned int>(what)));
      for( i=0; i<dimy; i++ )
	hold[i]=p[(i+1)*dimx-1];
      for( j=0; j<dimy; j++, p+=dimx ) {
	 for( i=dimx-1; --i;)
	   p[i]=p[i-1];
	 *p=hold[j];
      }
      delete[] hold;
      break;
    case left: 
      hold=new unsigned char[dimy];
      p=static_cast<unsigned char *>(getfbp(static_cast<unsigned int>(what)));
      for( i=0; i<dimy; i++ )
	hold[i]=p[i*dimx];
      for( i=0; i<dimy; i++, p+=dimx ) {
	 memcpy( p, p+1, static_cast<unsigned int>(dimx-1) );
	 p[dimx-1]=hold[i];
      }
      delete[] hold;
      break;
    case down: 
      hold=new unsigned char[dimx];
      p=static_cast<unsigned char *>(getfbp(static_cast<unsigned int>(what)));
      memcpy( hold, p+(dimy-1)*dimx, static_cast<size_t>(dimx) );
      memcpy( p+dimx, p, static_cast<size_t>((dimy-1)*dimx) );
      memcpy( p, hold, static_cast<size_t>(dimx) );
      delete[] hold;
      break;
    case turn_right: 
      hold=new unsigned char[dimx*dimy];
      memset( hold, 0, static_cast<size_t>(dimx*dimy) );
      for( j=0; j<16; j++ ) {
	 if( j >= dimy )
	   break;
	 for( i=0; i<8; i++ ) {
	    if( i >= dimx )
	      break;
	 }
      }
      delete[] hold;
      break;
    case turn_left:
      break;
   }
}
