
/********************************************
zmalloc.c
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the AWK programming language.

Mawk is distributed without warranty under the terms of
the GNU General Public License, version 2, 1991.
********************************************/

/*$Log:	zmalloc.c,v $
 * Revision 3.4.1.1  91/09/14  17:24:31  brennan
 * VERSION 1.0
 * 
 * Revision 3.4  91/08/13  06:52:19  brennan
 * VERSION .9994
 * 
 * Revision 3.3  91/06/28  04:17:47  brennan
 * VERSION 0.999
 * 
 * Revision 3.2  91/06/26  05:57:48  brennan
 * fixed alignment bug , only showed on some machines, V9981
 * 
 * Revision 3.1  91/06/07  10:28:30  brennan
 * VERSION 0.995
 * 
 * Revision 2.7  91/06/05  11:22:37  brennan
 * changed stupid computation of blocks
 * 
 * Revision 2.6  91/06/05  07:20:44  brennan
 * better error messages when regular expression compiles fail
 * 
 * Revision 2.5  91/06/04  09:12:00  brennan
 * added some ptr casts
 * 
 * Revision 2.4  91/06/04  06:50:34  brennan
 * use parser table memory in zmalloc (if using byacc)
 * 
 * Revision 2.3  91/06/03  07:39:52  brennan
 * fixed bug that only shows up if nearly out of memory
 * this bug was found by Carl Mascott
 * 
 * Revision 2.2  91/04/09  12:39:45  brennan
 * added static to funct decls to satisfy STARDENT compiler
 * 
 * Revision 2.1  91/04/08  08:24:17  brennan
 * VERSION 0.97
 * 
*/

/*  zmalloc.c  */
#include  "mawk.h"
#include  "zmalloc.h"

void PROTO( mawk_exit, (int) ) ;

extern  struct yacc_mem  *yacc_memp ;

/*
  zmalloc() gets mem from malloc() in CHUNKS of 2048 bytes
  and cuts these blocks into smaller pieces that are multiples
  of eight bytes.  When a piece is returned via zfree(), it goes
  on a linked linear list indexed by its size.  The lists are
  an array, pool[].

  E.g., if you ask for 22 bytes with p = zmalloc(22), you actually get
  a piece of size 24.  When you free it with zfree(p,22) , it is added
  to the list at pool[2].
*/

#define ZBLOCKSZ    8    
#define ZSHIFT      3
#define POOLSZ      16

#define  CHUNK          256    
        /* number of blocks to get from malloc */

static PTR  PROTO( emalloc, (unsigned) ) ;
void PROTO( errmsg, (int , char *, ...) ) ;

static PTR emalloc(size)
  unsigned size ;
{ PTR p ;
  static char out[] = "out of memory" ;

  if( !(p = (PTR) malloc(SIZE_T(size))) )
	if ( mawk_state == EXECUTION ) rt_error(out) ;
	else /* I don't think this will ever happen */
	{ compile_error(out) ; mawk_exit(1) ; }
  return p ;
}


typedef  union  zblock {
char dummy[ZBLOCKSZ] ;
union zblock *link ;
}  ZBLOCK  ;

/* ZBLOCKS of sizes 1, 2, ... 16
   which is bytes of sizes 8, 16, ... , 128
   are stored on the linked linear lists in
   pool[0], pool[1], ... , pool[15]
*/

static  ZBLOCK  *pool[POOLSZ] ;

PTR   zmalloc( size )
  unsigned size ;
{ register unsigned blocks ;
  register ZBLOCK *p ;
  static  unsigned amt_avail ;
  static  ZBLOCK  *avail ;

  if ( size > POOLSZ * ZBLOCKSZ )  return emalloc(size) ;

  blocks = (size + (ZBLOCKSZ-1)) >> ZSHIFT ;

  if ( p = pool[blocks-1] )
  { pool[blocks-1] = p->link ; return (PTR) p ; }

  if ( blocks > amt_avail )
  { if ( amt_avail ) /* free avail */
    { avail->link = pool[--amt_avail] ; pool[amt_avail] = avail ; }

    /* use parser tables first */
    if ( yacc_memp->zblocks >= blocks )
    { avail = (ZBLOCK *) yacc_memp->mem ;
      amt_avail = yacc_memp++ -> zblocks ;
      /* make sure its -- aligned */
      if ( (int) avail & 7 )
      { avail = (ZBLOCK*)((char *)avail + 8 - ((int)avail&7)) ;
	amt_avail-- ;
      }
    }
    else
    if ( !(avail = (ZBLOCK *) malloc(SIZE_T(CHUNK*ZBLOCKSZ))) )
    { /* if we get here, almost out of memory */
        amt_avail = 0 ;   
	return  emalloc(blocks << ZSHIFT) ;
    }
    else  amt_avail = CHUNK ;
  }
  
  /* get p from the avail pile */
  p = avail ; avail += blocks ; amt_avail -= blocks ; 
  return (PTR) p ;
}

void  zfree( p, size)
  register PTR p ;  unsigned size ;
{ register int index ; ;

  if ( size > POOLSZ * ZBLOCKSZ )  free(p) ;
  else
  {
    index  = ((size + (ZBLOCKSZ-1))>>ZSHIFT) - 1;
    ((ZBLOCK *) p)->link = pool[index] ;
    pool[index] = (ZBLOCK *) p ;
  }
}

PTR  zrealloc( p, old_size, new_size )
  register PTR  p ;
  unsigned old_size, new_size ;
{ register PTR q ;

  (void) memcpy(q = zmalloc(new_size), p, 
                SIZE_T(old_size < new_size ? old_size : new_size)) ;
 
  zfree(p, old_size) ;
  return q ;
}


