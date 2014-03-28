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

// Wed Dec 17 19:46:56 EET 1997 (proff@alf.melmac)
// created file vp.C

// Current version:
// Last Modified:

#include "hello.h"

#include <cmath>

// --
// -- main( options ) 
// --
// --

int main( int argc, char ** argv )
{
   int dimx=320, dimy=200;
   int c;

   while( 1 ) {
      int option_index = 0;
      static struct option long_options[] =
      {
        {"dimx", 1, 0, 'X'},
        {"dimy", 1, 0, 'Y'},
        {"help", 1, 0, '?'},
        {0, 0, 0, 0}
      };

      c = getopt_long( argc, argv, "?", long_options, &option_index );
      if( c == -1 )
	break;

      switch( c ) {
        case 'X':
	   dimx=atoi(optarg);
          break;
        case 'Y':
	   dimy=atoi(optarg);
          break;
	 case '?':
	   usage();
          break;  
      } 
   }
   
   if( optind < argc ) {
       usage();  
   }

   fprintf( stderr, "hello %dx%d\n", dimx, dimy );
   if( loadfonts( ) )
       return 1;
   
   assign_gdev_svgalib();
   assign_rgbdev8_svgalib();
   if( G->open(dimx,dimy) ) {
      fprintf( stderr, "error opening gdev in %dx%dx8 resolution!\n", dimx, dimy );
      return 1;
   }

   // full device
   test();
   
   // "1/4 device"
   G->setview( dimx/4, dimy/4, dimx-dimx/4, dimy-dimy/4 );
   test();
   
   G->close();
   return 0;
}

// --
// -- usage( ) 
// --
// --

void usage( void )
{
   fprintf( stderr, "usage: hello [options]\n" 
	            "options:\n"
	            "          --dimx <value>\n"
	            "          --dimy <value>\n"
	            "          --font-file <file>\n"
	            "\n"
	              );
   exit(1);
}

// --
// -- test( ) 
// --
// -- runs sequence of tests in previously specified viewport
// -- no parameters are used nor accepted
// --

static float ffactor[64];
static unsigned char mypal[768]; 
static void fade( int out=1 )
{
   int i, j, beg, end, step;
   //(works only for 8bpp vicinity)
   unsigned char fadepal[768];

   if( out ) {
      // fade away..
      beg=63;
      end=-1;
      step=-1;
   } else {
      // fade in..
      beg=0;
      end=64;
      step=1;
   }

   for( j=beg; j!=end; j+=step ) { 
      unsigned char *p=fadepal, *mp=mypal;
      for( i=0; i<768; i++, p++, mp++ ) {
	 *p=(char)(j*ffactor[*mp]);
      }
      RGB8->set( fadepal );
   }

   if( !out )
     RGB8->set( mypal );   
}

fontdev *FONT=new fontdev;
fontdev *BROADWAYFONT=new fontdev;
fontdev *SMALLERFONT=new fontdev;
fontdev *MEDIEVALFONT=new fontdev;

void test( void )
{
   int i;
   ffactor[0]=0.0;
   for( i=1; i<64; i++ )
     ffactor[i]=(i*1.0)/64.0;
   
   RGB8->get( mypal );

   //
   // Input test
   //
   
   fade();
   G->clear( );
   i=0;
   gprintf( FONT, 0, 0,     1, "Press something..." ); 
   i+=FONT->getdimy()+1;
   gprintf( FONT, 0, i, 1, "(to be continued)" ); 
   i+=BROADWAYFONT->getdimy()+1;
   gprintf( BROADWAYFONT, 0, i, 1, "HELLO WORLD" ); 
   i+=SMALLERFONT->getdimy()+1;
   gprintf( SMALLERFONT, 0, i, 1, "hello world..." ); 
   i+=MEDIEVALFONT->getdimy()+1;
   gprintf( MEDIEVALFONT, 0, i, 1, "Hello World?" ); 
   G->update();
   fade(0);
   wait();
   
   fade();
   G->clear( );
   int dx=G->getdimx(), dy=G->getdimy();
      
   //
   // line test: print a "star"
   //
   int radius=dx>dy?dy/2:dx/2;
   for( i=0; i<32; i++ ) {
      float angle=(i/32.0)*2*3.14159;
      int ddx=(int)(sin(angle)*radius);
      int ddy=(int)(cos(angle)*radius);
      G->line( dx/2, dy/2, dx/2+ddx, dy/2+ddy, 32+i );
   }

   G->update();
   fade(0);
   sleep(1);
   fade();

   //
   // rectangle test
   //
   for( i=15; i>=0; i-- )
     G->rect( 0, 0, (int)(i/15.0*dx), (int)(i/15.0*dy), i );
   G->update();
   fade(0);
   sleep(1);
   fade();
   G->clear();
   G->update();
   fade(0);
   
   //
   // vbuf test: user framebuffer manipulation support
   //
   unsigned char pattern[225];
   for( i=0; i<224; i++ )
     pattern[i]=i+32;
   pattern[i]=0;
   unsigned char *start_pattern=pattern;
   time_t end=clock()+CLOCKS_PER_SEC;
   int count=0;
   while( clock() < end ) {
      count++;
      start_pattern++;
      if( !*pattern )
	start_pattern=pattern;
      unsigned char *dest=(unsigned char *)G->getvbuf(), *src=start_pattern;
      for( i=0; i<dx*dy; i++, dest++, src++ ) {
	 if( !*src )
	   src=pattern;
	 *dest=*src;
      }
      G->update();
   }
   fade();
   G->clear();
   G->update();
   fade(0);
}

// --
// -- loadfont( fn ) 
// --
// --


const char *myfontpaths[] = {
   "fonts",
   "../fonts",
   "lib/fonts",
   "../lib/fonts",
   NULL
};

int loadfonts( void )
{
   fontpaths=myfontpaths;
   if( FONT->load( "simple-5x6" ) )
     return 1;
   if( BROADWAYFONT->load( "broadway-16" ) )
     return 1;
   if( SMALLERFONT->load( "alt-8x14" ) )
     return 1;
   if( MEDIEVALFONT->load( "medieval-16" ) )
     return 1;
   return 0;
}

void wait( void )
{
   char looping=1;
   while( looping ) {
      gevent *g= G->event(1);
      if( g )
	if( 0 != (g->type&gevent::pressed) )
	  looping=0;
   }
}
