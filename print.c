
/********************************************
print.c
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the AWK programming language.

Mawk is distributed without warranty under the terms of
the GNU General Public License, version 2, 1991.
********************************************/

/* $Log:	print.c,v $
 * Revision 3.4.1.1  91/09/14  17:24:01  brennan
 * VERSION 1.0
 * 
 * Revision 3.4  91/08/13  06:51:57  brennan
 * VERSION .9994
 * 
 * Revision 3.3  91/06/28  04:17:24  brennan
 * VERSION 0.999
 * 
 * Revision 3.2  91/06/10  15:59:29  brennan
 * changes for V7
 * 
 * Revision 3.1  91/06/07  10:28:06  brennan
 * VERSION 0.995
 * 
 * Revision 2.4  91/05/28  15:18:07  brennan
 * removed STRING_BUFF back to temp_buff.string_buff
 * 
 * Revision 2.3  91/05/28  09:05:04  brennan
 * removed main_buff
 * 
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
                    fwrite(string(p)->str, SIZE_T(1), SIZE_T(len), fp) ;
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
    { fwrite( p, SIZE_T(q-p), SIZE_T(1), fp) ; p = q+1 ; continue ; }

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

  temp_buff.string_buff[SPRINTF_SZ-1] = *target = 0 ;
  while ( 1 )
  { if ( ! (q = strchr(p, '%'))  )
       if ( argcnt == 0 )
       { strcpy(target, p) ; 
         /* check the result is not too large */
         if ( temp_buff.string_buff[SPRINTF_SZ-1] != 0 )
         { /* This may have damaged us -- try to croak out an error
              message and exit */
           rt_overflow("sprintf buffer", SPRINTF_SZ) ;
         }
         return ; 
       }
       else
         rt_error("too many arguments in call to sprintf(%s)", 
             format ) ; 

    if ( * ++q == '%' )
    { unsigned len ;

      (void) memcpy(target, p, SIZE_T(len = q-p) ) ;
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
            target += strlen(target) ;
            break ;
      case 'e' :
      case 'g' :
      case 'f' :
            if ( cp->type != C_DOUBLE ) cast1_to_d(cp) ;
            (void) sprintf(target, p, cp->dval ) ;
            target += strlen(target) ;
            break ;
      case  's' :
            if ( cp->type < C_STRING ) cast1_to_s(cp) ;
            (void) sprintf(target, p, string(cp)->str ) ;
            target += strlen(target) ;
            break ;
      default :
            rt_error("bad format string in call to sprintf(%s)", 
                format) ;
    }
    *q = save ; p = q ; argcnt-- ; cp++ ;
  }
}

