
/********************************************
init.h
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the Awk programming language as defined in
Aho, Kernighan and Weinberger, The AWK Programming Language,
Addison-Wesley, 1988.

See the accompaning file, LIMITATIONS, for restrictions
regarding modification and redistribution of this
program in source or binary form.
********************************************/

/* $Log:	init.h,v $
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
