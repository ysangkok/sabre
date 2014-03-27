#ifndef INFLATE_H_RCSID
#define INFLATE_H_RCSID "$Id: inflate.h,v 1.1 1994/10/19 16:27:27 brian Exp $"

/* inflate.h for UnZip -- put in the public domain by Mark Adler
   version c14e, 29 September 1993 */


/* You can do whatever you like with this source file, though I would
   prefer that if you modify it and redistribute it that you include
   comments to that effect with your name and the date.  Thank you.

   History:
   vers    date          who           what
   ----  ---------  --------------  ------------------------------------
    c14  12 Mar 93  M. Adler        made inflate.c standalone with the
    				    introduction of inflate.h.
    c14d 28 Aug 93  G. Roelofs      replaced flush/FlushOutput with new version
    c14e 29 Sep 93  G. Roelofs      moved everything into unzip.h; added crypt.h
 */

#define STORED            0    /* compression methods */
#define SHRUNK            1
#define REDUCED1          2
#define REDUCED2          3
#define REDUCED3          4
#define REDUCED4          5
#define IMPLODED          6
#define TOKENIZED         7
#define DEFLATED          8
#define NUM_METHODS       9    /* index of last method + 1 */
#define memzero(dest,len)      memset(dest,0,len)

#ifdef WIN31
#define _RTLDLL
#endif

#include <stdio.h>
#include "unc.h"
#include "mytypes.h"

#define WSIZE 0x4000
#ifdef WIN31
extern uch *slide;
#else
extern uch slide[WSIZE];
#endif

extern int qflag;

#define NEXTBYTE local_nextbyte()
#define FLUSH(n) local_flush(n)

#ifdef __cplusplus
extern "C"
{
#endif
extern int inflate(void);
extern int inflate_free(void);
#ifdef __cplusplus
}
#endif
#endif /* INFLATE_H_RCSID */
