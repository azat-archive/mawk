
/********************************************
files.h
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the AWK programming language.

Mawk is distributed without warranty under the terms of
the GNU General Public License, version 2, 1991.
********************************************/

/*$Log:	files.h,v $
 * Revision 3.3.1.1  91/09/14  17:23:19  brennan
 * VERSION 1.0
 * 
 * Revision 3.3  91/08/13  06:51:26  brennan
 * VERSION .9994
 * 
 * Revision 3.2  91/06/28  04:16:41  brennan
 * VERSION 0.999
 * 
 * Revision 3.1  91/06/07  10:27:30  brennan
 * VERSION 0.995
 * 
 * Revision 2.2  91/05/22  07:40:58  brennan
 * fixed get_pipe prototype
 * 
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
PTR  PROTO(get_pipe, (char *, int, int *) ) ;
int  PROTO(wait_for, (int) ) ;
void  PROTO( close_out_pipes, (void) ) ;


#endif
