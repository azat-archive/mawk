
/********************************************
zmalloc.h
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the AWK programming language.

Mawk is distributed without warranty under the terms of
the GNU General Public License, version 2, 1991.
********************************************/

/*$Log:	zmalloc.h,v $
 * Revision 3.3.1.1  91/09/14  17:24:33  brennan
 * VERSION 1.0
 * 
 * Revision 3.3  91/08/13  06:52:21  brennan
 * VERSION .9994
 * 
 * Revision 3.2  91/06/28  04:17:49  brennan
 * VERSION 0.999
 * 
 * Revision 3.1  91/06/07  10:28:32  brennan
 * VERSION 0.995
 * 
 * Revision 2.3  91/05/28  09:05:24  brennan
 * removed main_buff
 * 
 * Revision 2.2  91/05/22  07:48:33  brennan
 * removed __STDC__
 * 
 * Revision 2.1  91/04/08  08:24:19  brennan
 * VERSION 0.97
 * 
*/

/* zmalloc.h */

#ifndef  ZMALLOC_H
#define  ZMALLOC_H

#if ! HAVE_STDLIB_H
char *malloc() , *realloc() ;
void free() ;
#endif


PTR  PROTO( zmalloc, (unsigned) ) ;
void PROTO( zfree, (PTR, unsigned) ) ;
PTR  PROTO( zrealloc , (PTR,unsigned,unsigned) ) ;



#endif  /* ZMALLOC_H */
