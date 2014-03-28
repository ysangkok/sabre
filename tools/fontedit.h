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

// Mon Dec 22 20:43:25 EET 1997 (proff@alf.melmac)
// created file fontedit.h

#ifndef __fontedit_h
#define __fontedit_h

#include <stdio.h>
#include <string.h>
#include <getopt.h>

#include "gdev-svgalib.h"
#include "fontutils.h"

void usage( void );
void parseargs( int argc, char **argv );
void edit( const char *fn );

#endif // __fontedit_h
