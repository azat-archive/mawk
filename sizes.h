
/********************************************
sizes.h
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the Awk programming language as defined in
Aho, Kernighan and Weinberger, The AWK Programming Language,
Addison-Wesley, 1988.

See the accompaning file, LIMITATIONS, for restrictions
regarding modification and redistribution of this
program in source or binary form.
********************************************/

/* $Log:	sizes.h,v $
 * Revision 2.1  91/04/08  08:24:09  brennan
 * VERSION 0.97
 * 
*/

/*  sizes.h  */

#ifndef  SIZES_H
#define  SIZES_H

#define  HASH_PRIME  53
#define  A_HASH_PRIME 37


#if      SMALL_EVAL_STACK
/* allow some put not a lot of recursion */
#define  EVAL_STACK_SIZE  64
#else
#define  EVAL_STACK_SIZE  256
#endif

#define  MAX_COMPILE_ERRORS  5 /* quit if more than 4 errors */

#define  BUFFSZ    4096   /* input buffer size */

#define  MAX_LOOP_DEPTH   20
/* should never be exceeded, doesn't matter if its too
   big (unless gross) because resources sized by it are freed */

#define  MAX_FIELD   100  /* biggest field number */
#define  SPRINTF_SZ   300  /* biggest sprintf string length */

/* the size of the temp buffer in front of main_buff */
#define  PTR_SZ   sizeof(PTR)
#define  TEMP_BUFF_SZ  (MAX_FIELD*PTR_SZ > SPRINTF_SZ ?\
        MAX_FIELD*PTR_SZ : SPRINTF_SZ )

#define  PAGE_SZ    1024  /* max instructions for a block */

#endif   /* SIZES_H */
