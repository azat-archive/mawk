
/********************************************
init.h
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the AWK programming language.

Mawk is distributed without warranty under the terms of
the GNU General Public License, version 2, 1991.
********************************************/

/* $Log:	init.h,v $
 * Revision 3.3.1.1  91/09/14  17:23:31  brennan
 * VERSION 1.0
 * 
 * Revision 3.3  91/08/13  06:51:36  brennan
 * VERSION .9994
 * 
 * Revision 3.2  91/06/28  04:16:51  brennan
 * VERSION 0.999
 * 
 * Revision 3.1  91/06/07  10:27:41  brennan
 * VERSION 0.995
 * 
 * Revision 2.1  91/04/08  08:23:17  brennan
 * VERSION 0.97
 * 
*/

/* init.h  */


#ifndef  INIT_H
#define  INIT_H


void  PROTO( initialize, (int, char **) ) ;
void  PROTO( code_init, (void) ) ;
void  PROTO( code_cleanup, (void) ) ;
void  PROTO( compile_cleanup, (void) ) ;
void PROTO(scan_init, (int, char *) ) ;
void PROTO(scan_cleanup, (void) ) ;
void PROTO(bi_vars_init, (void) ) ;
void PROTO(bi_funct_init, (void) ) ;
void PROTO(print_init, (void) ) ;
void PROTO(kw_init, (void) ) ;
void PROTO(jmp_stacks_init, (void) ) ;
void PROTO(jmp_stacks_cleanup, (void) ) ;
void  PROTO( field_init, (void) ) ;
void  PROTO( fpe_init, (void) ) ;

#endif   /* INIT_H  */
