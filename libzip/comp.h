#ifndef COMP_H_RCSID
#define COMP_H_RCSID "$Id$"
/* Compress the source buffer src into the target buffer tgt. */
uint32_t memcompress (char *tgt, 
			   uint32_t tgtsize, 
			   char *src, 
			   uint32_t srcsize);
#endif /* COMP_H_RCSID */
