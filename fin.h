
/********************************************
fin.h
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the AWK programming language.

Mawk is distributed without warranty under the terms of
the GNU General Public License, version 2, 1991.
********************************************/

/*$Log:	fin.h,v $
 * Revision 3.3.1.1  91/09/14  17:23:24  brennan
 * VERSION 1.0
 * 
 * Revision 3.3  91/08/13  06:51:31  brennan
 * VERSION .9994
 * 
 * Revision 3.2  91/06/28  04:16:46  brennan
 * VERSION 0.999
 * 
 * Revision 3.1  91/06/08  06:14:35  brennan
 * VERSION 0.995
 * 
 * Revision 2.3  91/06/08  06:00:21  brennan
 * changed how eof is marked on main_fin
 * 
 * Revision 2.2  91/05/30  09:04:47  brennan
 * input buffer can grow dynamically
 * 
 * Revision 2.1  91/04/08  08:23:11  brennan
 * VERSION 0.97
 * 
*/

/* fin.h */

#ifndef  FIN_H
#define  FIN_H
/* structure to control input files */

typedef struct {
int  fd ;
FILE *fp ;   /* NULL unless interactive */
char *buff ;
char *buffp ;
unsigned nbuffs ; /* sizeof *buff in BUFFSZs */
int  flags ;
}  FIN ;

#define  MAIN_FLAG    1   /* part of main input stream if on */
#define  EOF_FLAG     2

FIN *  PROTO (FINdopen, (int, int) );
FIN *  PROTO (FINopen, (char *, int) );
void   PROTO (FINclose, (FIN *) ) ;
char*  PROTO (FINgets, (FIN *, unsigned *) ) ;
unsigned PROTO ( fillbuff, (int, char *, unsigned) ) ;


extern  FIN  *main_fin ;  /* for the main input stream */
void   PROTO( open_main, (void) ) ;
#endif  /* FIN_H */
