
/********************************************
init.c
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the AWK programming language.

Mawk is distributed without warranty under the terms of
the GNU General Public License, version 2, 1991.
********************************************/


/* $Log:	init.c,v $
 * Revision 3.6.1.1  91/09/14  17:23:28  brennan
 * VERSION 1.0
 * 
 * Revision 3.6  91/08/13  06:51:34  brennan
 * VERSION .9994
 * 
 * Revision 3.5  91/08/03  05:20:11  brennan
 * allow -ffile on cmdline
 * 
 * Revision 3.4  91/07/19  07:51:09  brennan
 * escape sequence now recognized in command line assignments
 * 
 * Revision 3.3  91/06/28  04:16:49  brennan
 * VERSION 0.999
 * 
 * Revision 3.2  91/06/19  10:23:41  brennan
 * changes for xenix_r2, call this version 0.997
 * 
 * Revision 3.1  91/06/07  10:27:38  brennan
 * VERSION 0.995
 * 
 * Revision 2.7  91/06/06  09:42:54  brennan
 * proto for print_version
 * 
 * Revision 2.6  91/05/29  14:25:42  brennan
 * -V option for version
 * 
 * Revision 2.5  91/05/28  09:04:52  brennan
 * removed main_buff
 * 
 * Revision 2.4  91/05/22  07:46:50  brennan
 * dependency cleanup for DOS
 * 
 * Revision 2.4  91/05/16  12:19:51  brennan
 * cleanup of machine dependencies
 * 
 * Revision 2.3  91/05/15  12:07:39  brennan
 * dval hash table for arrays
 * 
 * Revision 2.2  91/04/09  12:39:08  brennan
 * added static to funct decls to satisfy STARDENT compiler
 * 
 * Revision 2.1  91/04/08  08:23:15  brennan
 * VERSION 0.97
 * 
*/


/* init.c */
#include "mawk.h"
#include "code.h"
#include "init.h"
#include "memory.h"
#include "symtype.h"
#include "bi_vars.h"
#include "field.h"

#ifdef THINK_C
#include <console.h>
#endif

#define PROGRAM_FROM_CMDLINE   1

/* static protos */
static void PROTO( no_program, (void) ) ;
static void PROTO( process_cmdline , (int, char **) ) ;
static void PROTO( set_FS, (char *) ) ;
static void PROTO( set_dump, (char *) ) ;

extern  void PROTO( print_version, (void) ) ;

#if  MSDOS  &&  ! HAVE_REARGV
#include <fcntl.h>
static  void  PROTO(emit_prompt, (void) ) ;
#endif


void initialize(argc, argv)
  int argc ; char **argv ;
{
  bi_vars_init() ; /* load the builtin variables */
  bi_funct_init() ; /* load the builtin functions */
  kw_init() ; /* load the keywords */
  field_init() ; 

#ifdef THINK_C
  fputc('\n',stderr);	/* (causes Think C console window to appear) */
  SetWTitle( FrontWindow(), "\pMacMAWK" );
  argc = ccommand(&argv);
#endif 
  process_cmdline(argc, argv)  ;   

  jmp_stacks_init() ;
  code_init() ;
  fpe_init() ;

#if  ! HAVE_STRTOD
  strtod_init() ;
#endif

}

void  compile_cleanup()
/* program has parsed OK, free some memory
   we don't need anymore */
{
   scan_cleanup() ;
   jmp_stacks_cleanup() ;
   code_cleanup() ;
}


static void no_program()
{ errmsg( 0, "no program") ; mawk_exit(1) ; }

int  dump_code ;  /* if on dump internal code */
#ifdef   DEBUG
int  dump_RE   ;  /* if on dump compiled REs  */
#endif

static void set_FS(s)
  char *s ;
{
  rm_escape(s) ; /* recognize escape sequences */
  cell_destroy(field+FS) ;
  field[FS].type = C_STRING ;
  field[FS].ptr = (PTR) new_STRING(s) ;
  cast_for_split( cellcpy(&fs_shadow, field+FS) ) ;
}

#ifdef  DEBUG
static void set_dump(s)
  char *s ;
{
  while ( 1 )
  { switch ( *s )
    { case 'p' :
      case 'P' :  yydebug = 1 ; break ;

      case  'c' :
      case  'C' :  dump_code = 1 ; break ;

      case  'r' :
      case  'R' :  dump_RE = 1 ; break ;

      case   0  :  
             if ( s[-1] == 'D' ) dump_code = 1 ;
             return ;

      default :  break ;
    }
    s++ ;
  }
}
#else
static void  set_dump(s)
  char *s ;
{ dump_code = 1 ; }
#endif

static void process_cmdline(argc, argv)
  int argc ; char **argv ;
{ extern int program_fd ; 
  int i ;  /* index to walk command line */
  char *p ;
  CELL *cp ;
  SYMTAB *st_p ;
  CELL argi ; /* sets up ARGV */


  for( i = 1 ; i < argc && argv[i][0] == '-' ; i++ )
  { 
    p = & argv[i][1] ;
    switch( *p )
    {
      case 'F' :
	  set_FS(p+1) ; break  ;

      case 'D' :
	  set_dump(p+1) ; break ;

      case 'V' :
	  print_version() ; break ; /* no return */

      case 'f' :
	 /* check if program is glued to -f */
	  if ( p[1] == 0 ) /* normal -- not glued */
	  { if ( i == argc - 1 )  no_program() ;
	    scan_init(! PROGRAM_FROM_CMDLINE, argv[i+1] ) ;
	    i += 2 ;
	  }
	  else /* glued */
	  { scan_init(! PROGRAM_FROM_CMDLINE, p+1) ; i++ ; }

	  goto  set_ARGV ;

      default : /* start of expression with uminus (hah) */
	  goto out ;
    }
  }

out:

#if  MSDOS   &&  ! HAVE_REARGV
/* allows short programs to be typed in without mucking stdin */
  emit_prompt() ;
  scan_init(! PROGRAM_FROM_CMDLINE, "CON") ;
#else   /* the real world */

  if ( i == argc )  no_program() ;
  scan_init(PROGRAM_FROM_CMDLINE, argv[i]) ;
  i++ ;
#endif

set_ARGV:

  /* now set up ARGC and ARGV  */
  st_p = insert( "ARGV" ) ;
  st_p->type = ST_ARRAY ;
  Argv = st_p->stval.array = new_ARRAY() ;
  argi.type = C_DOUBLE ;
  argi.dval = 0.0 ;
  cp = array_find( st_p->stval.array, &argi, CREATE) ;
  cp->type = C_STRING ;
  cp->ptr = (PTR) new_STRING( progname ) ;

  /* ARGV[0] is set, do the rest 
     The type of ARGV[1] ... should be C_MBSTRN
     because the user might enter numbers from the command line */

    for( argi.dval = 1.0 ; i < argc ; i++, argi.dval += 1.0 )
    { 
      cp = array_find( st_p->stval.array, &argi, CREATE) ;
      cp->type = C_MBSTRN ;
      cp->ptr = (PTR) new_STRING( argv[i] ) ;
    }
    bi_vars[ARGC].type = C_DOUBLE ;
    bi_vars[ARGC].dval = argi.dval ;

}

#if  MSDOS  &&  ! HAVE_REARGV

static void  emit_prompt()
{  static char prompt[] = "mawk> " ;
   int fd = open("CON", O_WRONLY, 0) ;

   (void) write(fd, prompt, strlen(prompt)) ;
   (void) close(fd) ;
}
#endif

