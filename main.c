
/********************************************
main.c
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the Awk programming language as defined in
Aho, Kernighan and Weinberger, The AWK Programming Language,
Addison-Wesley, 1988.

See the accompaning file, LIMITATIONS, for restrictions
regarding modification and redistribution of this
program in source or binary form.
********************************************/


/* $Log:	main.c,v $
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

#if  DOS 
void  reargv(int *, char ***) ;
#endif


void  PROTO( process, (void) ) ;
void  PROTO( main_loop, (void) ) ;

extern int program_fd ;
char *progname ;

jmp_buf   exit_jump, next_jump ;
int  exit_code ;


main(argc , argv )
  int argc ; char **argv ;
{ 

#if   DOS
  progname = "mawk" ;
#if      HAVE_REARGV
  reargv(&argc, &argv) ;
#endif
#else
  { char *strrchr() ;
    char *p = strrchr(argv[0], '/') ;
    progname = p ? p+1 : argv[0] ; }
#endif

  initialize(argc, argv) ;

  if ( parse() || compile_error_count )  exit(1) ;

  compile_cleanup() ;
  process() ;

  mawk_exit( exit_code ) ;
  return 0 ;
}


static  void  process()
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

  if ( main_fin == (FIN *) -1 && ! open_main()
       || ! main_fin )  return ;

  if ( main_start )
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
  else  /* eat main to set NR and FNR before executing END */
  { long nr ;

    if ( TEST2(bi_vars+NR) != TWO_DOUBLES ) cast2_to_d(bi_vars+NR) ;
    nr = (long) bi_vars[NR].dval ;
    while ( FINgets( main_fin, &len ) )
    { nr++ ; bi_vars[FNR].dval += 1.0 ; }
    bi_vars[NR].dval = (double) nr ;
  }
}


void  mawk_exit(x)
  int x ;
{
#if  !  DOS
  close_out_pipes() ;  /* no effect, if no out pipes */
#endif
  exit(x) ;
}
