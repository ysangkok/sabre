#ifndef MYTYPES_H_RCSID
#define MYTYPES_H_RCSID "$Id: mytypes.h,v 1.1 1994/10/19 16:27:27 brian Exp $"

#ifdef MSDOS
#include <sys\types.h>
#else
#include <sys/types.h>
#endif

typedef unsigned char uch;
typedef unsigned short ush;
typedef unsigned long ulg;
#endif /* MYTYPES_H_RCSID */
