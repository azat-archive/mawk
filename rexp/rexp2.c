
/********************************************
rexp2.c
copyright 1991, Michael D. Brennan

This is a source file for mawk an implementation of
the Awk programming language as defined in
Aho, Kernighan and Weinberger, The AWK Programming Language,
Addison-Wesley, 1988.

See the accompaning file, LIMITATIONS, for restrictions
regarding modification and redistribution of this
program in source or binary form.
********************************************/

/*  rexp2.c   */

/*  test a string against a machine   */

#include "rexp.h"
#include <string.h>

/* statics */
static RT_STATE *PROTO(slow_push,(RT_STATE *,STATE*,char*,int)); 

/*  check that a bit is on  */
#define  ison(b,x) ( (b)[(x)>>3] & ( 1 << ((x)&7)  ))


RT_STATE *RE_run_stack_base; 
RT_STATE *RE_run_stack_limit ;
/* for statistics and debug */
static RT_STATE *stack_max ; 

void RE_run_stack_init()
{ if ( !RE_run_stack_base )
  {
    RE_run_stack_base = (RT_STATE *)
                 RE_malloc(sizeof(RT_STATE) * 16 ) ;
    RE_run_stack_limit = RE_run_stack_base + 16 ;
    stack_max = RE_run_stack_base-1 ;
  }
}

RT_STATE  *RE_new_run_stack()
{ int oldsize = RE_run_stack_limit - RE_run_stack_base ;

  RE_run_stack_base = (RT_STATE *) RE_realloc( RE_run_stack_base ,
          (oldsize+8) * sizeof(RT_STATE) ) ;
  RE_run_stack_limit = RE_run_stack_base + oldsize + 8 ;
  return  stack_max = RE_run_stack_base + oldsize ;
}

static RT_STATE *slow_push(sp, m, s, u)
  RT_STATE *sp ;
  STATE *m ;
  char *s ;
  int   u ;
{ 
  if ( sp > stack_max )
     if ( (stack_max = sp) == RE_run_stack_limit )
	     sp = RE_new_run_stack() ;

  sp->m = m ; sp->s = s ; sp->u = u ;
  return sp ;
}

#ifdef   DEBUG
void  print_max_stack(f)
  FILE *f ;
{ fprintf(f, "stack_max = %d\n", stack_max-RE_run_stack_base+1) ; }
#endif

#ifdef   DEBUG
#define  push(mx,sx,ux)   stackp = slow_push(++stackp, mx, sx, ux)
#else
#define  push(mx,sx,ux)   if (++stackp == RE_run_stack_limit)\
                                stackp = slow_push(stackp,mx,sx,ux) ;\
                          else\
                          { stackp->m=mx;stackp->s=sx;stackp->u=ux;}
#endif


#define   CASE_UANY(x)  case  x + U_OFF :  case  x + U_ON


int  REtest( str, machine)
  char *str ;
  VOID *machine ;
{ register STATE *m = (STATE *) machine ;
  register char *s = str ;
  register RT_STATE *stackp ;
  int u_flag ;
  char *str_end ;
  char *ts ; /*convenient temps */
  STATE *tm ;

  /* handle the easy case quickly */
  if ( (m+1)->type == M_ACCEPT && m->type == M_STR )
        return  (int ) str_str(s, m->data.str, m->len) ;
  else
  { u_flag = U_ON ; str_end = (char *) 0 ;
    stackp = RE_run_stack_base - 1 ;
    goto  reswitch ;
  }

refill :
  if ( stackp < RE_run_stack_base )  return  0 ;
  m = stackp->m ;
  s = stackp->s ;
  u_flag  = stackp-- -> u ;


reswitch  :

  switch( m->type + u_flag )
  {
    case M_STR + U_OFF + END_OFF :
            if ( strncmp(s, m->data.str, m->len) ) goto refill ;
            s += m->len ;  m++ ;
            goto reswitch ;

    case M_STR + U_OFF + END_ON :
            if ( strcmp(s, m->data.str) ) goto refill ;
            s += m->len ;  m++ ;
            goto reswitch ;

    case M_STR + U_ON + END_OFF :
            if ( !(s = str_str(s, m->data.str, m->len)) ) goto refill ;
            push(m, s+1, U_ON) ;
            s += m->len ; m++ ; u_flag = U_OFF ;
            goto reswitch ;

    case M_STR + U_ON + END_ON :
            if ( !str_end )  str_end = strchr(s, 0) ;
            ts = str_end - m->len ;
            if (ts < s || memcmp(ts,m->data.str,m->len+1)) goto refill ; 
            s = str_end ; m++ ; u_flag = U_OFF ;
            goto reswitch ;

    case M_CLASS + U_OFF + END_OFF :
            if ( !ison(*m->data.bvp, s[0] ) )  goto refill ;
            s++ ; m++ ;
            goto reswitch ;

    case M_CLASS + U_OFF + END_ON :
            if ( s[1] || !ison(*m->data.bvp,s[0]) )  goto refill ;
            s++ ; m++ ;
            goto reswitch ;

    case M_CLASS + U_ON + END_OFF :
            while ( !ison(*m->data.bvp,s[0]) )
                if ( s[0] == 0 )  goto refill ;
                else  s++ ;
            s++ ;
            push(m, s, U_ON) ;
            m++ ; u_flag = U_OFF ;
            goto reswitch ;

    case M_CLASS + U_ON + END_ON :
            if ( ! str_end )  str_end = strchr(s,0) ;
            if ( ! ison(*m->data.bvp, str_end[-1]) ) goto refill ;
            s = str_end ; m++ ; u_flag = U_OFF ;
            goto reswitch ;

    case M_ANY + U_OFF + END_OFF :
            if ( s[0] == 0 )  goto refill ;
            s++ ; m++ ;
            goto  reswitch ;

    case M_ANY + U_OFF + END_ON :
            if ( s[0] == 0 || s[1] != 0 )  goto refill ;
            s++ ; m++ ;
            goto reswitch ;

    case M_ANY + U_ON + END_OFF :
            if ( s[0] == 0 )  goto refill ;
            s++ ; 
            push(m, s, U_ON) ;
            m++ ; u_flag = U_OFF ;
            goto  reswitch ;

    case M_ANY + U_ON + END_ON :
            if ( s[0] == 0 )  goto refill ;
            if ( ! str_end )  str_end = strchr(s,0) ;
            s = str_end ; m++ ; u_flag = U_OFF ;
            goto reswitch ;

    case  M_START + U_OFF + END_OFF :
    case  M_START + U_ON  + END_OFF :
            if ( s != str )  goto  refill ;
            m++ ;  u_flag = U_OFF ;
            goto  reswitch ;

    case  M_START + U_OFF + END_ON :
    case  M_START + U_ON  + END_ON :
            if ( s != str || s[0] != 0 )  goto  refill ;
            m++ ; u_flag = U_OFF ;
            goto  reswitch ;

    case  M_END + U_OFF  :
            if ( s[0]  != 0 )  goto  refill ;
            m++ ; goto reswitch ;

    case  M_END + U_ON :
            s = strchr(s, 0) ;
            m++ ; u_flag = U_OFF ;
            goto reswitch ;

    CASE_UANY(M_U) :
            u_flag = U_ON ; m++ ;
            goto reswitch ;

    CASE_UANY(M_1J) :
            m += m->data.jump ;
            goto reswitch ;

    CASE_UANY(M_2JA) : /* take the non jump branch */
            /* don't stack an ACCEPT */
            if ( (tm = m + m->data.jump)->type == M_ACCEPT ) return 1 ;
            push(tm, s, u_flag) ;
            m++ ;
            goto reswitch ;

    CASE_UANY(M_2JB) : /* take the jump branch */
            /* don't stack an ACCEPT */
            if ( (tm = m + 1)->type == M_ACCEPT ) return 1 ;
            push(tm, s, u_flag) ;
            m += m->data.jump ;
            goto reswitch ;

    CASE_UANY(M_ACCEPT) :
            return 1 ;

    default :
            RE_panic("unexpected case in REtest") ;
  }
}

  

#ifdef  MAWK

char *is_string_split( p, lenp )
  register STATE *p ;
  unsigned *lenp ;
{
  if ( p[0].type == M_STR && p[1].type == M_ACCEPT )
  { *lenp = p->len ;
    return  p->data.str ;
  }
  else   return  (char *) 0 ;
}
#else /* mawk provides its own str_str */

char *str_str(target, key, klen)
  register char *target ;
  register char *key ;
  unsigned klen ;
{ int c = key[0] ;

  switch( klen )
  { case 0 :  return (char *) 0 ;
    case 1 :  return strchr(target, c) ;
    case 2 :  
              while ( target = strchr(target, c) )
                    if ( target[1] == key[1] ) return target ;
                    else target++ ;
              break ;

    default :
              klen-- ; key++ ;
              while ( target = strchr(target, c) )
                    if ( memcmp(target+1,key,klen) == 0 ) return target ;
                    else target++ ;
              break ;
  }
  return (char *) 0 ;
}
              

#endif  /* MAWK */
