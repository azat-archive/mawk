
/********************************************
regexp.h
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the Awk programming language as defined in
Aho, Kernighan and Weinberger, The AWK Programming Language,
Addison-Wesley, 1988.

See the accompaning file, LIMITATIONS, for restrictions
regarding modification and redistribution of this
program in source or binary form.
********************************************/

/*$Log:	regexp.h,v $
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


