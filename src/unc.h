/*!!*****************************************************!!
  !! copyright (c) 1995 by Dan Hammer and Robert Lippman !!
  !!*****************************************************!!*/
#ifndef UNC_H_RCSID
#define UNC_H_RCSID "$Id: unc.h,v 1.1 1997/11/30 00:58:28 dan Exp $"
#ifdef __cplusplus
extern "C" 
{
#endif
  long unc_mem_to_mem(unsigned char *dst, long dst_size,
			   unsigned char *src, long src_size);
  long unc_file_to_mem(unsigned char *dst, long dst_size,
			    FILE *input, long from, long to);
  long unc_file_to_file(FILE *output,
			     FILE *input, long from, long to);
  int local_flush(int num_bytes);
  int local_nextbyte(void);
#ifdef __cplusplus
}
#endif
#endif /* UNC_H_RCSID */
