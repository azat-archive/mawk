
/********************************************
regexp.h
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the AWK programming language.

Mawk is distributed without warranty under the terms of
the GNU General Public License, version 2, 1991.
********************************************/

/*$Log:	regexp.h,v $
 * Revision 3.3.1.1  91/09/14  17:24:07  brennan
 * VERSION 1.0
 * 
 * Revision 3.3  91/08/13  06:52:01  brennan
 * VERSION .9994
 * 
 * Revision 3.2  91/06/28  04:17:28  brennan
 * VERSION 0.999
 * 
 * Revision 3.1  91/06/07  10:28:12  brennan
 * VERSION 0.995
 * 
 * Revision 2.1  91/04/08  08:23:47  brennan
 * VERSION 0.97
 * 
*/

#include <stdio.h>

PTR   PROTO( REcompile , (char *) ) ;
int   PROTO( REtest, (char *, PTR) ) ;
char *PROTO( REmatch, (char *, PTR, unsigned *) ) ;
void  PROTO( REmprint, (PTR , FILE*) ) ;

extern  int  REerrno ;
extern  char *REerrlist[] ;


