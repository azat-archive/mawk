
/********************************************
jmp.h
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the Awk programming language as defined in
Aho, Kernighan and Weinberger, The AWK Programming Language,
Addison-Wesley, 1988.

See the accompaning file, LIMITATIONS, for restrictions
regarding modification and redistribution of this
program in source or binary form.
********************************************/

/* $Log:	jmp.h,v $
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
void  PROTO(code_jmp, (int, INST *) ) ;
void  PROTO(patch_jmp, (INST *) ) ;


#endif  /* JMP_H  */

