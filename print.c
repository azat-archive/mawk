
/********************************************
print.c
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the Awk programming language as defined in
Aho, Kernighan and Weinberger, The AWK Programming Language,
Addison-Wesley, 1988.

See the accompaning file, LIMITATIONS, for restrictions
regarding modification and redistribution of this
program in source or binary form.
********************************************/

/* $Log:	print.c,v $
 * Revision 2.2  91/04/09  12:39:23  brennan
 * added static to funct decls to satisfy STARDENT compiler
 * 
 * Revision 2.1  91/04/08  08:23:43  brennan
 * VERSION 0.97
 * 
*/

#include "mawk.h"
#include "bi_vars.h"
#include "bi_funct.h"
#include "memory.h"
#include "field.h"
#include "scan.h"
#include "files.h"

/*  static  functions */
static void  PROTO( print_cell, (CELL *, FILE *) ) ;
static void  PROTO( do_printf, (FILE *, char *, unsigned, CELL *) ) ;
static void  PROTO( do_sprintf, (char *, unsigned, CELL *) ) ;


static void print_cell(p, fp)
  register CELL *p ;
  register FILE *fp ;
{ register int len ;
  
  switch( p->type )
  {
    case C_NOINIT : break ;
    case C_MBSTRN :
    case C_STRING :
    case C_STRNUM :
        switch( len = string(p)->len )
        {
          case 0 :  break ;
          case 1 :
                    putc(string(p)->str[0],fp) ;
                    break ;

          default :
                    fwrite(string(p)->str, 1, len, fp) ;
        }
        break ;

    case C_DOUBLE :
        fprintf(fp, string(field + OFMT)->str, p->dval) ;
        break ;

    default :
        bozo("bad cell passed to print_cell") ;
  }
}

/* on entry to bi_print or bi_printf the stack is:

   sp[0] = an integer k
       if ( k < 0 )  output is to a file with name in sp[-1]
       { so open file and sp -= 2 }

   sp[0] = k >= 0 is the number of print args
   sp[-k]   holds the first argument 
*/

CELL *bi_print(sp)
  CELL *sp ; /* stack ptr passed in */
{ register CELL *p ;
  register int k ;
  FILE *fp ;

  if ( (k = sp->type) < 0 )
  { if ( (--sp)->type < C_STRING ) cast1_to_s(sp) ;
    fp = (FILE *) file_find( string(sp), k ) ;
    free_STRING(string(sp)) ;
    k = (--sp)->type ;
  }
  else  fp = stdout ;

  if ( k )  
  { p = sp - k ; /* clear k variables off the stack */
    sp = p - 1 ;
    while ( k-- > 1 ) 
    { print_cell(p,fp) ; print_cell(bi_vars+OFS,fp) ;
      cell_destroy(p) ; p++ ; }
    
    print_cell(p, fp) ;  cell_destroy(p) ;
  }
  else  
  { sp-- ;
    print_cell( &field[0], fp )  ; }

  print_cell( bi_vars + ORS , fp) ;
  return sp ;
}
  
/* the contents of format are preserved */
static void do_printf( fp, format, argcnt, cp)
  FILE *fp ;
  char *format ; unsigned argcnt ;
  CELL *cp ;  /* ptr to an array of arguments ( on the eval stack) */
{ register char *q ;
  char  save ;
  char *p = format ;

  while ( 1 )
  { if ( ! (q = strchr(p, '%'))  )
       if ( argcnt == 0 )
       { fputs(p, fp) ; return ; }
       else
         rt_error("too many arguments in call to printf(%s)", 
              format ) ; 

    if ( * ++q == '%' )
    { fwrite( p, q-p, 1, fp) ; p = q+1 ; continue ; }

    if ( argcnt == 0 )
        rt_error("too few arguments in call to printf(%s)", format) ; 

    if ( *q == '-' ) q++ ;
    while ( scan_code[*(unsigned char*)q] == SC_DIGIT )  q++ ;
    if ( *q == '.' )
    { q++ ;
      while ( scan_code[*(unsigned char*)q] == SC_DIGIT ) q++ ; }
    
    save = * ++q ;  *q = 0 ;
    switch( q[-1] )
    {
      case 'c' :  
      case 'd' :
      case 'o' :
      case 'x' :
            if ( cp->type != C_DOUBLE ) cast1_to_d(cp) ;
            (void) fprintf(fp, p, (int) cp->dval) ;
            break ;
      case 'e' :
      case 'g' :
      case 'f' :
            if ( cp->type != C_DOUBLE ) cast1_to_d(cp) ;
            (void) fprintf(fp, p, cp->dval) ;
            break ;
      case  's' :
            if ( cp->type < C_STRING ) cast1_to_s(cp) ;
            (void) fprintf(fp, p, string(cp)->str) ;
            break ;
      default :
            rt_error("bad format string in call to printf(%s)",
              format) ;
    }
    *q = save ; p = q ; argcnt-- ; cp++ ;
  }
}


CELL *bi_printf(sp)
  register CELL *sp ;
{ register int k ;
  register CELL *p ;
  FILE *fp ;

  if ( (k = sp->type) < 0 )
  { if ( (--sp)->type < C_STRING ) cast1_to_s(sp) ;
    fp = (FILE *) file_find( string(sp), k ) ;
    free_STRING(string(sp)) ;
    k = (--sp)->type ;
  }
  else  fp = stdout ;

  sp -= k-- ; /* sp points at the format string */
  if ( sp->type < C_STRING )  cast1_to_s(sp) ;
  do_printf(fp, string(sp)->str, k, sp+1) ;

  free_STRING(string(sp)) ;
  for ( p = sp+1 ; k-- ; p++ )  cell_destroy(p) ;
  return --sp ;
}

CELL *bi_sprintf(sp)
  CELL *sp ;
{ CELL *p ;
  int argcnt = sp->type ;
  void do_sprintf() ;

  sp -= argcnt-- ; /* sp points at the format string */
  if ( sp->type < C_STRING )  cast1_to_s(sp) ;
  do_sprintf(string(sp)->str, argcnt, sp+1) ;

  free_STRING(string(sp)) ;
  for ( p = sp+1 ; argcnt-- ; p++ )  cell_destroy(p) ;

  sp->ptr = (PTR) new_STRING( temp_buff.string_buff ) ;
  return sp ;
}


/* the contents of format are preserved */
static void do_sprintf( format, argcnt, cp)
  char *format ; 
  unsigned argcnt ;
  CELL *cp ;
{ register char *q ;
  char  save ;
  char *p = format ;
  register char *target = temp_buff.string_buff ;

  *target = 0 ;
  while ( 1 )
  { if ( ! (q = strchr(p, '%'))  )
       if ( argcnt == 0 )
       { strcpy(target, p) ; 
         /* check the result is not too large */
         if ( main_buff[-1] != 0 )
         { /* This may have damaged us -- try to croak out an error
              message and exit */
           rt_overflow("sprintf buffer", TEMP_BUFF_SZ) ;
         }
         return ; 
       }
       else
         rt_error("too many arguments in call to sprintf(%s)", 
             format ) ; 

    if ( * ++q == '%' )
    { unsigned len ;

      (void) memcpy(target, p, len = q-p ) ;
      p = q + 1 ; *(target += len) = 0 ;
      continue ;
    }

    if ( argcnt == 0 )
      rt_error("too few arguments in call to sprintf(%s)", format) ; 

    if ( *q == '-' ) q++ ;
    while ( scan_code[*(unsigned char*)q] == SC_DIGIT )  q++ ;
    if ( *q == '.' )
    { q++ ;
      while ( scan_code[*(unsigned char*)q] == SC_DIGIT ) q++ ; }
    
    save = * ++q ;  *q = 0 ;
    switch( q[-1] )
    {
      case 'c' :  
      case 'd' :
      case 'o' :
      case 'x' :
            if ( cp->type != C_DOUBLE ) cast1_to_d(cp) ;
            (void) sprintf(target, p, (int) cp->dval ) ;
            target = strchr(target, 0) ;
            break ;
      case 'e' :
      case 'g' :
      case 'f' :
            if ( cp->type != C_DOUBLE ) cast1_to_d(cp) ;
            (void) sprintf(target, p, cp->dval ) ;
            target = strchr(target, 0) ;
            break ;
      case  's' :
            if ( cp->type < C_STRING ) cast1_to_s(cp) ;
            (void) sprintf(target, p, string(cp)->str ) ;
            target = strchr(target, 0) ;
            break ;
      default :
            rt_error("bad format string in call to sprintf(%s)", 
                format) ;
    }
    *q = save ; p = q ; argcnt-- ; cp++ ;
  }
}

