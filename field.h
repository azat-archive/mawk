
/********************************************
field.h
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the Awk programming language as defined in
Aho, Kernighan and Weinberger, The AWK Programming Language,
Addison-Wesley, 1988.

See the accompaning file, LIMITATIONS, for restrictions
regarding modification and redistribution of this
program in source or binary form.
********************************************/

/* $Log:	field.h,v $
 * Revision 2.1  91/04/08  08:23:03  brennan
 * VERSION 0.97
 * 
*/

/* field.h */


#ifndef  FIELD_H
#define  FIELD_H   1

void  PROTO( set_field0, (char *, unsigned) ) ;
void  PROTO( split_field0, (void) ) ;
int   PROTO( is_strnum, (char *, unsigned, double *) ) ;
void  PROTO( field_assign, (int, CELL *) ) ;
char *PROTO( is_string_split, (PTR , unsigned *) ) ;

#define   NF            (MAX_FIELD+1)
#define   RS            (MAX_FIELD+2)
#define   FS            (MAX_FIELD+3)
#define   OFMT          (MAX_FIELD+4)
#define   NUM_FIELDS    (MAX_FIELD+5)


extern  CELL  field[NUM_FIELDS] ;

extern  int  nf ; /* shadows NF */

/* a shadow type for RS and FS */
#define  SEP_SPACE      0
#define  SEP_CHAR       1
#define  SEP_STR        2
#define  SEP_RE         3

typedef  struct {
char  type ;
char  c ;
PTR ptr ; /* STRING* or RE machine* */
} SEPARATOR ;

extern   SEPARATOR  rs_shadow  ;
extern   CELL  fs_shadow ;






#endif   /* FIELD_H  */
