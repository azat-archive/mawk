
/********************************************
matherr.c
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the AWK programming language.

Mawk is distributed without warranty under the terms of
the GNU General Public License, version 2, 1991.
********************************************/

/*$Log:	matherr.c,v $
 * Revision 3.5.1.1  91/09/14  17:23:44  brennan
 * VERSION 1.0
 * 
 * Revision 3.5  91/08/16  11:02:09  brennan
 * Carl's addition of SW_FP_CHECK for V7 XNX23A
 * 
 * Revision 3.4  91/08/13  06:51:46  brennan
 * VERSION .9994
 * 
 * Revision 3.3  91/06/28  04:17:02  brennan
 * VERSION 0.999
 * 
 * Revision 3.2  91/06/19  10:23:49  brennan
 * changes for xenix_r2, call this version 0.997
 * 
 * Revision 3.1  91/06/07  10:27:53  brennan
 * VERSION 0.995
 * 
 * Revision 2.3  91/06/06  07:50:43  brennan
 * added infnan for libm functions on bsd43_vax
 * 
 * Revision 2.2  91/05/16  12:19:59  brennan
 * cleanup of machine dependencies
 * 
 * Revision 2.1  91/04/08  08:23:31  brennan
 * VERSION 0.97
 * 
*/

#include  "mawk.h"
#include  <math.h>

#if   FPE_TRAPS_ON
#include <signal.h>

/* machine dependent changes might be needed here */

static void  fpe_catch( signal, why)
  int signal, why ;
{

#if   NOINFO_SIGFPE
 /* some systems give no hook to find out what the exception
    was -- stuff like this is why people still use fortran 

    If this fits, #define NOINFO_SIGFPE 1 in  your config.h
*/
  rt_error("floating point exception, probably overflow") ;
#else

  switch(why)
  {
    case FPE_ZERODIVIDE :
       rt_error("division by zero") ;

    case FPE_OVERFLOW  :
       rt_error("floating point overflow") ;

    default :
      rt_error("floating point exception") ;
  }
#endif  
}

void   fpe_init()
{ (void) signal(SIGFPE, fpe_catch) ; }

#else  /* FPE_TRAPS_ON==0 */

void  fpe_init()
{
  TURN_OFF_FPE_TRAPS() ;
}
#endif

#if  HAVE_MATHERR

#if  ! FPE_TRAPS_ON

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


/* this is how one gets the libm calls to do the right
thing on bsd43_vax
*/

#ifdef   BSD43_VAX

#include <errno.h>

double infnan( arg )
  int arg ;
{
  switch(arg)
  {
    case  ERANGE : errno = ERANGE ; return HUGE ;
    case -ERANGE : errno = EDOM ; return -HUGE ;
    default :  errno = EDOM ; 
  }
  return 0.0 ;
}

#endif  /* BSD43_VAX */

/* This routine is for XENIX-68K 2.3A.
    Error check routine to be called after fp arithmetic.
*/

#if SW_FP_CHECK
/* Definitions of bit values in iserr() return value */

#define OVFLOW		2
#define UFLOW		4
#define ZERODIV		8
#define OVFLFIX		32
#define INFNAN		64

void
fpcheck()
{
	register int fperrval ;
	char *errdesc ;

	if ((fperrval = iserr()) == 0)
		return ;	/* no error */

	errdesc = (char *) 0 ;

	if (fperrval & INFNAN)
		errdesc = "arg is infinity or NAN" ;
	else if (fperrval & ZERODIV)
		errdesc = "division by zero" ;
	else if (fperrval & OVFLOW)
		errdesc = "overflow" ;
	else if (fperrval & UFLOW)
		;		/* ignored */

	if (errdesc)
		rt_error("%s", errdesc) ;
}

#endif
