
/********************************************
init.c
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the Awk programming language as defined in
Aho, Kernighan and Weinberger, The AWK Programming Language,
Addison-Wesley, 1988.

See the accompaning file, LIMITATIONS, for restrictions
regarding modification and redistribution of this
program in source or binary form.
********************************************/


/* $Log:	init.c,v $
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

#define PROGRAM_FROM_CMDLINE   1

/* static protos */
static void PROTO( no_program, (void) ) ;
static void PROTO( process_cmdline , (int, char **) ) ;
static void PROTO( set_FS, (char *) ) ;
static void PROTO( set_dump, (char *) ) ;

#if  DOS  &&  ! HAVE_REARGV
#include <fcntl.h>
static  void  PROTO(emit_prompt, (void) ) ;
#endif

union tbuff  temp_buff ;
char *main_buff = temp_buff.string_buff + TEMP_BUFF_SZ ;

void initialize(argc, argv)
  int argc ; char **argv ;
{
  bi_vars_init() ; /* load the builtin variables */
  bi_funct_init() ; /* load the builtin functions */
  kw_init() ; /* load the keywords */
  field_init() ; 
  process_cmdline(argc, argv)  ;   

  jmp_stacks_init() ;
  code_init() ;
  fpe_init() ;

#if  NO_STRTOD
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
  char xbuff[20] ;


  for( i = 1 ; i < argc && argv[i][0] == '-' ; i++ )
  { p = & argv[i][1] ;
    if ( *p == 'F' )  set_FS(p+1) ;

    else if ( *p == 'D' )  set_dump(p+1) ;
    else if ( *p == 'f' )
    { if ( i == argc - 1 )  no_program() ;
      scan_init(! PROGRAM_FROM_CMDLINE, argv[i+1] ) ;
      i += 2 ;
      goto  set_ARGV ;
    }
  }

#if  DOS   &&  ! HAVE_REARGV
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
  xbuff[0] = '0' ; xbuff[1] = 0 ;
  cp = array_find( st_p->stval.array, xbuff, 1) ;
  cp->type = C_STRING ;
  cp->ptr = (PTR) new_STRING( progname ) ;

  /* ARGV[0] is set, do the rest 
     The type of ARGV[1] ... should be C_MBSTRN
     because the user might enter numbers from the command line */
  { int arg_count = 1 ;

    for( ; i < argc ; i++, arg_count++ )
    { 
      if ( arg_count < 10 )  xbuff[0] = arg_count + '0' ;
      else (void) sprintf(xbuff, "%u" , arg_count ) ;
      cp = array_find( st_p->stval.array, xbuff, 1) ;
      cp->type = C_MBSTRN ;
      cp->ptr = (PTR) new_STRING( argv[i] ) ;
    }
    bi_vars[ARGC].type = C_DOUBLE ;
    bi_vars[ARGC].dval = (double) arg_count ;
  }
}

#if  DOS  &&  ! HAVE_REARGV

static void  emit_prompt()
{  static char prompt[] = DOS_PROMPT ;
   int fd = open("CON", O_WRONLY, 0) ;

   (void) write(fd, prompt, strlen(prompt)) ;
   (void) close(fd) ;
}
#endif

