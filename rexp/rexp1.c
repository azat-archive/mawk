
/********************************************
rexp1.c
copyright 1991, Michael D. Brennan

This is a source file for mawk an implementation of
the Awk programming language as defined in
Aho, Kernighan and Weinberger, The AWK Programming Language,
Addison-Wesley, 1988.

See the accompaning file, LIMITATIONS, for restrictions
regarding modification and redistribution of this
program in source or binary form.
********************************************/

/*  rexp1.c   */

/*  re machine  operations  */

#include  "rexp.h"

static MACHINE *PROTO( new_TWO , (int) ) ;


static  MACHINE  *new_TWO(type)
  int type ;
{ 
  static  MACHINE  x ;

  x.start = (STATE *) RE_malloc(2*STATESZ) ;
  x.stop = x.start + 1 ;
  x.start->type = type ;
  x.stop->type = M_ACCEPT ;
  return &x ;
} ;


/*  build a machine that recognizes any  */
MACHINE  RE_any()
{ return  * new_TWO(M_ANY) ; }

/*  build a machine that recognizes the start of string  */
MACHINE  RE_start()
{ return  * new_TWO(M_START) ; }

MACHINE  RE_end()
{ return  * new_TWO(M_END) ; }

/*  build a machine that recognizes a class  */
MACHINE  RE_class( bvp )
  BV *bvp  ;
{ register MACHINE *p = new_TWO(M_CLASS) ;

  p->start->data.bvp = bvp ;
  return *p ;
}


MACHINE  RE_u()
{ return  *new_TWO(M_U) ; }

MACHINE  RE_str( str, len)
  char *str ;
  unsigned len ;
{ register MACHINE *p = new_TWO(M_STR) ;

  p->start->len = len ;
  p->start->data.str = str ;
  return *p ;
}

/*  replace m and n by a machine that recognizes  mn   */
void  RE_cat( mp, np)
  MACHINE  *mp, *np ;
{ unsigned sz1, sz2, sz ;

  sz1 = mp->stop - mp->start  ;
  sz2 = np->stop - np->start + 1 ;
  sz  = sz1 + sz2 ;

  mp->start = (STATE *) RE_realloc( mp->start, sz * STATESZ ) ;
  mp->stop = mp->start + (sz - 1) ;
  (void)  memcpy( mp->start + sz1, np->start, sz2 * STATESZ ) ;
  RE_free( np->start ) ;
}

 /*  replace m by a machine that recognizes m|n  */

void  RE_or( mp, np)
  MACHINE  *mp, *np ;
{ register STATE *p ;
  unsigned szm, szn ;

  szm = mp->stop - mp->start + 1 ;
  szn = np->stop - np->start + 1 ;

  p = (STATE *) RE_malloc( (szm+szn+1) * STATESZ ) ;
  (void) memcpy( p+1, mp->start, szm * STATESZ ) ;
  RE_free( mp->start) ;
  mp->start = p ;
  (mp->stop  = p + szm + szn) -> type = M_ACCEPT ;
  p->type = M_2JA ;
  p->data.jump = szm+1 ;
  (void) memcpy( p + szm + 1 , np->start, szn * STATESZ) ;
  RE_free( np->start ) ;
  (p += szm)->type = M_1J ;
  p->data.jump = szn ;
}

/*  UNARY  OPERATIONS     */

/*  replace m by m*   */

void  RE_close( mp )
  MACHINE  *mp ;
{ register STATE *p ;
  unsigned sz ;

  sz = mp->stop - mp->start + 1 ;
  p = (STATE *) RE_malloc( (sz+2) * STATESZ ) ;
  (void) memcpy( p+1, mp->start, sz * STATESZ) ;
  RE_free( mp->start ) ;
  mp->start = p ;
  mp->stop  = p + (sz+1) ;
  p->type = M_2JA ;
  p->data.jump = sz + 1 ;
  (p += sz) -> type = M_2JB ;
  p->data.jump = -(sz-1) ;
  (p+1)->type = M_ACCEPT ;
}

/*  replace m  by  m+  (positive closure)   */

void  RE_poscl( mp )
  MACHINE  *mp ;
{ register STATE *p ;
  unsigned  sz ;

  sz = mp->stop - mp->start + 1 ;
  mp->start = p = (STATE *) RE_realloc(mp->start ,  (sz+1) * STATESZ ) ;
  mp->stop  = p + sz ;
  p +=  --sz ;
  p->type = M_2JB ;
  p->data.jump = -sz ;
  (p+1)->type = M_ACCEPT ;
}

/* replace  m  by  m? (zero or one)  */

void  RE_01( mp )
  MACHINE  *mp ;
{ unsigned  sz ;
  register  STATE *p ;

  sz = mp->stop - mp->start + 1 ;
  p = (STATE *) RE_malloc( (sz+1) * STATESZ ) ;
  (void) memcpy( p+1, mp->start, sz * STATESZ) ;
  RE_free( mp->start ) ;
  mp->start = p ;
  mp->stop = p + sz ;
  p->type = M_2JB ;
  p->data.jump = sz ;
}

/*===================================
MEMORY  ALLOCATION
 *==============================*/


VOID *RE_xmalloc( sz ) 
  unsigned sz ;
{ register VOID *p ;

  if ( ! ( p = malloc(sz) ) )  RE_error_trap(MEMORY_FAILURE) ;
  return p ;
}

VOID *RE_xrealloc( p, sz)
  register VOID *p ; unsigned sz ;
{ if ( ! ( p = realloc( p, sz) ) )  RE_error_trap(MEMORY_FAILURE) ;
  return p ;
}

