
/********************************************
code.c
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the AWK programming language.

Mawk is distributed without warranty under the terms of
the GNU General Public License, version 2, 1991.
********************************************/


/* $Log:	code.c,v $
 * Revision 3.4.1.1  91/09/14  17:22:52  brennan
 * VERSION 1.0
 * 
 * Revision 3.4  91/08/13  06:50:59  brennan
 * VERSION .9994
 * 
 * Revision 3.3  91/06/28  04:16:15  brennan
 * VERSION 0.999
 * 
 * Revision 3.2  91/06/27  08:23:57  brennan
 * bigger page size for main code 
 * bigger loop stacks -- both changes need to handle AWF
 * 
 * Revision 3.1  91/06/07  10:27:03  brennan
 * VERSION 0.995
 * 
 * Revision 2.1  91/04/08  08:22:46  brennan
 * VERSION 0.97
 * 
*/

/*  code.c  */

#include "mawk.h"
#include "code.h"
#include "init.h"


#define   CODE_SZ      (PAGE_SZ*sizeof(INST))
#define   MAIN_CODE_SZ  (MAIN_PAGE_SZ*sizeof(INST))

INST *code_ptr  ;
INST *main_start , *main_code_ptr ;
INST *begin_start , *begin_code_ptr ;
INST *end_start , *end_code_ptr ;
unsigned main_size , begin_size, end_size ;
    /* when the code is done executing its freed,
       that's why this is global */

void  PROTO(fdump, (void) ) ;

void  code_init()
{ 
  code_ptr = main_code_ptr = main_start 
    = (INST *) zmalloc(MAIN_CODE_SZ) ;
#if 0
  begin_code_ptr = begin_start = (INST *) zmalloc(CODE_SZ) ;
  end_code_ptr = end_start = (INST *) zmalloc(CODE_SZ) ;
  code_ptr = main_code_ptr ;
#endif
}

void code_cleanup()
{ 
  if ( dump_code )  fdump() ; /* dumps all functions */

  if ( begin_start )
  { 
    begin_code_ptr++->op = _HALT ;
    if ( (begin_size = begin_code_ptr - begin_start) > PAGE_SZ )
	overflow("BEGIN code", PAGE_SZ) ;

    begin_size *= sizeof(INST) ;
    begin_start = (INST *) zrealloc(begin_start, CODE_SZ , begin_size) ;

    if ( dump_code )
    { fprintf(stderr, "BEGIN\n") ;
      da(begin_start, stderr) ; 
    }
  }

  if ( end_start )
  { 
    end_code_ptr++->op = _HALT ;
    if ( (end_size = end_code_ptr - end_start) > PAGE_SZ )
	overflow("END code", PAGE_SZ) ;

    end_size *= sizeof(INST) ;
    end_start = (INST *) zrealloc(end_start, CODE_SZ , end_size) ;

    if ( dump_code )
    { fprintf(stderr, "END\n") ;
      da(end_start, stderr) ; 
    }
  }

    /* main_start is always set */
    code_ptr++->op = _HALT ;
    if ( (main_size = code_ptr - main_start) == 1 ) /* empty */
    {
      zfree( main_start, MAIN_CODE_SZ ) ;
      main_start = (INST *) 0 ;
    }
    else
    if ( main_size > MAIN_PAGE_SZ ) overflow("MAIN code" , MAIN_PAGE_SZ) ;
    else
    {  
       main_size *= sizeof(INST) ;
       main_start = (INST *) zrealloc(main_start, MAIN_CODE_SZ, 
		    main_size ) ;

       if ( dump_code )
       { fprintf(stderr, "MAIN\n") ;
         da(main_start, stderr) ;
       }
    }
}
