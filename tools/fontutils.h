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

// Mon Dec 29 16:40:06 EET 1997 (proff@alf.melmac)
// created file fontutils.h

#ifndef __fontutils_h
#define __fontutils_h

#include "fontdev.h"

void gputc( fontdev *f, int x, int y, int c, int ascii );
void gputs( fontdev *f, int x, int y, int c, const char *s );
void gprintf( fontdev *f, int x, int y, int c, const char *fmt, ... );

#endif // __fontutils_h
