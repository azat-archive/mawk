
/********************************************
Idefault.h
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the AWK programming language.

Mawk is distributed without warranty under the terms of
the GNU General Public License, version 2, 1991.
********************************************/


/* $Log:	Idefault.h,v $
 * Revision 3.7.1.1  91/09/25  12:59:16  brennan
 * VERSION 1.0
 * 
 * Revision 3.7  91/08/16  08:49:51  brennan
 * Carl's addition of SW_FP_CHECK for XNX23A
 * 
 * Revision 3.6  91/08/13  09:04:05  brennan
 * VERSION .9994
 * 
 * Revision 3.5  91/08/03  06:10:46  brennan
 * changed CHECK_DIVZERO macro
 * 
 * Revision 3.4  91/08/03  05:35:59  brennan
 * changed name to Idefault.h 
 * 
 * Revision 3.3  91/06/28  04:36:28  brennan
 * adjustments with __STDC__
 * 
 * Revision 3.3  91/06/19  10:21:37  brennan
 * changes for xenix_r2.h and gcc
 * 
 * Revision 3.2  91/06/15  09:24:34  brennan
 * Carl's diffs for V7
 * 
 * 06/11/91  C. Mascott		add default D2BOOL
 *
 * Revision 3.1  91/06/07  10:38:46  brennan
 * VERSION 0.995
 * 
*/

/* The most common configuration is defined here:
 
   no function prototypes
   have void*
   have matherr(), strtod(), fmod()
   uses <varargs.h>

   fpe_traps default to off

   memory is not small

   OS is some flavor of Unix

*/

/* WARNING:  To port to a new configuration, don't make changes
   here.  This file is included at the end of your new 
   config.h 

   Read the file   mawk/INSTALL
*/


/*------------- compiler ----------------------------*/
/* do not have function prototypes */

#ifndef  HAVE_PROTOS
#define  HAVE_PROTOS		0
#endif

/* have type   void *    */
#ifndef  HAVE_VOID_PTR
#define  HAVE_VOID_PTR		1
#endif

/* logical test of double is OK */
#ifndef D2BOOL
#define D2BOOL(x)		(x)
#endif

/*---------------- library ----------------------*/


#ifndef  HAVE_MATHERR
#define  HAVE_MATHERR		1
#endif

#ifndef  HAVE_STRTOD
#define  HAVE_STRTOD		1
#endif

#ifndef  HAVE_FMOD
#define  HAVE_FMOD		1
#endif

/* uses <varargs.h> instead of <stdarg.h> */
#ifndef  HAVE_STDARG_H	
#define  HAVE_STDARG_H		0
#endif

/* has <string.h>, 
   doesn't have <stdlib.h>  
   has <fcntl.h>
*/

#ifndef  HAVE_STRING_H
#define  HAVE_STRING_H		1
#endif

#ifndef  HAVE_STDLIB_H
#define  HAVE_STDLIB_H		0
#endif

#ifndef  HAVE_FCNTL_H
#define  HAVE_FCNTL_H		1
#endif

/*------------- machine ------------------------*/

/* default is IEEE754 and data space is not scarce */

#ifndef  FPE_TRAPS_ON
#define  FPE_TRAPS_ON		0
#endif

#ifndef   NOINFO_SIGFPE
#define   NOINFO_SIGFPE		0
#endif

#if   ! FPE_TRAPS_ON
#undef   NOINFO_SIGFPE  
#define  NOINFO_SIGFPE          0 /* make sure no one does
				     something stupid */
#endif

#if      NOINFO_SIGFPE
#define  CHECK_DIVZERO(x)	if( (x) == 0.0 )rt_error(dz_msg);else
#endif

/* SW_FP_CHECK is specific to V7 and XNX23A
	(1) is part of STDC_MATHERR def.
	(2) enables calls to XENIX-68K 2.3A clrerr(), iserr()
 */
#ifndef  SW_FP_CHECK
#define  SW_FP_CHECK		0
#endif

#ifndef  TURN_OFF_FPE_TRAPS
#define  TURN_OFF_FPE_TRAPS()	/* nothing */
#endif

#ifndef  HAVE_SMALL_MEMORY
#define  HAVE_SMALL_MEMORY	0
#endif


/*------------------------------------------------*/


#ifndef  __STDC__
#define  __STDC__  0
#endif

#if   __STDC__  

#undef  HAVE_PROTOS
#define HAVE_PROTOS		1
#undef  HAVE_VOID_PTR
#define HAVE_VOID_PTR		1
#undef  HAVE_STDARG_H
#define HAVE_STDARG_H		1
#undef  HAVE_STRING_H
#define HAVE_STRING_H		1
#endif  

#if   __STDC__  &&  ! __GNUC__
/* with gcc __STDC__ can be defined, but stdlib.h is missing */
#undef  HAVE_STDLIB_H
#define HAVE_STDLIB_H		1
#endif




/* the painfull case: we need to catch fpe's and look at errno
   after lib calls */

#define  STDC_MATHERR	((SW_FP_CHECK || FPE_TRAPS_ON) && HAVE_MATHERR==0)

#ifndef  MSDOS
#define  MSDOS		0
#endif

#if	MSDOS
#ifndef  HAVE_REARGV
#define  HAVE_REARGV	0
#endif
#endif


#if  HAVE_PROTOS
#define  PROTO(name, args)  name  args
#else
#define  PROTO(name, args)  name()
#endif

/* ultrix pukes if __STDC__ is defined 0  */
/* SCO UNIX's cc (from Microsoft) sneezes if __STDC__ is re-#define-d */
#if (__STDC__ == 0)
#ifndef M_I386
#undef __STDC__
#endif
#endif

/* for Think C on the Macintosh, sizeof(size_t) != sizeof(unsigned
 * Rather than unilaterally imposing size_t, when not all compilers would
 * necessarily have it defined, we use the SIZE_T() macro where appropriate
 * to typecast function arguments
 */
#ifndef SIZE_T
#define SIZE_T(x) (x)
#endif
