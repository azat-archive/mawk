
/********************************************
repl.h
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the AWK programming language.

Mawk is distributed without warranty under the terms of
the GNU General Public License, version 2, 1991.
********************************************/

/*$Log:	repl.h,v $
 * Revision 3.3.1.1  91/09/14  17:24:09  brennan
 * VERSION 1.0
 * 
 * Revision 3.3  91/08/13  06:52:03  brennan
 * VERSION .9994
 * 
 * Revision 3.2  91/06/28  04:17:30  brennan
 * VERSION 0.999
 * 
 * Revision 3.1  91/06/07  10:28:13  brennan
 * VERSION 0.995
 * 
 * Revision 2.1  91/04/08  08:23:49  brennan
 * VERSION 0.97
 * 
*/

/* repl.h */

#ifndef  REPL_H
#define  REPL_H

PTR  PROTO( re_compile, (STRING *) ) ;
char *PROTO( re_uncompile, (PTR) ) ;


CELL *PROTO( repl_compile, (STRING *) ) ;
char *PROTO( repl_uncompile, (CELL *) ) ;
void  PROTO( repl_destroy, (CELL *) ) ;
CELL *PROTO( replv_cpy, (CELL *, CELL *) ) ;
CELL *PROTO( replv_to_repl, (CELL *, STRING *) ) ;

#endif
