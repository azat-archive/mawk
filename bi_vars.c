
/********************************************
bi_vars.c
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the AWK programming language.

Mawk is distributed without warranty under the terms of
the GNU General Public License, version 2, 1991.
********************************************/

/* $Log:	bi_vars.c,v $
 * Revision 3.4.1.1  91/09/14  17:22:45  brennan
 * VERSION 1.0
 * 
 * Revision 3.4  91/08/13  06:50:52  brennan
 * VERSION .9994
 * 
 * Revision 3.3  91/06/29  09:46:53  brennan
 * Only track NR if needed
 * 
 * Revision 3.2  91/06/28  04:16:08  brennan
 * VERSION 0.999
 * 
 * Revision 3.1  91/06/07  10:26:56  brennan
 * VERSION 0.995
 * 
 * Revision 2.2  91/05/29  14:24:51  brennan
 * -V option for version
 * 
 * Revision 2.1  91/04/08  08:22:22  brennan
 * VERSION 0.97
 * 
*/


/* bi_vars.c */

#include "mawk.h"
#include "symtype.h"
#include "bi_vars.h"
#include "field.h"
#include "init.h"
#include "memory.h"

/* the builtin variables */
CELL  bi_vars[NUM_BI_VAR] ;

/* the order here must match the order in bi_vars.h */

static char *bi_var_names[NUM_BI_VAR] = {
"NR" ,
"FNR" ,
"ARGC" ,
"FILENAME" ,
"OFS" ,
"ORS" ,
"RLENGTH" ,
"RSTART" ,
"SUBSEP"
} ;

/* insert the builtin vars in the hash table */

void  bi_vars_init()
{ register int i ;
  register SYMTAB *s ;

  
  for ( i = 0 ; i < NUM_BI_VAR ; i++ )
  { s = insert( bi_var_names[i] ) ;
    s->type = i <= 1 ? ST_NR : ST_VAR ; 
    s->stval.cp = bi_vars + i ;
    /* bi_vars[i].type = 0 which is C_NOINIT */
  }

  /* set defaults */

  bi_vars[FILENAME].type = C_STRING ;
  bi_vars[FILENAME].ptr = (PTR) new_STRING( "" ) ; 

  bi_vars[ OFS ].type = C_STRING ;
  bi_vars[OFS].ptr = (PTR) new_STRING( " " ) ;
  
  bi_vars[ ORS ].type = C_STRING ;
  bi_vars[ORS].ptr = (PTR) new_STRING( "\n" ) ;

  bi_vars[ SUBSEP ].type = C_STRING ;
  bi_vars[SUBSEP].ptr =  (PTR) new_STRING( "\034" ) ;

  bi_vars[NR].type = bi_vars[FNR].type = C_DOUBLE ;
  /* dval is already 0.0 */

  cell_zero.type = C_DOUBLE ;
  cell_one.type = C_DOUBLE ;
  cell_one.dval = 1.0 ;
}

CELL cell_zero ;
CELL cell_one ;
