
/********************************************
array.c
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the AWK programming language.

Mawk is distributed without warranty under the terms of
the GNU General Public License, version 2, 1991.
********************************************/

/* $Log:	array.c,v $
 * Revision 3.3.1.1  91/09/14  17:22:35  brennan
 * VERSION 1.0
 * 
 * Revision 3.3  91/08/13  06:50:44  brennan
 * VERSION .9994
 * 
 * Revision 3.2  91/06/28  04:16:00  brennan
 * VERSION 0.999
 * 
 * Revision 3.1  91/06/07  10:26:48  brennan
 * VERSION 0.995
 * 
 * Revision 2.3  91/05/22  07:39:40  brennan
 * static prototypes
 * 
 * Revision 2.2  91/05/15  12:07:27  brennan
 * dval hash table for arrays
 * 
 * Revision 2.1  91/04/08  08:22:15  brennan
 * VERSION 0.97
 * 
*/

#include "mawk.h"
#include "symtype.h"
#include "memory.h"
#include "bi_vars.h"

/* convert doubles to string for array indexing */
#define   A_FMT         "%.6g"

static ANODE *PROTO(find_by_sval, (ARRAY, STRING *, int) ) ;
static D_ANODE *PROTO(find_by_dval, (ARRAY, double , int) ) ;

extern int returning ; 
   /* flag -- on if returning from function call */

extern unsigned hash() ;

/* An array A is a pointer to an array of struct array,
   which is two hash tables in one.  One for strings
   and one for doubles.

   each array is of size A_HASH_PRIME.

   When an index is deleted via  delete A[i], the
   ANODE is not removed from the hash chain.  A[i].cp
   and A[i].sval are both freed and sval is set NULL.
   This method of deletion simplifies for( i in A ) loops.

   On the D_ANODE list, we use real deletion and move to the
   front on access.

   Separate nodes (as opposed to one type of node on two lists)
   to
     (1) d1 != d2, but sprintf(A_FMT,d1) == sprintf(A_FMT,d1)
         so two dnodes can point at the same anode.
     (2) Save a little data space(64K PC mentality).

   the cost is an extra level of indirection.

   Some care is needed so that things like
     A[1] = 2 ; delete A["1"] work .
*/

#define  _dhash(d)    (((int)(d)&0x7fff)%A_HASH_PRIME)
#define  DHASH(d)     (last_dhash=_dhash(d))
static  unsigned  last_dhash ;


static  ANODE *find_by_sval(A, sval, cflag)
  ARRAY  A ;
  STRING *sval ;
  int  cflag ; /* create if on */
{ 
   char *s = sval->str ;
   unsigned h = hash(s) % A_HASH_PRIME ;
   register ANODE *p = A[h].link ;
   ANODE *q = 0 ; /* holds first deleted ANODE */

   while ( p )
   {
     if ( p->sval )
     { if ( strcmp(s,p->sval->str) == 0 )  return p ; }
     else /* its deleted, mark with q */
     if ( ! q )  q = p ;  

     p = p->link ;
   }

   /* not there */
   if ( cflag )
   {
       if ( q )  p = q ; /* reuse the deleted node q */
       else
       { p = (ANODE *)zmalloc(sizeof(ANODE)) ;
         p->link = A[h].link ; A[h].link = p ;
       }

       p->sval = sval ;
       sval->ref_cnt++ ;
       p->cp = (CELL *) zmalloc(sizeof(CELL)) ;
       p->cp->type = C_NOINIT ;
   }
   return p ;
}


/* on the D_ANODE list, when we find a node we move it
   to the front of the hash chain */

static D_ANODE  *find_by_dval(A, d, cflag)
  ARRAY  A ;
  double d ;
  int cflag ;
{
  unsigned h = DHASH(d) ;
  register D_ANODE *p = A[h].dlink ;
  D_ANODE *q = 0 ; /* trails p for move to front */
  ANODE *ap ;

   while ( p )
       if ( p->dval == d )
       { /* found */
         if ( ! p->ap->sval ) /* but it was deleted by string */
         { if ( q )  q->dlink = p->dlink ;
           else A[h].dlink = p->dlink ;
           zfree(p, sizeof(D_ANODE)) ;
           break ; 
         }
         /* found */
         if ( !q )  return  p ; /* already at front */
         else /* delete to put at front */
         { q->dlink = p->dlink ; goto found ; }
       }
       else
       { q = p ; p = p->dlink ; }

   /* not there, still need to look by sval 
      CANNOT use temp_buff, may be in use by split */
   
   { char xbuff[16] ;
     STRING *sval ;

     (void) sprintf(xbuff, A_FMT, d) ;
     sval = new_STRING(xbuff) ;
     ap = find_by_sval(A, sval, cflag) ;
     free_STRING(sval) ;
   }    

   if ( ! ap )  return (D_ANODE *) 0 ;
   /* create new D_ANODE  */
   p = (D_ANODE *) zmalloc(sizeof(D_ANODE)) ;
   p->ap = ap ; p->dval = d ;

found : /* put p at front */
   p->dlink = A[h].dlink ; A[h].dlink = p ;
   return p ;
}

CELL *array_find(A, cp, create_flag)
  ARRAY A ;
  CELL *cp ;
  int create_flag ;
{
  ANODE *ap ;
  D_ANODE *dp ;

  switch( cp->type )
  {
    case C_DOUBLE :
        return  (dp = find_by_dval(A, cp->dval, create_flag))
                ? dp->ap->cp : (CELL *) 0 ;

    case  C_NOINIT :
        ap = find_by_sval(A, &null_str, create_flag) ;
        break ;

    default :
        ap = find_by_sval(A, string(cp), create_flag) ;
        break ;
  }

  return  ap ? ap->cp : (CELL *) 0 ;
}


void  array_delete(A, cp)
  ARRAY A ; CELL *cp ;
{
  ANODE *ap ;
  D_ANODE *dp ;

  switch( cp->type )
  {
    case C_DOUBLE :
        if ( !(dp = find_by_dval(A, cp->dval, 0)) ) return ;

        ap = dp->ap ;
        /* dp is at front of A[last_dhash], delete the D_ANODE */
        A[last_dhash].dlink = dp->dlink ;
        zfree(dp, sizeof(D_ANODE)) ;
        break ;

    case  C_NOINIT :
        ap = find_by_sval(A, &null_str, 0) ;
        break ;

    default :
        ap = find_by_sval(A, string(cp), 0) ;
        break ;
  }

  if ( ap )
  { free_STRING(ap->sval) ; ap->sval = (STRING *) 0 ;
    cell_destroy(ap->cp)  ; zfree(ap->cp, sizeof(CELL)) ;
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
  for ( p = A[i].link ; p ; p = p->link )
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
  { (void) memcpy(t, string(p)->str, SIZE_T(string(p)->len)) ;
    (void) memcpy( t += string(p)->len, subsep_str, SIZE_T(subsep_len)) ;
    t += subsep_len ;
  }
  /* p == top */
  (void) memcpy(t, string(p)->str, SIZE_T(string(p)->len)) ;

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
{ register ANODE *ap ;
  register D_ANODE *dp ;
  register int i ;
  ANODE *aq ;
  D_ANODE *dq ;

  for( i = 0 ; i < A_HASH_PRIME ; i++ )
  { ap = A[i].link ;
    while ( ap )
    { /* check its not a deleted node */
      if ( ap->sval )
      { free_STRING(ap->sval) ;
        cell_destroy(ap->cp) ;
        free_CELL(ap->cp) ;
      }

      aq = ap ; ap = ap->link ;
      zfree( aq, sizeof(ANODE)) ;
    }

    dp = A[i].dlink ;
    while ( dp )
    { dq = dp ; dp = dp->dlink ; zfree(dq,sizeof(D_ANODE)) ; }
  }

  zfree(A, sizeof(A[0]) * A_HASH_PRIME ) ;
}
