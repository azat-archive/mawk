
/********************************************
field.h
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the AWK programming language.

Mawk is distributed without warranty under the terms of
the GNU General Public License, version 2, 1991.
********************************************/

/* $Log:	field.h,v $
 * Revision 3.4.1.1  91/09/14  17:23:15  brennan
 * VERSION 1.0
 * 
 * Revision 3.4  91/08/13  06:51:20  brennan
 * VERSION .9994
 * 
 * Revision 3.3  91/07/17  15:10:51  brennan
 * moved space_split() proto to here
 * 
 * Revision 3.2  91/06/28  04:16:37  brennan
 * VERSION 0.999
 * 
 * Revision 3.1  91/06/07  10:27:24  brennan
 * VERSION 0.995
 * 
 * Revision 2.1  91/04/08  08:23:03  brennan
 * VERSION 0.97
 * 
*/

/* field.h */


#ifndef  FIELD_H
#define  FIELD_H   1

void  PROTO( set_field0, (char *, unsigned) ) ;
void  PROTO( split_field0, (void) ) ;
int   PROTO( space_split, (char *, unsigned) ) ;
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
