/*

 Copyright (C) 1990-1993 Mark Adler, Richard B. Wales, Jean-loup Gailly,
 Kai Uwe Rommel and Igor Mandrichenko.
 Permission is granted to any individual or institution to use, copy, or
 redistribute this software so long as all of the original files are included,
 that it is not sold for profit, and that this copyright notice is retained.

*/

/*
 *  zip.h by Mark Adler.
 */


#define ZIP   /* for crypt.c:  include zip password functions, not unzip */

/* some standard includes */
#include <unistd.h>
#include <stdint.h>

/* Set up portability */
#include "tailor.h"

#define MIN_MATCH  3
#define MAX_MATCH  258
/* The minimum and maximum match lengths */

#define WSIZE ((unsigned)0x4000)
#ifndef WSIZE
#  define WSIZE  ((unsigned)32768)
#endif
/* Maximum window size = 32K. If you are really short of memory, compile
 * with a smaller WSIZE but this reduces the compression ratio for files
 * of size > WSIZE. WSIZE must be a power of two in the current implementation.
 */

#define MIN_LOOKAHEAD (MAX_MATCH+MIN_MATCH+1)
/* Minimum amount of lookahead, except at the end of the input file.
 * See deflate.c for comments about the MIN_MATCH+1.
 */

#define MAX_DIST  (WSIZE-MIN_LOOKAHEAD)
/* In order to simplify the code, particularly on 16 bit machines, match
 * distances are limited to MAX_DIST instead of WSIZE.
 */

/* Define malloc() and string functions */

/* Types centralized here for easy modification */
#define local static            /* More meaningful outside functions */
typedef uint8_t uch;      /* unsigned 8-bit value */
typedef uint16_t ush;     /* unsigned 16-bit value */
typedef uint32_t ulg;      /* unsigned 32-bit value */

/* crc.h needs typedef of ulg */
#include "crc.h"

/* Lengths of headers after signatures in bytes */
#define LOCHEAD 26
#define CENHEAD 42
#define ENDHEAD 18

/* Structures for in-memory file information */
struct zlist {
  /* See central header in zipfile.c for what vem..off are */
  ush vem, ver, flg, how;
  ulg tim, crc, siz, len;
  extent nam, ext, cext, com;   /* offset of ext must be >= LOCHEAD */
  ush dsk, att, lflg;           /* offset of lflg must be >= LOCHEAD */
  ulg atx, off;
  char *name;                   /* File name in zip file */
  char *extra;                  /* Extra field (set only if ext != 0) */
  char *cextra;                 /* Extra in central (set only if cext != 0) */
  char *comment;                /* Comment (set only if com != 0) */
  char *zname;                  /* Name for new zip file header */
  int mark;                     /* Marker for files to operate on */
  int trash;                    /* Marker for files to delete */
  int dosflag;                  /* Set to force MSDOS file attributes */
  struct zlist far *nxt;        /* Pointer to next header in list */
};
struct flist {
  char *name;                   /* Pointer to zero-delimited name */
  char *zname;                  /* Name used for zip file headers */
  int dosflag;                  /* Set to force MSDOS file attributes */
  struct flist far * far *lst;  /* Pointer to link pointing here */
  struct flist far *nxt;        /* Link to next name */
};
struct plist {
  char *zname;                  /* Name used for zip file headers */
  int select;                   /* Selection flag ('i' or 'x') */
};

/* internal file attribute */
#define UNKNOWN (-1)
#define BINARY  0
#define ASCII   1

/* Public globals */

#define BEST -1                 /* Use best method (deflation or store) */
#define STORE 0                 /* Store method */
#define DEFLATE 8               /* Deflation method*/

extern int Compression_method;	/* Restriction on compression method */
extern int Compression_verbose;	/* Report oddities in zip file structure */
extern int Compression_level;	/* Compression level */

/* Diagnostic functions */
#ifdef DEBUG
# ifdef MSDOS
#  undef  stderr
#  define stderr stdout
# endif
#  define diag(where) fprintf(stderr, "zip diagnostic: %s\n", where)
#  define Assert(cond,msg) {if(!(cond)) error(msg);}
#  define Trace(x) fprintf x
#  define Tracev(x) {if (verbose) fprintf x ;}
#  define Tracevv(x) {if (verbose>1) fprintf x ;}
#  define Tracec(c,x) {if (verbose && (c)) fprintf x ;}
#  define Tracecv(c,x) {if (verbose>1 && (c)) fprintf x ;}
#else
#  define diag(where)
#  define Assert(cond,msg)
#  define Trace(x)
#  define Tracev(x)
#  define Tracevv(x)
#  define Tracec(c,x)
#  define Tracecv(c,x)
#endif

#define error(message) { fprintf(stderr,"%s\n",message); exit(1); }

/* Public function prototypes */
        /* in deflate.c */
void lm_init OF((int pack_level, ush *flags));
void lm_free OF((void));
ulg  deflate OF((void));

        /* in trees.c */
void ct_init     OF((ush *attr, int *method));
int  ct_tally    OF((int dist, unsigned int lc));
ulg  ct_flush_block OF((uch far *buf, ulg stored_len, int eof));

        /* in bits.c */
void     bits_send_bits  OF((int value, unsigned int length));
unsigned bits_reverse OF((unsigned value, int length));
void     bits_windup  OF((void));
void     bits_copy_block OF((uch far *buf, unsigned len, int header));
int      bits_seekable   OF((void));
extern   unsigned int (*bits_read_buf) OF((uch *buf, unsigned size));
ulg     memcompress OF((uch *tgt, ulg tgtsize, uch *src, ulg srcsize));

/* end of zip.h */
