
/********************************************
main.c
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the AWK programming language.

Mawk is distributed without warranty under the terms of
the GNU General Public License, version 2, 1991.
********************************************/

/* $Log:	main.c,v $
 * Revision 3.4.1.1  91/09/14  17:23:39  brennan
 * VERSION 1.0
 * 
 * Revision 3.4  91/08/13  06:51:43  brennan
 * VERSION .9994
 * 
 * Revision 3.3  91/06/29  09:47:14  brennan
 * Only track NR if needed
 * 
 * Revision 3.2  91/06/28  04:16:58  brennan
 * VERSION 0.999
 * 
 * Revision 3.1  91/06/08  06:14:36  brennan
 * VERSION 0.995
 * 
 * Revision 2.6  91/06/08  06:00:18  brennan
 * changed how eof is marked on main_fin
 * 
 * Revision 2.5  91/06/05  07:20:22  brennan
 * better error messages when regular expression compiles fail
 * 
 * Revision 2.4  91/05/30  11:14:38  brennan
 * added static 
 * 
 * Revision 2.3  91/05/16  12:19:56  brennan
 * cleanup of machine dependencies
 * 
 * Revision 2.2  91/04/22  08:08:58  brennan
 * cannot close(3) or close(4) because of bug in TurboC++ 1.0
 * 
 * Revision 2.1  91/04/08  08:23:27  brennan
 * VERSION 0.97
 * 
*/



/*  main.c  */

#include "mawk.h"
#include "code.h"
#include "init.h"
#include "fin.h"
#include "bi_vars.h"
#include "field.h"
#include "files.h"
#include <stdio.h>

#if  MSDOS 
void  reargv(int *, char ***) ;
#endif


static void  PROTO( execution, (void) ) ;
static void  PROTO( main_loop, (void) ) ;

extern int program_fd ;
char *progname ;
short mawk_state ; /* 0 is compiling */
short NR_reference ;

jmp_buf   exit_jump, next_jump ;
int  exit_code ;


main(argc , argv )
  int argc ; char **argv ;
{ 

#if   MSDOS
  progname = "mawk" ;
#if      HAVE_REARGV
  reargv(&argc, &argv) ;
#endif
#else	/* MSDOS */
#ifdef THINK_C
  progname = "MacMAWK";
#else	/* THINK_C */
  { char *strrchr() ;
    char *p = strrchr(argv[0], '/') ;
    progname = p ? p+1 : argv[0] ; }
#endif
#endif

  initialize(argc, argv) ;

  if ( parse() || compile_error_count )  mawk_exit(1) ;

  compile_cleanup() ;
  mawk_state = EXECUTION ;
  execution() ;

  mawk_exit( exit_code ) ;
  return 0 ;
}


static  void  execution()
{

  if ( setjmp(exit_jump) )
  { if ( begin_start ) zfree(begin_start, begin_size) ;
    goto the_exit ;
  }

  if ( begin_start )
  { (void) execute(begin_start, eval_stack-1, 0) ;
    zfree( begin_start , begin_size ) ;
    begin_start = (INST *) 0 ;
  }

  if ( main_start || end_start )  main_loop() ;

the_exit:

  if ( setjmp(exit_jump) )  mawk_exit(exit_code) ;

  if ( main_start )  zfree(main_start, main_size) ;
  if ( end_start ) (void) execute(end_start, eval_stack-1, 0) ;
}


static void  main_loop()
{ register char *p ;
  unsigned len ;

  /* the main file stream might already be open by a call of
     getline in the BEGIN block */

  if ( ! main_fin )  open_main()  ;

  if ( main_start )
  {

     if ( NR_reference )
     {
         (void)  setjmp(next_jump) ;

	 while ( p = FINgets( main_fin, &len ) )
	 { 
	   if ( TEST2(bi_vars + NR) != TWO_DOUBLES )
			cast2_to_d(bi_vars + NR) ;

	   bi_vars[NR].dval += 1.0 ;
	   bi_vars[FNR].dval += 1.0 ;

	   set_field0(p, len) ;
	   (void) execute( main_start, eval_stack-1, 0) ;
	 }
     }
     else /* don't worry about NR */
     {
         (void)  setjmp(next_jump) ;

	 while ( p = FINgets( main_fin, &len ) )
	 { 
	   set_field0(p, len) ;
	   (void) execute( main_start, eval_stack-1, 0) ;
	 }
     }
  }
  else  /* eat main and set correct state */
  { long nr ;
    char *f0 ;
    unsigned f0_len ;

    if ( TEST2(bi_vars+NR) != TWO_DOUBLES ) cast2_to_d(bi_vars+NR) ;
    nr = (long) bi_vars[NR].dval ;
    f0 = (char *) 0 ;

    while ( p = FINgets(main_fin, &len) )
    {
      f0 = p ; f0_len = len ;
      nr++ ; bi_vars[FNR].dval += 1.0 ;
    }
    bi_vars[NR].dval = (double) nr ;
    set_field0(f0, f0_len) ;
  }
}


void  mawk_exit(x)
  int x ;
{
#if  !  MSDOS
#ifndef THINK_C
  close_out_pipes() ;  /* no effect, if no out pipes */
#endif
#endif
  exit(x) ;
}
