
/********************************************
jmp.c
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the Awk programming language as defined in
Aho, Kernighan and Weinberger, The AWK Programming Language,
Addison-Wesley, 1988.

See the accompaning file, LIMITATIONS, for restrictions
regarding modification and redistribution of this
program in source or binary form.
********************************************/

/* $Log:	jmp.c,v $
 * Revision 2.1  91/04/08  08:23:19  brennan
 * VERSION 0.97
 * 
*/

/* this module deals with back patching jumps, breaks and continues,
   and with save and restoring code when we move code.
   There are three stacks.  If we encounter a compile error, the
   stacks are frozen, i.e., we do not attempt error recovery
   on the stacks
*/


#include "mawk.h"
#include "jmp.h"
#include "code.h"
#include "sizes.h"
#include "init.h"
#include "memory.h"

extern unsigned compile_error_count ;
#define error_state  (compile_error_count>0)


/* a stack to hold jumps that need to be patched */

#define JMP_STK_SZ  (2*MAX_LOOP_DEPTH)

static INST **jmp_stack ; 
static INST **jmp_sp  ;

/*-------------------------------------*/
/* a stack to hold break or continue that need to be
   patched (which is all of them) */

#define  BC_SZ    MAX_LOOP_DEPTH

/* the stack holds a linked list of these */

struct BC_node { /* struct for the break/continue list */
char type ;   /*  'B' or 'C' */
INST *jmp ;   /*  the jump to patch */
struct BC_node *link ;
} ;

static   struct BC_node  **BC_stack ;
static   struct BC_node  **BC_sp ;

/*---------------------------------------*/
/* a stack to hold some pieces of code while 
   reorganizing loops */

#define  LOOP_CODE_SZ    (2*MAX_LOOP_DEPTH)

static struct loop_code {
INST *code ;
unsigned short len ;
}  *loop_code_stack , *lc_sp ;

/*--------------------------------------*/
void jmp_stacks_init()
{ jmp_stack = (INST **)  zmalloc(JMP_STK_SZ*sizeof(INST*)) ;
  jmp_sp = jmp_stack-1 ;

  BC_stack = (struct BC_node **) 
              zmalloc(BC_SZ*sizeof(struct BC_node*)) ;
  BC_sp =  BC_stack-1 ;

  loop_code_stack = (struct loop_code *)
                    zmalloc(LOOP_CODE_SZ*sizeof(struct loop_code)) ;
  lc_sp = loop_code_stack - 1 ;
}

void jmp_stacks_cleanup()
{ zfree(jmp_stack, JMP_STK_SZ*sizeof(INST*)) ;
  zfree(BC_stack, BC_SZ*sizeof(struct BC_node*)) ;
  zfree(loop_code_stack, LOOP_CODE_SZ*sizeof(struct loop_code)) ;
}
/*--------------------------------------*/
/* operations on the jmp_stack */

void code_jmp( jtype, target)
  int jtype ; INST *target ;
{ 
  if (error_state)  return ;

  /* check if a constant expression will be at top of stack,
     if so replace conditional jump with jump */

  if ( code_ptr[-2].op == _PUSHC && jtype != _JMP )
  { int t = test( (CELL *) code_ptr[-1].ptr ) ;
    if ( jtype == _JZ && ! t ||
         jtype == _JNZ && t )
    { code_ptr -= 2 ; jtype = _JMP ; }
  }
   
  if ( ! target ) /* jump will have to be patched later ,
                     put it on the jmp_stack */
  { if ( ++jmp_sp == jmp_stack + JMP_STK_SZ )
          overflow("jmp stack" , JMP_STK_SZ ) ; 
    *jmp_sp = code_ptr ;
    code2(jtype, 0) ;
  }
  else
  { INST *source = code_ptr ;
  
    code_ptr++->op = jtype ;
    code_ptr++->op = target - source ; 
  }
}

void patch_jmp(target)  /* patch a jump on the jmp_stack */
  INST *target ;
{ register INST *source ;

  if ( ! error_state )
  {
    if ( jmp_sp <= jmp_stack-1 ) bozo("jmp stack underflow") ;
    source = *jmp_sp-- ;
    source[1].op = target - source ;
  }
}


/*---------------------------*/

/* a stack of linked lists of BC_nodes for patching 
   break and continue statements.  */


void BC_new()  /* push an empty list on the stack */
{ 
  if ( ! error_state )
  { if ( ++BC_sp == BC_stack + BC_SZ ) overflow("BC stack", BC_SZ) ;
    * BC_sp = (struct BC_node *) 0 ;
  }
}

void BC_insert(type, address)
  int type ; INST *address ;
{ register struct BC_node *p ; 

  if ( error_state )  return ;
  if ( BC_sp <= BC_stack - 1 )
  {  compile_error(  type == 'B' ?
        "break statement outside of loop" :
        "continue statement outside of loop" ) ; 
     return ;
  }
  
  p = (struct BC_node *) zmalloc( sizeof(struct BC_node) ) ;
  p->type = type ; p->jmp = address ;
  p->link = *BC_sp ; *BC_sp = p ;
}

void BC_clear(B_address, C_address)  
/* patch all break and continues on list */
INST *B_address, *C_address ;
{ register struct BC_node *p , *q ;

  if (error_state) return ;
  if ( BC_sp <= BC_stack-1) bozo("underflow on BC stack") ;
  p = *BC_sp-- ;
  while ( p )
  { p->jmp[1].op = (p->type=='B' ? B_address : C_address) - p->jmp ;
    q = p ; p = p->link ; zfree(q, sizeof(struct BC_node)) ;
  }
}

/*---------------------------------------------*/
/*  save and restore some code for reorganizing
    loops on a stack */


void code_push( p, len)
  INST *p ; unsigned len ;
{ 
  if (error_state) return ;
  if ( ++lc_sp == loop_code_stack + LOOP_CODE_SZ )
        overflow("loop_code_stack" , LOOP_CODE_SZ) ;

  if ( len )
  { lc_sp->code = (INST *) zmalloc(sizeof(INST) * len) ;
    (void) memcpy(lc_sp->code, p, sizeof(INST) * len) ; }
  else  lc_sp->code = (INST *) 0 ;
  lc_sp->len = (unsigned short) len ;
}

/* copy the code at the top of the loop code stack to target.
   return the number of bytes moved */

unsigned code_pop(target) 
  INST *target ;
{ 
  if (error_state)  return 0 ;
  if ( lc_sp <= loop_code_stack-1 )  bozo("loop code stack underflow") ;
  if ( lc_sp->len )
  { (void) memcpy(target, lc_sp->code, lc_sp->len * sizeof(INST)) ;
    zfree(lc_sp->code, sizeof(INST)*lc_sp->len) ; }
  return lc_sp-- -> len ;
}
