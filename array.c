
/********************************************
array.c
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the Awk programming language as defined in
Aho, Kernighan and Weinberger, The AWK Programming Language,
Addison-Wesley, 1988.

See the accompaning file, LIMITATIONS, for restrictions
regarding modification and redistribution of this
program in source or binary form.
********************************************/

/* $Log:	array.c,v $
 * Revision 2.1  91/04/08  08:22:15  brennan
 * VERSION 0.97
 * 
*/

#include "mawk.h"
#include "symtype.h"
#include "memory.h"
#include "bi_vars.h"

extern int returning ; 
   /* flag -- on if returning from function call */

extern unsigned hash() ;

/* An array A is a pointer to a hash table of size
   A_HASH_PRIME holding linked lists of ANODEs.

   When an index is deleted via  delete A[i], the
   ANODE is not removed from the hash chain.  A[i].cp
   and A[i].sval are both freed and sval is set NULL.
   This method of deletion simplifies for( i in A ) loops.
*/

/* is sval in A ? */
int array_test( A, sval)
  ARRAY A ; 
  STRING *sval ;
{ char *s = sval->str ;
  register ANODE *p = A[ hash(s) % A_HASH_PRIME ] ;
  
  while ( p )
  { if ( p->sval && strcmp(s, p->sval->str) == 0 )  return 1 ;
    p = p->link ; }
  /* not there */
  return 0 ;
}
  
/* find x in array a
   if flag is ON x is a char* else a STRING*,
   computes a[x] as a CELL*
*/

CELL *array_find( a, x, flag)
  ARRAY a ; PTR  x ; int flag ;
{ register ANODE *p ;  /* search with p */
  ANODE *q ;  /* pts at a deleted node */
  unsigned h ;
  char *s ;

  s = flag ? (char *) x : ( (STRING *) x) -> str ;
  p = a[ h = hash(s) % A_HASH_PRIME ] ;
  q = (ANODE *) 0 ; 

  while ( p )
  { 
    if ( p->sval )
    {
      if ( strcmp(s,p->sval->str) == 0 )  /* found */
	    return  p->cp ;
    }
    else /* a deleted node */
    if ( !q )  q = p ;

    p = p->link ;
  }
  
  /* not there make one  */
  if ( q )  p = q ; /* reuse the node */
  else
  { p = (ANODE *) zmalloc( sizeof(ANODE) ) ;
    p->link = a[h] ; a[h] = p ; }

  if ( flag )  p->sval = new_STRING(s) ;
  else
  { p->sval = (STRING *) x ; p->sval->ref_cnt++ ; }
  p->cp = new_CELL() ; p->cp->type = C_NOINIT ;
  return p->cp ;
}

void  array_delete( a, sval)
  ARRAY a ; STRING *sval ;
{ char *s = sval->str ;
  register ANODE *p = a[ hash(s) % A_HASH_PRIME ] ;

  while ( p )
  { if ( p->sval && strcmp(s, p->sval->str)== 0 ) /* found */
    { 
      cell_destroy(p->cp) ;  free_CELL(p->cp) ;
      free_STRING(p->sval) ; p->sval = (STRING *) 0 ;

      break ;
    }

    p = p->link ;
  }
}
      

/* for ( i in A ) ,
   loop over elements of an array 

sp[0].ptr :  a pointer to A ( the hash table of A)
sp[-1] :  a pointer to i ( a cell ptr)

cdp[0] :  a stop op to catch breaks
cdp[1] :  offset from cdp of the code after the loop (n+2)
cdp[2] :  start of body of the loop
cdp[3..n] : the rest of the body
cdp[n+1]  : a stop op to delimit the body and catch continues
*/

INST  *array_loop( cdp, sp, fp) /* passed code, stack and frame ptrs */
  INST *cdp ; 
  CELL *sp, *fp ;
{ int i ;
  register ANODE *p ;
  ARRAY   A = (ARRAY) sp-- -> ptr ;
  register CELL *cp = (CELL *) sp-- -> ptr ;

  for ( i = 0 ; i < A_HASH_PRIME ; i++ )
  for ( p = A[i] ; p ; p = p->link )
  { if ( ! p->sval /* its deleted */ )  continue ;
  
    cell_destroy(cp) ;
    cp->type = C_STRING ;
    cp->ptr = (PTR) p->sval ;
    p->sval->ref_cnt++ ;

    /* execute the body of the loop */
    if ( execute(cdp+2, sp, fp) == cdp /* exec'ed a break statement */
         || returning  /* function return in body of loop */
       )  
           goto break2 /* break both for loops */ ; 
  }

break2 :
  return   cdp + cdp[1].op ;
}


/* cat together cnt elements on the eval stack to form
   an array index using SUBSEP */

CELL *array_cat( sp, cnt)
  register CELL *sp ;
  int cnt ;
{ register CELL *p ;  /* walks the stack */
  CELL subsep ;  /* a copy of bi_vars[SUBSEP] */
  unsigned subsep_len ;
  char *subsep_str ;
  unsigned total_len ; /* length of cat'ed expression */
  CELL *top ;  /* sp at entry */
  char *t ; /* target ptr when catting */
  STRING *sval ;  /* build new STRING here */

  /* get a copy of subsep, we may need to cast */
  (void) cellcpy(&subsep, bi_vars + SUBSEP) ;
  if ( subsep.type < C_STRING ) cast1_to_s(&subsep) ;
  subsep_len = string(&subsep)->len ;
  subsep_str = string(&subsep)->str ;
  total_len = --cnt * subsep_len ;

  top = sp ;
  sp -= cnt ;
  for( p = sp ; p <= top ; p++ )
  {
    if ( p->type < C_STRING ) cast1_to_s(p) ;
    total_len += string(p)->len ;
  }

  sval = new_STRING((char *)0, total_len) ;
  t = sval->str ;

  /* put the pieces together */
  for( p = sp ; p < top ; p++ )
  { (void) memcpy(t, string(p)->str, string(p)->len) ;
    (void) memcpy( t += string(p)->len, subsep_str, subsep_len) ;
    t += subsep_len ;
  }
  /* p == top */
  (void) memcpy(t, string(p)->str, string(p)->len) ;

  /* done, now cleanup */
  free_STRING(string(&subsep)) ;
  while ( p >= sp )  { free_STRING(string(p)) ; p-- ; }
  sp->type = C_STRING ;
  sp->ptr = (PTR) sval ;
  return sp ;
}


/* free all memory used by an array,
   only used for arrays local to a function call
*/

void  array_free(A)
  ARRAY  A ;
{ register ANODE *p ;
  register int i ;
  ANODE *q ;

  for( i = 0 ; i < A_HASH_PRIME ; i++ )
  { p = A[i] ;
    while ( p )
    { /* check its not a deleted node */
      if ( p->sval )
      { free_STRING(p->sval) ;
        cell_destroy(p->cp) ;
        free_CELL(p->cp) ;
      }

      q = p ; p = p->link ;
      zfree( q, sizeof(ANODE)) ;
    }
  }

  zfree(A, sizeof(ANODE *) * A_HASH_PRIME ) ;
}
