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

// Sat Dec 20 10:23:30 EET 1997 (proff@alf.melmac)
// created file gdev/hello.h

#ifndef __hello_h
#define __hello_h

#include <stdio.h>
#include <stdlib.h>
//#include <stdarg.h>
//#include <string.h>
#include <unistd.h>
#include <time.h>
#include <getopt.h>

#include "gdev-svgalib.h"
#include "fontutils.h"

void usage( void );
void wait( void );
void test( void );
int loadfonts( void );

#endif // __hello_h
