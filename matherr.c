
/********************************************
matherr.c
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the Awk programming language as defined in
Aho, Kernighan and Weinberger, The AWK Programming Language,
Addison-Wesley, 1988.

See the accompaning file, LIMITATIONS, for restrictions
regarding modification and redistribution of this
program in source or binary form.
********************************************/

/*$Log:	matherr.c,v $
 * Revision 2.1  91/04/08  08:23:31  brennan
 * VERSION 0.97
 * 
*/

#include  "mawk.h"
#include  <math.h>

#if   FPE_TRAPS
#include <signal.h>

/* machine dependent changes might be needed here */

static void  fpe_catch( signal, why)
  int signal, why ;
{
  switch(why)
  {
    case FPE_ZERODIVIDE :
       rt_error("division by zero") ;

    case FPE_OVERFLOW  :
       rt_error("floating point overflow") ;

    default :
      rt_error("floating point exception") ;
  }
}

void   fpe_init()
{ (void) signal(SIGFPE, fpe_catch) ; }

#else

void  fpe_init()
{
  TURNOFF_FPE_TRAPS() ;
}
#endif

#if  HAVE_MATHERR

#if  ! FPE_TRAPS 

/* If we are not trapping math errors, we will shutup the library calls
*/

int  matherr( e )
  struct exception *e ;
{ return 1 ; } 

#else   /* print error message and exit */

int matherr( e )
  struct exception  *e ;
{ char *error ;

  switch( e->type )
  {
    case  DOMAIN :
    case  SING :
            error = "domain error" ;
            break ;

    case  OVERFLOW :
            error = "overflow" ;
            break ;

    case  TLOSS :
    case  PLOSS :
            error = "loss of significance" ;
            break ;

    case  UNDERFLOW :
            e->retval = 0.0 ;
            return  1 ;  /* ignore it */
  }

  if ( strcmp(e->name, "atan2") == 0 )
      rt_error("atan2(%g,%g) : %s" ,
         e->arg1, e->arg2, error ) ;
  else
      rt_error("%s(%g) : %s" , e->name, e->arg1, error) ;

  /* won't get here */
  return 0 ;
}
#endif   /* FPE_TRAPS */

#endif   /*  HAVE_MATHERR */
