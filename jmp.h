
/********************************************
jmp.h
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the AWK programming language.

Mawk is distributed without warranty under the terms of
the GNU General Public License, version 2, 1991.
********************************************/

/* $Log:	jmp.h,v $
 * Revision 3.3.1.1  91/09/14  17:23:35  brennan
 * VERSION 1.0
 * 
 * Revision 3.3  91/08/13  06:51:40  brennan
 * VERSION .9994
 * 
 * Revision 3.2  91/06/28  04:16:55  brennan
 * VERSION 0.999
 * 
 * Revision 3.1  91/06/07  10:27:45  brennan
 * VERSION 0.995
 * 
 * Revision 2.1  91/04/08  08:23:21  brennan
 * VERSION 0.97
 * 
*/

#ifndef   JMP_H
#define   JMP_H

void  PROTO(BC_new, (void) ) ;
void  PROTO(BC_insert, (int, INST*) ) ;
void  PROTO(BC_clear, (INST *, INST *) ) ;
void  PROTO(code_push, (INST *, unsigned) ) ;
unsigned  PROTO(code_pop, (INST *) ) ;
void  PROTO(code_jmp, (int, INST *, INST *) ) ;
void  PROTO(patch_jmp, (INST *) ) ;


#endif  /* JMP_H  */

