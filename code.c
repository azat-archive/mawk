
/********************************************
code.c
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the Awk programming language as defined in
Aho, Kernighan and Weinberger, The AWK Programming Language,
Addison-Wesley, 1988.

See the accompaning file, LIMITATIONS, for restrictions
regarding modification and redistribution of this
program in source or binary form.
********************************************/


/* $Log:	code.c,v $
 * Revision 2.1  91/04/08  08:22:46  brennan
 * VERSION 0.97
 * 
*/

/*  code.c  */

#include "mawk.h"
#include "code.h"
#include "init.h"


#define   CODE_SZ      (PAGE_SZ*sizeof(INST))

INST *code_ptr  ;
INST *main_start , *main_code_ptr ;
INST *begin_start , *begin_code_ptr ;
INST *end_start , *end_code_ptr ;
unsigned  main_size, begin_size, end_size ;

void  PROTO(fdump, (void) ) ;

void  code_init()
{ 
  main_code_ptr = main_start = (INST *) zmalloc(CODE_SZ) ;
  begin_code_ptr = begin_start = (INST *) zmalloc(CODE_SZ) ;
  end_code_ptr = end_start = (INST *) zmalloc(CODE_SZ) ;
  code_ptr = main_code_ptr ;
}

void code_cleanup()
{
  if ( dump_code )  fdump() ; /* dumps all functions */

  begin_code_ptr++->op = _HALT ;
  if ( (begin_size = begin_code_ptr - begin_start) == 1 ) /* empty */
  {
      zfree( begin_start, CODE_SZ ) ;
      begin_start = (INST *) 0 ;
  }
  else
  if ( begin_size > PAGE_SZ ) overflow("BEGIN code" , PAGE_SZ) ;
  else
  {  begin_size *= sizeof(INST) ;
     begin_start = (INST *) zrealloc(begin_start,CODE_SZ,begin_size) ;
     if ( dump_code )
     { fprintf(stderr, "BEGIN\n") ;
       da(begin_start, stderr) ; 
     }
  }

  end_code_ptr++->op = _HALT ;
  if ( (end_size = end_code_ptr - end_start) == 1 ) /* empty */
  {
      zfree( end_start, CODE_SZ ) ;
      end_start = (INST *) 0 ;
  }
  else
  if ( end_size > PAGE_SZ ) overflow("END code" , PAGE_SZ) ;
  else
  {  end_size *= sizeof(INST) ;
     end_start = (INST *) zrealloc(end_start, CODE_SZ, end_size) ;
     if ( dump_code )
     { fprintf(stderr, "END\n") ;
       da(end_start, stderr) ;
     }
  }

  code_ptr++->op = _HALT ;
  if ( (main_size = code_ptr - main_start) == 1 ) /* empty */
  {
      zfree( main_start, CODE_SZ ) ;
      main_start = (INST *) 0 ;
  }
  else
  if ( main_size > PAGE_SZ ) overflow("MAIN code" , PAGE_SZ) ;
  else
  {  main_size *= sizeof(INST) ;
     main_start = (INST *) zrealloc(main_start, CODE_SZ, main_size) ;
     if ( dump_code )
     { fprintf(stderr, "MAIN\n") ;
       da(main_start, stderr) ;
     }
  }
}
