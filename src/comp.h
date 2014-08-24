/*!!*****************************************************!!
  !! copyright (c) 1995 by Dan Hammer and Robert Lippman !!
  !!*****************************************************!!*/
#ifndef COMP_H_RCSID
#define COMP_H_RCSID "$Id: comp.h,v 1.1 1997/11/30 00:58:28 dan Exp $"
/* Compress the source buffer src into the target buffer tgt. */
#ifdef __cplusplus
extern "C" {
#endif
uint32_t memcompress (char *tgt, 
			   uint32_t tgtsize, 
			   char *src, 
			   uint32_t srcsize);
#ifdef __cplusplus
}
#endif
#endif /* COMP_H_RCSID */
