
/********************************************
rexp3.c
copyright 1991, Michael D. Brennan

This is a source file for mawk an implementation of
the Awk programming language as defined in
Aho, Kernighan and Weinberger, The AWK Programming Language,
Addison-Wesley, 1988.

See the accompaning file, LIMITATIONS, for restrictions
regarding modification and redistribution of this
program in source or binary form.
********************************************/

/*  rexp3.c   */

/*  match a string against a machine   */

#include "rexp.h"
#include <string.h>


/*  check that a bit is on  */
#define  ison(b,x) ( (b)[(x)>>3] & ( 1 << ((x)&7)  ))


extern RT_STATE *RE_run_stack_base; 
extern RT_STATE *RE_run_stack_limit ;
RT_STATE  *RE_new_run_stack() ;


#define  push(mx,sx,ssx,ux)   if (++stackp == RE_run_stack_limit)\
                                stackp = RE_new_run_stack() ;\
               stackp->m=mx;stackp->s=sx;stackp->ss=ssx;stackp->u=ux;


#define   CASE_UANY(x)  case  x + U_OFF :  case  x + U_ON

/* returns start of first longest match and the length by
   reference.  If no match returns NULL and length zero */

char *REmatch(str, machine, lenp)
  char *str ;
  VOID   *machine ;
  unsigned *lenp ;
{ register STATE *m = (STATE *) machine ;
  register char *s = str ;
  char *ss ;
  register RT_STATE *stackp ;
  int u_flag ;
  char *str_end, *ts ;

  /* state of current best match stored here */
  char *cb_ss ;  /* the start */
  char *cb_e  ;  /* the end , pts at first char not matched */

  *lenp = 0 ;

  /* check for the easy case */
  if ( (m+1)->type == M_ACCEPT && m->type == M_STR )
  { if ( ts = str_str(s, m->data.str, m->len) ) *lenp = m->len ;
    return ts ;
  }
    
  u_flag = U_ON ; cb_ss = ss = str_end = (char *) 0 ;
  stackp = RE_run_stack_base - 1 ;
  goto  reswitch ;

refill :
  if ( stackp < RE_run_stack_base )  /* empty stack */
  { if ( cb_ss )  *lenp = cb_e - cb_ss ;
    return cb_ss ;
  }
  ss = stackp->ss ;
  s = stackp-- -> s ;
  if ( cb_ss )  /* does new state start too late ? */
      if ( ss )
      { if ( cb_ss < ss )  goto refill ; }
      else
      if ( cb_ss < s ) goto refill ;

  m = (stackp+1)->m ;
  u_flag  = (stackp+1)->u ;


reswitch  :

  switch( m->type + u_flag )
  {
    case M_STR + U_OFF + END_OFF :
            if ( strncmp(s, m->data.str, m->len) ) goto refill ;
	    if ( !ss )  
	        if ( cb_ss && s > cb_ss ) goto refill ;
		else ss = s ;
            s += m->len ;  m++ ;
            goto reswitch ;

    case M_STR + U_OFF + END_ON :
            if ( strcmp(s, m->data.str) ) goto refill ;
	    if ( !ss )  
	        if ( cb_ss && s > cb_ss ) goto refill ;
		else ss = s ;
            s += m->len ;  m++ ;
            goto reswitch ;

    case M_STR + U_ON + END_OFF :
            if ( !(s = str_str(s, m->data.str, m->len)) ) goto refill ;
            push(m, s+1,ss, U_ON) ;
	    if ( !ss )  
	        if ( cb_ss && s > cb_ss ) goto refill ;
		else ss = s ;
            s += m->len ; m++ ; u_flag = U_OFF ;
            goto reswitch ;

    case M_STR + U_ON + END_ON :
            if ( !str_end )  str_end = strchr(s, 0) ;
            ts = str_end - m->len ;
            if (ts < s || memcmp(ts,m->data.str,m->len+1)) goto refill ; 
	    if ( !ss )  
		if ( cb_ss && ts > cb_ss )  goto refill ;
		else  ss = ts ;
            s = str_end ; m++ ; u_flag = U_OFF ;
            goto reswitch ;

    case M_CLASS + U_OFF + END_OFF :
            if ( !ison(*m->data.bvp, s[0] ) )  goto refill ;
	    if ( !ss )
		if ( cb_ss && s > cb_ss )  goto refill ;
		else  ss = s ;
            s++ ; m++ ;
            goto reswitch ;

    case M_CLASS + U_OFF + END_ON :
            if ( s[1] || !ison(*m->data.bvp,s[0]) )  goto refill ;
	    if ( !ss )
		if ( cb_ss && s > cb_ss )  goto refill ;
		else  ss = s ;
            s++ ; m++ ;
            goto reswitch ;

    case M_CLASS + U_ON + END_OFF :
            while ( !ison(*m->data.bvp,s[0]) )
                if ( s[0] == 0 )  goto refill ;
                else  s++ ;

            s++ ;
            push(m, s, ss, U_ON) ;
	    if ( !ss )
		if ( cb_ss && s-1 > cb_ss )  goto refill ;
		else  ss = s-1 ;
            m++ ; u_flag = U_OFF ;
            goto reswitch ;

    case M_CLASS + U_ON + END_ON :
            if ( ! str_end )  str_end = strchr(s,0) ;
            if ( ! ison(*m->data.bvp, str_end[-1]) ) goto refill ;
	    if ( !ss )
		if ( cb_ss && str_end-1 > cb_ss )  goto refill ;
		else  ss = str_end-1 ;
            s = str_end ; m++ ; u_flag = U_OFF ;
            goto reswitch ;

    case M_ANY + U_OFF + END_OFF :
            if ( s[0] == 0 )  goto refill ;
	    if ( !ss )
		if ( cb_ss && s > cb_ss )  goto refill ;
		else ss = s ;
            s++ ; m++ ;
            goto  reswitch ;

    case M_ANY + U_OFF + END_ON :
            if ( s[0] == 0 || s[1] != 0 )  goto refill ;
	    if ( !ss )
		if ( cb_ss && s > cb_ss )  goto refill ;
		else ss = s ;
            s++ ; m++ ;
            goto reswitch ;

    case M_ANY + U_ON + END_OFF :
            if ( s[0] == 0 )  goto refill ;
            s++ ; 
            push(m, s, ss, U_ON) ;
	    if ( !ss )
		if ( cb_ss && s-1 > cb_ss )  goto refill ;
		else  ss = s-1 ;
            m++ ; u_flag = U_OFF ;
            goto  reswitch ;

    case M_ANY + U_ON + END_ON :
            if ( s[0] == 0 )  goto refill ;
            if ( ! str_end )  str_end = strchr(s,0) ;
	    if ( !ss )
		if ( cb_ss && str_end-1 > cb_ss )  goto refill ;
		else  ss = str_end - 1 ;
            s = str_end ; m++ ; u_flag = U_OFF ;
            goto reswitch ;

    case  M_START + U_OFF + END_OFF :
    case  M_START + U_ON  + END_OFF :
            if ( s != str )  goto  refill ;
	    ss = s ;
            m++ ;  u_flag = U_OFF ;
            goto  reswitch ;

    case  M_START + U_OFF + END_ON :
    case  M_START + U_ON  + END_ON :
            if ( s != str || s[0] != 0 )  goto  refill ;
	    ss = s ;
            m++ ; u_flag = U_OFF ;
            goto  reswitch ;

    case  M_END + U_OFF  :
            if ( s[0]  != 0 )  goto  refill ;
	    if ( !ss ) 
		if ( cb_ss && s > cb_ss )  goto refill ;
		else  ss = s ;
            m++ ; goto reswitch ;

    case  M_END + U_ON :
	    s = str_end ? str_end : (str_end =  strchr(s,0)) ;
	    if ( !ss ) 
		if ( cb_ss && s > cb_ss )  goto refill ;
		else  ss = s ;
            m++ ; u_flag = U_OFF ;
            goto reswitch ;

    CASE_UANY(M_U) :
	    if ( !ss ) 
		if ( cb_ss && s > cb_ss )  goto refill ;
		else  ss = s ;
            u_flag = U_ON ; m++ ;
            goto reswitch ;

    CASE_UANY(M_1J) :
            m += m->data.jump ;
            goto reswitch ;

    CASE_UANY(M_2JA) : /* take the non jump branch */
            push(m+m->data.jump, s, ss, u_flag) ;
            m++ ;
            goto reswitch ;

    CASE_UANY(M_2JB) : /* take the jump branch */
            push(m+1, s, ss, u_flag) ;
            m += m->data.jump ;
            goto reswitch ;

    case M_ACCEPT + U_OFF :
	    if ( !ss )  ss = s ;
	    if ( !cb_ss || ss < cb_ss || ss == cb_ss && s > cb_e )
	    { /* we have a new current best */
	      cb_ss = ss ; cb_e = s ;
	    }
	    goto  refill ;

    case  M_ACCEPT + U_ON :
	    if ( !ss )  ss = s ;
	    else
		s = str_end ? str_end : (str_end = strchr(s,0)) ;

	    if ( !cb_ss || ss < cb_ss || ss == cb_ss && s > cb_e )
	    { /* we have a new current best */
	      cb_ss = ss ; cb_e = s ;
	    }
	    goto  refill ;

    default :
            RE_panic("unexpected case in REmatch") ;
  }
}

