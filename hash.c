
/********************************************
hash.c
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the Awk programming language as defined in
Aho, Kernighan and Weinberger, The AWK Programming Language,
Addison-Wesley, 1988.

See the accompaning file, LIMITATIONS, for restrictions
regarding modification and redistribution of this
program in source or binary form.
********************************************/


/* $Log:	hash.c,v $
 * Revision 2.1  91/04/08  08:23:13  brennan
 * VERSION 0.97
 * 
*/


/* hash.c */

#include "mawk.h"
#include "memory.h"
#include "symtype.h"
#include <string.h>


unsigned hash(s)
  register char *s ;
{ register unsigned h = 0 ;

  while ( *s )  h += h + *s++ ;
  return  h  ;
}

typedef struct hash {
struct hash *link ;
SYMTAB  symtab ;
}  HASHNODE ;

static  HASHNODE *PROTO( delete, (char *) ) ;

#define  new_HASHNODE() (HASHNODE *) zmalloc(sizeof(HASHNODE))

static HASHNODE *hash_table[HASH_PRIME] ;

/*
 *   insert -- s is not there and need not be duplicated
 *   -- used during initialization
 */

SYMTAB *insert(s) 
  char *s ;
{ register HASHNODE *p = new_HASHNODE();
  register unsigned h ;
  
  p->link = hash_table[h = hash(s) % HASH_PRIME ] ;
  p->symtab.name = s ;
  hash_table[h] = p ;
  return &p->symtab ;
}

/*
 *  find --  s might be there, find it else insert and dup
 *  s 
 */

SYMTAB *find(s)
  char *s ;
{ register HASHNODE *p ;
  HASHNODE *q ;
  unsigned h ;

  p = hash_table[h = hash(s) % HASH_PRIME ] ;
  q = (HASHNODE *) 0 ;
  while ( 1 )
  { if ( !p )
    { p = new_HASHNODE() ;
      p->symtab.type = ST_NONE ;
      p->symtab.name = strcpy(zmalloc( strlen(s)+1 ), s) ;
      break ;
    }

    if ( strcmp(p->symtab.name, s) == 0 ) /* found */
      if ( !q )  /* already at the front */
        return  &p->symtab ;
      else /* delete from the list */
      { q->link = p->link ;  break ; }

    q = p ; p = p->link ;
  }
  /* put p on front of the list */
  p->link = hash_table[h] ;
  hash_table[h] = p ;
  return & p->symtab ;
}


/* remove a node from the hash table
   return a ptr to the node */

static unsigned last_hash ;

static  HASHNODE  *delete( s )
  char *s ;
{ register HASHNODE *p ;
  HASHNODE *q = (HASHNODE *) 0 ;
  unsigned h ;

  p = hash_table[ last_hash = h = hash(s) % HASH_PRIME ] ;
  while ( p )
      if ( strcmp(p->symtab.name, s) == 0 )  /* found */
      {
        if ( q )  q->link = p->link ;
        else  hash_table[h] = p->link ;
        return p ;
      }
      else { q = p ; p = p->link ; }

#ifdef  DEBUG   /* we should not ever get here */
  bozo("delete") ;
#endif
  return (HASHNODE *) 0 ;
}

/* when processing user functions,  global ids which are
   replaced by local ids are saved on this list */

static HASHNODE  *save_list ;

/* store a global id on the save list,
   return a ptr to the local symtab  */
SYMTAB *save_id( s )
  char *s ;
{ HASHNODE *p, *q ;
  unsigned h ;

  p = delete(s) ;
  q = new_HASHNODE() ;
  q->symtab.type = ST_LOCAL_NONE ;
  q->symtab.name = p->symtab.name ;
  /* put q in the hash table */
  q->link = hash_table[ h = last_hash ] ;
  hash_table[h] = q ;

  /* save p */
  p->link = save_list ; save_list = p ;

  return & q->symtab ;
}

/* restore all global indentifiers */
void  restore_ids()
{ register HASHNODE *p, *q ;
  register unsigned h ;

  q = save_list ; save_list = (HASHNODE *) 0 ;
  while ( q )
  {
    p = q ; q = q->link ;
    zfree( delete(p->symtab.name) , sizeof(HASHNODE) ) ;
    p->link = hash_table[h = last_hash ] ; 
    hash_table[h] = p ;
  }
}
