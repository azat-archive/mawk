
/********************************************
zmalloc.c
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the Awk programming language as defined in
Aho, Kernighan and Weinberger, The AWK Programming Language,
Addison-Wesley, 1988.

See the accompaning file, LIMITATIONS, for restrictions
regarding modification and redistribution of this
program in source or binary form.
********************************************/

/*$Log:	zmalloc.c,v $
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

  if( !(p = malloc(size)) )
  { errmsg(0, "out of memory") ; mawk_exit(1) ; }
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

  blocks = (size >> ZSHIFT) + ((size & (ZBLOCKSZ-1)) != 0) ;

  if ( p = pool[blocks-1] )
  { pool[blocks-1] = p->link ; return (PTR) p ; }

  if ( blocks > amt_avail )
  { if ( amt_avail ) /* free avail */
    { avail->link = pool[--amt_avail] ; pool[amt_avail] = avail ; }
    if ( !(avail = (ZBLOCK *) malloc(CHUNK*ZBLOCKSZ)) )
    { /* if we get here, almost out of memory */
        amt_avail = 0 ;   return  emalloc(size) ; }
    amt_avail = CHUNK ;
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
    index  = (size >> ZSHIFT) + ((size & (ZBLOCKSZ-1)) != 0) - 1;
    ((ZBLOCK *) p)->link = pool[index] ;
    pool[index] = (ZBLOCK *) p ;
  }
}

PTR  zrealloc( p, old_size, new_size )
  register PTR  p ;
  unsigned old_size, new_size ;
{ register PTR q ;

  (void) memcpy(q = zmalloc(new_size), p, 
                old_size < new_size ? old_size : new_size) ;
 
  zfree(p, old_size) ;
  return q ;
}


