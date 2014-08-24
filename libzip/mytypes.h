#ifndef MYTYPES_H_RCSID
#define MYTYPES_H_RCSID "$Id: mytypes.h,v 1.1 1994/10/19 16:27:27 brian Exp $"

#include <stdint.h>
#ifdef MSDOS
#include <sys\types.h>
#else
#include <sys/types.h>
#endif

typedef unsigned char uch;
typedef unsigned short ush;
typedef uint32_t ulg;
#endif /* MYTYPES_H_RCSID */
