
/********************************************
memory.h
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the AWK programming language.

Mawk is distributed without warranty under the terms of
the GNU General Public License, version 2, 1991.
********************************************/


/* $Log:	memory.h,v $
 * Revision 3.3.1.1  91/09/14  17:23:51  brennan
 * VERSION 1.0
 * 
 * Revision 3.3  91/08/13  06:51:51  brennan
 * VERSION .9994
 * 
 * Revision 3.2  91/06/28  04:17:08  brennan
 * VERSION 0.999
 * 
 * Revision 3.1  91/06/07  10:27:59  brennan
 * VERSION 0.995
 * 
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
