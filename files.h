
/********************************************
files.h
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the Awk programming language as defined in
Aho, Kernighan and Weinberger, The AWK Programming Language,
Addison-Wesley, 1988.

See the accompaning file, LIMITATIONS, for restrictions
regarding modification and redistribution of this
program in source or binary form.
********************************************/

/*$Log:	files.h,v $
 * Revision 2.1  91/04/08  08:23:07  brennan
 * VERSION 0.97
 * 
*/

#ifndef   FILES_H
#define   FILES_H

/* IO redirection types */
#define  F_IN           (-5)
#define  PIPE_IN        (-4)
#define  PIPE_OUT       (-3)
#define  F_APPEND       (-2)
#define  F_TRUNC        (-1)

extern char *shell ; /* for pipes and system() */

PTR  PROTO(file_find, (STRING *, int)) ;
int  PROTO(file_close, (STRING *)) ;
PTR  PROTO(get_pipe, (char *, int) ) ;
int  PROTO(wait_for, (int) ) ;
void  PROTO( close_out_pipes, (void) ) ;


#endif
