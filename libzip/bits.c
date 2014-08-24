/*

 Copyright (C) 1990-1993 Mark Adler, Richard B. Wales, Jean-loup Gailly,
 Kai Uwe Rommel and Igor Mandrichenko.
 Permission is granted to any individual or institution to use, copy, or
 redistribute this software so long as all of the original files are included,
 that it is not sold for profit, and that this copyright notice is retained.

*/

/*
 *  bits.c by Jean-loup Gailly and Kai Uwe Rommel.
 *
 *  This is a new version of im_bits.c originally written by Richard B. Wales
 *
 *  PURPOSE
 *
 *      Output variable-length bit strings. Compression can be done
 *      to a file or to memory.
 *
 *  DISCUSSION
 *
 *      The PKZIP "deflate" file format interprets compressed file data
 *      as a sequence of bits.  Multi-bit strings in the file may cross
 *      byte boundaries without restriction.
 *
 *      The first bit of each byte is the low-order bit.
 *
 *      The routines in this file allow a variable-length bit value to
 *      be output right-to-left (useful for literal values). For
 *      left-to-right output (useful for code strings from the tree routines),
 *      the bits must have been reversed first with bits_reverse().
 *
 *      For in-memory compression, the compressed bit stream goes directly
 *      into the requested output buffer. The input data is read in blocks
 *      by the mem_read() function. The buffer is limited to 64K on 16 bit
 *      machines.
 *
 *  INTERFACE
 *
 *      void bits_init (FILE *zipfile)
 *          Initialize the bit string routines.
 *
 *      void bits_send_bits (int value, int length)
 *          Write out a bit string, taking the source bits right to
 *          left.
 *
 *      int bits_reverse (int value, int length)
 *          Reverse the bits of a bit string, taking the source bits left to
 *          right and emitting them right to left.
 *
 *      void bits_windup (void)
 *          Write out any remaining bits in an incomplete byte.
 *
 *      void bits_copy_block(char far *buf, unsigned len, int header)
 *          Copy a stored block to the zip file, storing first the length and
 *          its one's complement if requested.
 *
 *      int seekable(void)
 *          Return true if the zip file can be seeked.
 *
 *      ulg memcompress (char *tgt, ulg tgtsize, char *src, ulg srcsize);
 *          Compress the source buffer src into the target buffer tgt.
 */

#include <string.h>
#include "zip.h"

/* FUNCTION PROTOTYPES (created by emacs, do not edit) */
local void bits_init (FILE *zipfile);
void bits_send_bits(int value, unsigned int length);
unsigned bits_reverse(unsigned int code, int len);
void bits_windup(void);
void bits_copy_block(uch *buf, unsigned int len, int header);
int bits_seekable(void);
ulg memcompress(uch *tgt, ulg tgtsize, uch *src, ulg srcsize);
local void flush_outbuf(unsigned int w, unsigned int bytes);
local unsigned int mem_read(uch *b, unsigned int bsize);
/* END FUNCTION PROTOTYPES (created by emacs, do not edit) */

extern ulg deflate_window_size; /* size of sliding window */
extern uch file_outbuf[];

/* ===========================================================================
 * Globals (Yuck!)
 */
int Compression_level = 6;
int Compression_verbose = 0;

/* ===========================================================================
 * Local data used by the "bit string" routines.
 */

local FILE *zfile; /* output zip file */

local unsigned short bi_buf;
/* Output buffer. bits are inserted starting at the bottom (least significant
 * bits).
 */

#define Buf_size (8 * 2*sizeof(char))
/* Number of bits used within bi_buf. (bi_buf might be implemented on
 * more than 16 bits on some systems.)
 */

local unsigned int bi_valid;
/* Number of valid bits in bi_buf.  All bits above the last valid bit
 * are always zero.
 */

uch file_outbuf[1024];
/* Output buffer for compression to file */

local uch *in_buf, *out_buf;
/* Current input and output buffers. in_buf is used only for in-memory
 * compression.
 */

local unsigned in_offset, out_offset;
/* Current offset in input and output buffers. in_offset is used only for
 * in-memory compression. On 16 bit machiens, the buffer is limited to 64K.
 */

local unsigned in_size, out_size;
/* Size of current input and output buffers */

unsigned int (*bits_read_buf) OF((uch *buf, unsigned size));
/* Current input function. Set to mem_read for in-memory compression */

#ifdef DEBUG
ulg bits_sent;   /* bit length of the compressed data */
#endif

/* Output a 16 bit value to the bit stream, lower (oldest) byte first */
#define PUTSHORT(w) \
{ if (out_offset < out_size-1) { \
    out_buf[out_offset++] = (uch) ((w) & 0xff); \
    out_buf[out_offset++] = ((ush)(w) >> 8); \
  } else { \
    flush_outbuf((w),2); \
  } \
}

#define PUTBYTE(b) \
{ if (out_offset < out_size) { \
    out_buf[out_offset++] = (uch) (b); \
  } else { \
    flush_outbuf((b),1); \
  } \
}

/* ===========================================================================
 * Initialize the bit string routines.
 */
local void bits_init (FILE *zipfile)
{
    zfile  = zipfile;
    bi_buf = 0;
    bi_valid = 0;
#ifdef DEBUG
    bits_sent = 0L;
#endif

    /* Set the defaults for file compression. They are set by memcompress
     * for in-memory compression.
     */
    if (zfile != NULL) {
        out_buf = file_outbuf;
        out_size = sizeof(file_outbuf);
        out_offset = 0;
        bits_read_buf  = mem_read;
    }
}

/* ===========================================================================
 * Send a value on a given number of bits.
 * IN assertion: length <= 16 and value fits in length bits.
 */
void bits_send_bits(int value, unsigned int length)
{
#ifdef DEBUG
    Tracevv((stderr," l %2d v %4x ", length, value));
    Assert(length > 0 && length <= 15, "invalid length");
    bits_sent += (ulg)length;
#endif
    /* If not enough room in bi_buf, use (valid) bits from bi_buf and
     * (16 - bi_valid) bits from value, leaving (width - (16-bi_valid))
     * unused bits in value.
     */
    if (bi_valid > (int)Buf_size - length) {
        bi_buf |= (value << bi_valid);
        PUTSHORT(bi_buf);
        bi_buf = (ush)value >> (Buf_size - bi_valid);
        bi_valid += length - Buf_size;
    } else {
        bi_buf |= value << bi_valid;
        bi_valid += length;
    }
}

/* ===========================================================================
 * Reverse the first len bits of a code, using straightforward code (a faster
 * method would use a table)
 * IN assertion: 1 <= len <= 15
 */
unsigned bits_reverse(unsigned int code, int len)
{
    register unsigned res = 0;
    do {
        res |= code & 1;
        code >>= 1, res <<= 1;
    } while (--len > 0);
    return res >> 1;
}

/* ===========================================================================
 * Write out any remaining bits in an incomplete byte.
 */
void bits_windup(void)
{
    if (bi_valid > 8) {
        PUTSHORT(bi_buf);
    } else if (bi_valid > 0) {
        PUTBYTE(bi_buf);
    }
    if (zfile != NULL) {
        flush_outbuf(0, 0);
    }
    bi_buf = 0;
    bi_valid = 0;
#ifdef DEBUG
    bits_sent = (bits_sent+7) & ~7;
#endif
}

/* ===========================================================================
 * Copy a stored block to the zip file, storing first the length and its
 * one's complement if requested.
 */
void bits_copy_block(uch *buf, unsigned int len, int header)
{
    bits_windup();              /* align on byte boundary */

    if (header) {
        PUTSHORT((ush)len);   
        PUTSHORT((ush)~len);
#ifdef DEBUG
        bits_sent += 2*16;
#endif
    }
    if (zfile) {
        flush_outbuf(0, 0);
        fwrite(buf, 1, len, zfile);
        if (ferror(zfile)) error ("write error on zip file");
    } else if (out_offset + len > out_size) {
        error("output buffer too small for in-memory compression");
    } else {
        memcpy(out_buf + out_offset, buf, len);
        out_offset += len;
    }
#ifdef DEBUG
    bits_sent += (ulg)len<<3;
#endif
}


/* ===========================================================================
 * Return true if the zip file can be seeked. This is used to check if
 * the local header can be re-rewritten. This function always returns
 * true for in-memory compression.
 * IN assertion: the local header has already been written (ftell() > 0).
 */
int bits_seekable(void)
{
    return (zfile == NULL ||
            (fseek(zfile, -1L, SEEK_CUR) == 0 &&
             fseek(zfile,  1L, SEEK_CUR) == 0));
}    

/* ===========================================================================
 * In-memory compression. This version can be used only if the entire input
 * fits in one memory buffer. The compression is then done in a single
 * call of memcompress(). (An extension to allow repeated calls would be
 * possible but is not needed here.)
 * The first two bytes of the compressed output are set to a short with the
 * method used (DEFLATE or STORE). The following four bytes contain the CRC.
 * The values are stored in little-endian order on all machines.
 * This function returns the byte size of the compressed output, including
 * the first six bytes (method and crc).
 */

ulg memcompress(uch *tgt, ulg tgtsize, uch *src, ulg srcsize)
                           /* target and source buffers */
                           /* target and source sizes */
{
    ush att      = (ush)UNKNOWN;
    ush flags    = 0;
    ulg crc      = 0;
    int method   = DEFLATE;

    if (tgtsize <= 6L) error("target buffer too small");

    crc = updcrc(NULL, 0);
    crc = updcrc(src, (extent) srcsize);

    bits_read_buf = mem_read;
    in_buf 	  = src;
    in_size 	  = (unsigned)srcsize;
    in_offset 	  = 0;

    out_buf    = tgt;
    out_size   = (unsigned)tgtsize;
    out_offset = 2 + 4;
    deflate_window_size = 0L;

    bits_init((FILE *)NULL);
    ct_init(&att, &method);
    lm_init((Compression_level != 0 ? Compression_level : 1), &flags);
    deflate();
    deflate_window_size = 0L; /* was updated by lm_init() */

    /* For portability, force little-endian order on all machines: */
    tgt[0] = (uch)(method & 0xff);
    tgt[1] = (uch)((method >> 8) & 0xff);
    tgt[2] = (uch)(crc & 0xff);
    tgt[3] = (uch)((crc >> 8) & 0xff);
    tgt[4] = (uch)((crc >> 16) & 0xff);
    tgt[5] = (uch)((crc >> 24) & 0xff);

    return (ulg)out_offset;
}

/* ===========================================================================
 * Flush the current output buffer.
 */
local void flush_outbuf(unsigned int w, unsigned int bytes)
                    /* value to flush */
                    /* number of bytes to flush (0, 1 or 2) */
{
    if (zfile == NULL) {
        error("output buffer too small for in-memory compression");
    }
    /* Encrypt and write the output buffer: */
    if (out_offset != 0) {
        fwrite(out_buf, 1, (extent)out_offset, zfile);
        if (ferror(zfile)) error ("write error on zip file");
    }
    out_offset = 0;
    if (bytes == 2) {
        PUTSHORT(w);
    } else if (bytes == 1) {
        out_buf[out_offset++] = (uch) (w & 0xff);
    }
}

/* ===========================================================================
 * In-memory read function. As opposed to file_read(), this function
 * does not perform end-of-line translation, and does not update the
 * crc and input size.
 *    Note that the size of the entire input buffer is an uint32_t,
 * but the size used in mem_read() is only an unsigned int. This makes a
 * difference on 16 bit machines. mem_read() may be called several
 * times for an in-memory compression.
 */
local unsigned int mem_read(uch *b, unsigned int bsize)
{
    if (in_offset < in_size) {
        ulg block_size = in_size - in_offset;
        if (block_size > (ulg)bsize) block_size = (ulg)bsize;
        memcpy(b, in_buf + in_offset, (unsigned)block_size);
        in_offset += (unsigned)block_size;
        return block_size;
    } else {
        return 0; /* end of input */
    }
}
