#ifndef COMP_H_RCSID
#define COMP_H_RCSID "$Id$"
/* Compress the source buffer src into the target buffer tgt. */
unsigned long memcompress (char *tgt, 
			   unsigned long tgtsize, 
			   char *src, 
			   unsigned long srcsize);
#endif /* COMP_H_RCSID */
