
/********************************************
fin.h
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the Awk programming language as defined in
Aho, Kernighan and Weinberger, The AWK Programming Language,
Addison-Wesley, 1988.

See the accompaning file, LIMITATIONS, for restrictions
regarding modification and redistribution of this
program in source or binary form.
********************************************/

/*$Log:	fin.h,v $
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
short flags ;
}  FIN ;

#define  MAIN_FLAG    1   /* part of main input stream if on */
#define  EOF_FLAG     2

FIN *  PROTO (FINdopen, (int, int) );
FIN *  PROTO (FINopen, (char *, int) );
void   PROTO (FINclose, (FIN *) ) ;
char*  PROTO (FINgets, (FIN *, unsigned *) ) ;
unsigned PROTO ( fillbuff, (int, char *, unsigned) ) ;


extern  FIN  *main_fin ;  /* for the main input stream */
int   PROTO( open_main, (void) ) ;
#endif  /* FIN_H */
