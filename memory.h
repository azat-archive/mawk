
/********************************************
memory.h
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the Awk programming language as defined in
Aho, Kernighan and Weinberger, The AWK Programming Language,
Addison-Wesley, 1988.

See the accompaning file, LIMITATIONS, for restrictions
regarding modification and redistribution of this
program in source or binary form.
********************************************/


/* $Log:	memory.h,v $
 * Revision 2.1  91/04/08  08:23:37  brennan
 * VERSION 0.97
 * 
*/


/*  memory.h  */

#ifndef  MEMORY_H
#define  MEMORY_H

#include "zmalloc.h"

#define  new_CELL()  (CELL *) zmalloc(sizeof(CELL))
#define  free_CELL(p)  zfree(p,sizeof(CELL))

#ifndef  SUPPRESS_NEW_STRING_PROTO
STRING  *PROTO( new_STRING, (char *, ...) ) ;
#endif

#ifdef   DEBUG
void  PROTO( DB_free_STRING , (STRING *) ) ;

#define  free_STRING(s)  DB_free_STRING(s)

#else

#define  free_STRING(sval)   if ( -- (sval)->ref_cnt == 0 )\
                                zfree(sval, (sval)->len+5) ; else
#endif


#endif   /* MEMORY_H */
