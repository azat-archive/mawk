
/********************************************
rexp.c
copyright 1991, Michael D. Brennan

This is a source file for mawk an implementation of
the Awk programming language as defined in
Aho, Kernighan and Weinberger, The AWK Programming Language,
Addison-Wesley, 1988.

See the accompaning file, LIMITATIONS, for restrictions
regarding modification and redistribution of this
program in source or binary form.
********************************************/

/*   rexp.c   */

/*  op precedence  parser for regular expressions  */

#include  "rexp.h"

/* static  prototypes */
void  PROTO( op_pop, (void) ) ;

/*  DATA   */
int   REerrno ;
char *REerrlist[] = { (char *) 0 ,
/* 1  */    "missing '('",
/* 2  */    "missing ')'",
/* 3  */    "bad class -- [], [^] or [" ,
/* 4  */    "missing operand" ,
/* 5  */    "resource exhaustion -- regular expression too large",
/* 6  */    "null regular expression" } ;

/* E5 is very unlikely to occur */

/* This table drives the operator precedence parser */
static  int  table[8][8]  =  {

/*        0   |   CAT   *   +   ?   (   )   */
/* 0 */   0,  L,  L,    L,  L,  L,  L,  E1,
/* | */   G,  G,  L,    L,  L,  L,  L,  G,
/* CAT*/  G,  G,  G,    L,  L,  L,  L,  G,
/* * */   G,  G,  G,    G,  G,  G, E7,  G,
/* + */   G,  G,  G,    G,  G,  G, E7,  G,
/* ? */   G,  G,  G,    G,  G,  G, E7,  G,
/* ( */   E2, L,  L,    L,  L,  L,  L,  EQ,
/* ) */   G , G,  G,    G,  G,  G,  E7,  G     }   ;


/*====================================
  THE  STACKS
 ==========================*/
typedef struct
{ int  token ;
  int  prec ;   }  OP ;

#define  STACKSZ   96

/*---------------------------
  m_ptr -> top filled slot on the m_stack
  op_ptr -> top filled slot on op_stack, 
     initially this is only half filled with the token
     the precedence is added later
 *----------------------*/

static  OP  *op_stack, *op_limit, *op_ptr ;
static  MACHINE *m_stack, *m_limit, *m_ptr ;

/* inline for speed on the m_stack */
#define m_pop() (m_ptr<m_stack?RE_error_trap(-E4): *m_ptr--)
#define m_push(x)  if(++m_ptr==m_limit) RE_error_trap(-E5);*m_ptr=(x)

/*=======================*/

static jmp_buf  err_buf  ;  /*  used to trap on error */

MACHINE  RE_error_trap(x)  /* return is dummy to make macro OK */
  int x ;
{
  while ( m_ptr >= m_stack ) RE_free( m_ptr-- -> start ) ;
  RE_free(m_stack) ; RE_free(op_stack) ;
  REerrno = x ;
  longjmp(err_buf, 1 ) ;
  /* dummy return to make compiler happy */
  return *m_stack ;
}


VOID *REcompile(re)
  char *re ;
{ MACHINE  m  ;
  register int  t ;

  RE_lex_init(re) ;

  if ( *re == 0 )
  { STATE *p = (STATE *) RE_malloc( sizeof(STATE) ) ;
    p->type = M_ACCEPT ;
    return  (VOID *) p ;
  }

  if ( setjmp(err_buf) )   return (VOID *) 0 ;
     /* global error trap */

  /* initialize the stacks  */
  m_stack =(MACHINE *) RE_malloc(STACKSZ*sizeof(MACHINE)) ;
  m_ptr = m_stack - 1 ;
  m_limit = m_stack + STACKSZ ;
  op_ptr = op_stack = (OP *) RE_malloc(STACKSZ*sizeof(OP)) ;
  op_ptr->token = 0 ;
  op_limit = op_stack + STACKSZ ;


  t = RE_lex(&m) ;

  while( 1 )
   { switch( t )
       { 
         case T_STR  :
         case T_ANY  :
         case T_U    :
         case T_START :
         case T_END :
         case T_CLASS :  m_push(m) ;  break ;

         case  0 :   /*  end of reg expr   */
           if ( op_ptr -> token == 0 )  /*  done   */
           { m = m_pop() ;
             if ( m_ptr < m_stack )  /* DONE !!! */
             { free(m_stack) ; free(op_stack) ;
               return  (VOID *) m.start ;
             }
               /*  machines still on the stack  */
             RE_panic("values still on machine stack") ;
             }
         /*  case 0  falls  thru to default
             which is operator case  */

         default:

           if ( (op_ptr -> prec = table[op_ptr -> token][t]) == G )
               { while ( op_ptr -> prec != L )  op_pop() ;
                 continue ; }

           if ( op_ptr -> prec < 0 )
              if ( op_ptr->prec == E7 ) 
                  RE_panic("parser returns E7") ;
              else  RE_error_trap(-op_ptr->prec) ;

           if ( ++op_ptr == op_stack + STACKSZ ) /* stack overflow */
                 RE_error_trap(-E5) ;
           op_ptr -> token = t ;
       }
    t = RE_lex(&m) ;
  }
}

static void  op_pop()
{ register int  t  ;
  MACHINE m, n ;

  if ( (t = op_ptr-- -> token) >= T_LP ) return ;
        /* nothing to do with '(' or ')' */
  if ( t <= T_CAT )  /* binary operation */
        n = m_pop() ;
  m = m_pop() ;

  switch( t )
  {  case  T_CAT :  RE_cat(&m, &n) ;  break ;
     case  T_OR  :  RE_or( &m, &n) ;  break ;
     case T_STAR  :  RE_close( &m) ;  break ;
     case T_PLUS  :  RE_poscl( &m ) ; break ;
     case T_Q     :  RE_01( &m ) ;    break ;
     default       :
        RE_panic("strange token popped from op_stack") ;
  }
  m_push(m) ;
}

/* getting here means a logic flaw or unforeseen case */
void RE_panic( s )
  char *s ;
{ fprintf( stderr, "REcompile() - panic:  %s\n", s) ;
  exit(100) ; }

