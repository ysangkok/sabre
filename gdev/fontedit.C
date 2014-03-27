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

// Mon Dec 22 20:43:18 EET 1997 (proff@alf.melmac)
// created file fontedit.C

// Current version:
// Last modified:

#include "fontedit.h"

// --
// -- main( options ) 
// --
// --

// standard vga mode
int screen_dimx=320, screen_dimy=200;
char *fontfn;
char *editfontpaths[]={
   "/usr/lib/kbd/consolefonts",
   "lib/fonts",
   "../lib/fonts",
   "fonts",
   "../fonts",
   "gdev/fonts",
   "../gdev/fonts",
   NULL
};

int main( int argc, char ** argv )
{
   fontpaths=editfontpaths;
   parseargs( argc, argv );
   edit( fontfn ); 
   G->close();
   fprintf( stderr, "\n" );
   return 0;
}

// --
// -- usage( ) 
// --
// --

void usage( void )
{
   fprintf( stderr, "usage: fontedit [options] <file>\n" 
	              );
   exit(1);
}

// --
// -- parseargs( args) 
// --

void parseargs( int argc, char **argv )
{
   int c;

   while( 1 ) {
      int option_index = 0;
      static struct option long_options[] =
      {
        {"help", 0, 0, '?'},
        {0, 0, 0, 0}
      };

      c = getopt_long( argc, argv, "?", long_options, &option_index );
      if( c == -1 )
	break;

      switch( c ) {
	 case '?':
	   usage();
          break;  
      } 
   }
   
   if( optind < argc ) {
      fontfn=argv[optind];
   } else
     usage();
}

fontdev *FONT=new fontdev;
fontdev *SYSFONT=new fontdev;

#define colormouse         23
#define colorborderdark    25
#define colorpixel         21 //33
#define colorbackground    27
#define colorborderbright  29

static int currentcell='A', previouscell='A';

static void drawborder( int x0, int y0, int x1, int y1, int invert=0 )
{
   int c1=invert?colorborderdark:colorborderbright,
       c2=invert?colorborderbright:colorborderdark;
   G->line(x0,y0,x1,y0,c1);
   G->line(x0,y0,x0,y1,c1);
   G->line(x0,y1,x1,y1,c2);
   G->line(x1,y0,x1,y1,c2);
}

static unsigned char buttlbl[]=
{24,27,26,25,'G',' ',' ','C','<','S','X','>'};

static int mx=0, my=0;
static int gridx,gridy;
static unsigned char mouseback[5*6];

static void hidemouse( void )
{
   unsigned char *dest=(unsigned char *)G->getvbuf( );
   unsigned char  *src=(unsigned char *)mouseback;
   dest+=(mx+my*G->getdimx());
   int i, j, dimx=SYSFONT->getdimx(), dimy=SYSFONT->getdimy();
   int dstep=(G->getdimx()-dimx);
   for( j=0; j<dimy; j++, dest+=dstep )
     for( i=0; i<dimx; i++ )
       *dest++=*src++;
   G->update(1,mx,my,mx+dimx,my+dimy);
}

static void savemouse( void )
{
   unsigned char  *src=(unsigned char *)G->getvbuf( );
   unsigned char *dest=(unsigned char *)mouseback;
   src+=(mx+my*G->getdimx());
   int i, j, dimx=SYSFONT->getdimx(), dimy=SYSFONT->getdimy();
   int dstep=(G->getdimx()-dimx);
   for( j=0; j<dimy; j++, src+=dstep )
     for( i=0; i<dimx; i++ )
       *dest++=*src++;
}

static void showmouse( void )
{
   gputc( SYSFONT, mx, my, colormouse, 128 );
   G->update(1,mx,my,mx+SYSFONT->getdimx(),my+SYSFONT->getdimy());
}

static void showcell( int ascii, int update=1 )
{
   int dx=10,dy=18, i=ascii%24, j=ascii/24;
   if( ascii == currentcell )	   
     G->rect( 1+i*dx, 1+j*dy, 8, 16, colorborderbright );
   else
     G->rect( 1+i*dx, 1+j*dy, 8, 16, colorborderdark );	   
   gputc( FONT, 1+i*dx, 1+j*dy, colorpixel, ascii );
   if( update )
     G->update(1,1+i*dx, 1+j*dy, 1+i*dx+8, 1+i*dy+16 );
}

static void showpixel( int x, int y, int update=1 )
{
   int dx=9,dy=9, x0=242, y0=0;
   unsigned char *src=(unsigned char *)FONT->getfbp(currentcell);
   src+=(y*8+x);
   G->rect( x0+x*dx+1, y0+y*dy+1, dx-1, dy-1, *src?colorpixel:colorborderdark );
   if( update )
   G->update(1,x0+x*dx+1, y0+y*dy+1, x0+(x+1)*dx, y0+(y+1)*dy );
}

static void showallpixels( int update=1 )
{
   int i, j;
   for( j=0; j<16; j++ ) {
      for( i=0; i<8; i++ ) {
	 showpixel( i, j, 0 );
      }
   }
   if( update )
     G->update();
}

static void showstatus( void )
{
   int dx=10,dy=18;
   G->rect( 3+16*dx, 3+10*dy+1, 8*dx-6, dy-6, colorbackground );
   gprintf( SYSFONT, 4+16*dx, 6+10*dy+1, colorpixel, "%3d(%02x) %dx%d", currentcell, currentcell, FONT->getmindimx(), FONT->getmindimy() );
   G->update(1,3+16*dx, 3+10*dy+1, 24*dx-3, 11*dy-3);
}

static void showall( void )
{
   int dx=10,dy=18;
   int x0=0,y0=0,x1=24*dx+1,y1=11*dy+1;
   // browse window
   G->rect(x0+1,y0+1,x1-x0,y1-y0, colorbackground );
   drawborder( x0, y0, x1, y1 );
   // title
   drawborder( 2+16*dx, 2+10*dy+1, 24*dx-2, 11*dy-2, 1 );
   showstatus();
   int i, j, ascii;
   for( ascii=0; ascii<256; ascii++ )
     showcell( ascii, 0 );
   dx=9,dy=9;
   x0=242,y0=0,x1=242+8*dx+1,y1=16*dy+1;
   // edit window
   G->rect(x0+1,y0+1,x1-x0-1,y1-y0-1, colorbackground );
   drawborder( x0, y0, x1, y1 );
   showallpixels(0);
   // buttons
   y0+=16*dy+2;
   for( j=0; j<3; j++ ) {
      for( i=0; i<4; i++ ) {
	 int X0=(int)(x0+i*18.7), Y0=(int)(y0+j*18);
	 int X1=(int)(x0+(i+1)*18.7)-1, Y1=(int)(y0+(j+1)*18)-1;
	 drawborder( X0, Y0, X1, Y1 );
	 G->rect( X0+1, Y0+1, X1-X0-1, Y1-Y0-1, colorbackground );
	 gputc( SYSFONT, X0+7, Y0+7, colorpixel, buttlbl[j*4+i] );
      }
   }
   savemouse();
   showmouse();
   G->update();
}

void edit( char *fn )
{
   if( SYSFONT->load("simple-5x6") )
     return;
   
   if( FONT->load(fn) )
     return;
   FONT->setcell( 8, 16 );

   assign_gdev_svgalib();
   assign_rgbdev8_svgalib();
   start_mouse_gdev_svgalib();
   if( G->open(screen_dimx,screen_dimy) ) {
      fprintf( stderr, "error opening gdev in %dx%dx8 resolution!\n", screen_dimx, screen_dimy );
      return;
   }

   showall();
   
   int looping=1;
   while( looping ) {
      gevent *ge=G->event();
      if( ge ) {
	 unsigned int type=ge->type;
	 if( (type&gevent::device_id) == gevent::device_serial_mouse ) {
	    gevent_data *gd=(gevent_data *)ge->data;
	    // mouse action
	    hidemouse();
	    mx=gd->x; my=gd->y;
	    savemouse();
	    if( (gd->flags&1) && (type&gevent::pressed)!=0) {
	       // button 1 activity
	       if( mx < 24*10 && (gd->flags&1) != 0 ) {
		  int ascii=(mx/10)+(my/18)*24;
		  if( ascii > 255 )
		    ascii =255;  
		  previouscell=currentcell;
		  currentcell=ascii;
		  showcell( previouscell );
		  showcell( currentcell );
		  showallpixels();
		  showstatus();
		  savemouse();
	       } else {
		  gridx=(mx-243)/9,gridy=my/9;
		  if( gridx > -1 && gridy > -1 && gridx < 8 && gridy < 16 ) {
		    // flip pixel
		     unsigned char *p=(unsigned char *)FONT->getfbp(currentcell);
		     p[gridy*8+gridx]^=0xff;
		     showcell(currentcell);
		     showpixel( gridx, gridy );
		     savemouse();
		  } else {
		     gridx=(int)((mx-243)/18.7); gridy=(my-146)/18;
		     switch( buttlbl[gridx+gridy*4] ) {
		      case 24:
			FONT->rotate( currentcell, fontdev::up );
			showallpixels();
			showcell( currentcell );
			break;
		      case 25:
			FONT->rotate( currentcell, fontdev::down );
			showallpixels();
			showcell( currentcell );
			break;
		      case 26:
			FONT->rotate( currentcell, fontdev::right );
			showallpixels();
			showcell( currentcell );
			break;
		      case 27:
			FONT->rotate( currentcell, fontdev::left );
			showallpixels();
			showcell( currentcell );
			break;
		      case 'S':
			FONT->save( "new-font" );
			FONT->setcell( 8, 16 );
			break;
		      case 'X':
			looping=0;
			break;
		      case '>':
			previouscell=currentcell;
			currentcell++;
			if( currentcell > 255 )
			  currentcell=0;
			showcell( previouscell );
			showcell( currentcell );
			showallpixels();
			showstatus();
			savemouse();
			break;
		      case '<':
			previouscell=currentcell;
			currentcell--;
			if( currentcell < 0 )
			  currentcell=255;
			showcell( previouscell );
			showcell( currentcell );
			showallpixels();
			showstatus();
			savemouse();
			break;
		      case 'G':
			FONT->calcmin();
			showstatus();
			break;
		      case 'C':
			memset( FONT->getfbp(currentcell), 0, 16*8 ); 
			showcell( currentcell );
			showallpixels();
			savemouse();
			break;
		     }
		  }
	       }
	    } // button #1 (left)
	    showmouse();
	 } else { // process mouseevents here
	    
	    // process keyevents here!
	 
	 } // process keyevents ends here
      } // got event ends here
   } // main loop ends here
}
