
/********************************************
kw.c
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the AWK programming language.

Mawk is distributed without warranty under the terms of
the GNU General Public License, version 2, 1991.
********************************************/


/* $Log:	kw.c,v $
 * Revision 3.4.1.1  91/09/14  17:23:37  brennan
 * VERSION 1.0
 * 
 * Revision 3.4  91/08/13  06:51:41  brennan
 * VERSION .9994
 * 
 * Revision 3.3  91/07/18  07:44:45  brennan
 * eliminated useless assignment
 * 
 * Revision 3.2  91/06/28  04:16:56  brennan
 * VERSION 0.999
 * 
 * Revision 3.1  91/06/07  10:27:47  brennan
 * VERSION 0.995
 * 
 * Revision 2.1  91/04/08  08:23:23  brennan
 * VERSION 0.97
 * 
*/


/* kw.c */


#include "mawk.h"
#include "symtype.h"
#include "parse.h"
#include "init.h"


static struct kw {
char *text ;
short kw ;
}  keywords[] = {

"print", PRINT,
"printf", PRINTF,
"do" , DO ,
"while" , WHILE ,
"for" , FOR ,
"break" , BREAK ,
"continue" , CONTINUE ,
"if" , IF ,
"else", ELSE ,
"in" , IN ,
"delete", DELETE ,
"split" , SPLIT ,
"match" , MATCH_FUNC ,
"BEGIN" , BEGIN,
"END" ,   END ,
"exit" , EXIT ,
"next" , NEXT ,
"return", RETURN,
"getline", GETLINE,
"sub" , SUB,
"gsub", GSUB,
"function", FUNCTION,
(char *) 0 , 0 } ;

/* put keywords in the symbol table */
void kw_init()
{ register struct kw *p = keywords ;
  register SYMTAB *q ;

  while ( p->text )
  { q = insert( p->text ) ;
    q->type = ST_KEYWORD ;
    q->stval.kw = p++ -> kw ;
  }
}

/* find a keyword to emit an error message */
char *find_kw_str( kw_token )
  int kw_token ;
{ struct kw *p ;

  for( p = keywords ; p->text ; p++ )
        if ( p->kw == kw_token )  return p->text ;
  /* search failed */
  return (char *) 0 ;
}
