
/********************************************
bi_vars.c
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the Awk programming language as defined in
Aho, Kernighan and Weinberger, The AWK Programming Language,
Addison-Wesley, 1988.

See the accompaning file, LIMITATIONS, for restrictions
regarding modification and redistribution of this
program in source or binary form.
********************************************/

/* $Log:	bi_vars.c,v $
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
"ARGC" ,
"FILENAME" ,
"NR" ,
"FNR" ,
"OFS" ,
"ORS" ,
"RLENGTH" ,
"RSTART" ,
"SUBSEP",
"VERSION"
} ;

/* insert the builtin vars in the hash table */

void  bi_vars_init()
{ register int i ;
  register SYMTAB *s ;

  for ( i = 0 ; i < NUM_BI_VAR ; i++ )
  { s = insert( bi_var_names[i] ) ;
    s->type = ST_VAR ; s->stval.cp = bi_vars + i ;
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

  bi_vars[VERSION].type = C_STRING ;
  bi_vars[VERSION].ptr = (PTR) new_STRING( VERSION_STRING ) ;

  bi_vars[NR].type = bi_vars[FNR].type = C_DOUBLE ;
  /* dval is already 0.0 */

  cell_zero.type = C_DOUBLE ;
  cell_one.type = C_DOUBLE ;
  cell_one.dval = 1.0 ;
}

CELL cell_zero ;
CELL cell_one ;
