
/********************************************
zmalloc.h
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the Awk programming language as defined in
Aho, Kernighan and Weinberger, The AWK Programming Language,
Addison-Wesley, 1988.

See the accompaning file, LIMITATIONS, for restrictions
regarding modification and redistribution of this
program in source or binary form.
********************************************/

/*$Log:	zmalloc.h,v $
 * Revision 2.1  91/04/08  08:24:19  brennan
 * VERSION 0.97
 * 
*/

/* zmalloc.h */

#ifndef  ZMALLOC_H
#define  ZMALLOC_H

#ifdef   __STDC__
#include  <stdlib.h>
#include  <string.h>   /* memcpy() */

#else

PTR  memcpy(), malloc(), realloc() ;
void free() ;
#endif


PTR  PROTO( zmalloc, (unsigned) ) ;
void PROTO( zfree, (PTR, unsigned) ) ;
PTR  PROTO( zrealloc , (PTR,unsigned,unsigned) ) ;



#endif  /* ZMALLOC_H */
