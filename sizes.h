
/********************************************
sizes.h
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the AWK programming language.

Mawk is distributed without warranty under the terms of
the GNU General Public License, version 2, 1991.
********************************************/

/* $Log:	sizes.h,v $
 * Revision 3.5.1.1  91/09/14  17:24:19  brennan
 * VERSION 1.0
 * 
 * Revision 3.5  91/08/13  06:52:10  brennan
 * VERSION .9994
 * 
 * Revision 3.4  91/06/28  04:17:38  brennan
 * VERSION 0.999
 * 
 * Revision 3.3  91/06/27  08:16:51  brennan
 * different page size of the main code 
 * bigger loop stacks -- the changes were need to run AWF
 * 
 * Revision 3.2  91/06/13  07:17:05  brennan
 * make sure MAX_SPLIT >= MAX_FIELD
 * 
 * Revision 3.1  91/06/07  10:28:21  brennan
 * VERSION 0.995
 * 
 * Revision 2.4  91/05/30  09:05:09  brennan
 * input buffer can grow dynamically
 * 
 * Revision 2.3  91/05/28  09:05:16  brennan
 * removed main_buff
 * 
 * Revision 2.2  91/05/16  12:20:24  brennan
 * cleanup of machine dependencies
 * 
 * Revision 2.1  91/04/08  08:24:09  brennan
 * VERSION 0.97
 * 
*/

/*  sizes.h  */

#ifndef  SIZES_H
#define  SIZES_H

#if	! HAVE_SMALL_MEMORY
#define EVAL_STACK_SIZE  256  /* limit on recursion */
#define MAX_SPLIT	 256  /* maximum pieces from a split */
#define MAX_FIELD	 256  /* maximum number of fields */

#else  /* have to be frugal with memory */

#define EVAL_STACK_SIZE   64
#define MAX_SPLIT	 200
#define MAX_FIELD	 100

#endif  

/* make sure MAX_SPLIT >= MAX_FIELD  */
#if    MAX_SPLIT  <  MAX_FIELD
#undef MAX_SPLIT
#define MAX_SPLIT	MAX_FIELD
#endif

#define SPRINTF_SZ	(MAX_SPLIT*sizeof(PTR))

#define  BUFFSZ		4096
  /* starting buffer size for input files, grows if 
     necessary */

#define  HASH_PRIME  53
#define  A_HASH_PRIME 37


#define  MAX_COMPILE_ERRORS  5 /* quit if more than 4 errors */


#define  MAX_LOOP_DEPTH   40
/* should never be exceeded, doesn't matter if its too
   big (unless gross) because resources sized by it are freed */

/* AWF showed the need for this */
#define  MAIN_PAGE_SZ    4096 /* max instr in main block */
#define  PAGE_SZ    1024  /* max instructions for other blocks */

#endif   /* SIZES_H */
