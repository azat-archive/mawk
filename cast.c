
/********************************************
cast.c
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the AWK programming language.

Mawk is distributed without warranty under the terms of
the GNU General Public License, version 2, 1991.
********************************************/


/*   $Log:	cast.c,v $
 * Revision 3.4.1.1  91/09/14  17:22:49  brennan
 * VERSION 1.0
 * 
 * Revision 3.4  91/08/16  10:32:08  brennan
 * SW_FP_CHECK for V7 XNX23A
 * 
 * Revision 3.3  91/08/13  06:50:56  brennan
 * VERSION .9994
 * 
 * Revision 3.2  91/06/28  04:16:12  brennan
 * VERSION 0.999
 * 
 * Revision 3.1  91/06/07  10:27:00  brennan
 * VERSION 0.995
 * 
 * Revision 2.5  91/06/04  06:42:57  brennan
 * removed <string.h>
 * 
 * Revision 2.4  91/05/28  15:17:32  brennan
 * removed STRING_BUFF back to temp_buff.string_buff
 * 
 * Revision 2.3  91/05/28  09:04:27  brennan
 * removed main_buff
 * 
 * Revision 2.2  91/05/16  12:19:31  brennan
 * cleanup of machine dependencies
 * 
 * Revision 2.1  91/04/08  08:22:44  brennan
 * VERSION 0.97
 * 
*/


/*  cast.c  */

#include "mawk.h"
#include "field.h"
#include "memory.h"
#include "scan.h"
#include "repl.h"

int pow2[NUM_CELL_TYPES] = {1,2,4,8,16,32,64,128,256,512} ;

void cast1_to_d( cp )
  register CELL *cp ;
{
  switch( cp->type )
  { case C_NOINIT :  cp->dval = 0.0 ; break ;

    case C_DOUBLE :  return ;

    case C_MBSTRN :
    case C_STRING :  
          { register STRING *s = (STRING *) cp->ptr  ;

#if FPE_TRAPS_ON  /* look for overflow error */
            errno = 0 ;
            cp->dval = strtod(s->str,(char **)0) ;
            if ( errno && cp->dval != 0.0 ) /* ignore underflow */
                rt_error("overflow converting %s to double", s) ;
#else
            cp->dval = strtod(s->str,(char **)0) ;
#endif
            free_STRING(s) ;
          }
            break ;

    case C_STRNUM :  
      /* don't need to convert, but do need to free the STRING part */
            free_STRING( string(cp) ) ;
            break ;


    default :
            bozo("cast on bad type") ;
  }
  cp->type = C_DOUBLE ;
}

void cast2_to_d( cp )
  register CELL *cp ;
{ register STRING *s ;

  switch( cp->type )
  { case C_NOINIT :  cp->dval = 0.0 ; break ;

    case C_DOUBLE :  goto two ;
    case C_STRNUM :  
            free_STRING( string(cp) ) ;
            break ;

    case C_MBSTRN :
    case C_STRING :  
            s = (STRING *) cp->ptr ;

#if FPE_TRAPS_ON  /* look for overflow error */
            errno = 0 ;
            cp->dval = strtod(s->str,(char **)0) ;
            if ( errno && cp->dval != 0.0 ) /* ignore underflow */
                rt_error("overflow converting %s to double", s) ;
#else
            cp->dval = strtod(s->str,(char **)0) ;
#endif
            free_STRING(s) ;
            break ;

    default :
            bozo("cast on bad type") ;
  }
  cp->type = C_DOUBLE ;

two:   cp++ ;
  switch( cp->type )
  { case C_NOINIT :  cp->dval = 0.0 ; break ;

    case C_DOUBLE :  return ;
    case C_STRNUM :  
            free_STRING( string(cp) ) ;
            break ;

    case C_MBSTRN :
    case C_STRING :  
            s = (STRING *) cp->ptr ;

#if FPE_TRAPS_ON  /* look for overflow error */
            errno = 0 ;
            cp->dval = strtod(s->str,(char **)0) ;
            if ( errno && cp->dval != 0.0 ) /* ignore underflow */
                rt_error("overflow converting %s to double", s) ;
#else
            cp->dval = strtod(s->str,(char **)0) ;
#endif
            free_STRING(s) ;
            break ;

    default :
            bozo("cast on bad type") ;
  }
  cp->type = C_DOUBLE ;
}

void cast1_to_s( cp )
  register CELL *cp ;
{ 
  switch( cp->type )
  { case C_NOINIT :  
        null_str.ref_cnt++ ;
        cp->ptr = (PTR) &null_str ;
        break ;

    case C_DOUBLE  :
        (void) sprintf(temp_buff.string_buff ,
            string(field+OFMT)->str, cp->dval) ;

        cp->ptr = (PTR) new_STRING(temp_buff.string_buff) ;
        break ;

    case C_STRING :  return ;

    case C_MBSTRN :
    case C_STRNUM :  break ;

    default :  bozo("bad type on cast") ;
  }
  cp->type = C_STRING ;
}

void cast2_to_s( cp )
  register CELL *cp ;
{ 

  switch( cp->type )
  { case C_NOINIT : 
        null_str.ref_cnt++ ;
        cp->ptr = (PTR) &null_str ;
        break ;

    case C_DOUBLE  :
        (void) sprintf(temp_buff.string_buff,
            string(field+OFMT)->str, cp->dval ) ;

        cp->ptr = (PTR) new_STRING(temp_buff.string_buff) ;
        break ;

    case C_STRING :  goto two ;

    case C_MBSTRN :
    case C_STRNUM :  break ;

    default :  bozo("bad type on cast") ;
  }
  cp->type = C_STRING ;

two:
  cp++ ;

  switch( cp->type )
  { case C_NOINIT :  
        null_str.ref_cnt++ ; 
        cp->ptr = (PTR) &null_str ;
        break ;

    case C_DOUBLE  :
        (void) sprintf(temp_buff.string_buff,
            string(field+OFMT)->str, cp->dval) ;

        cp->ptr = (PTR) new_STRING(temp_buff.string_buff) ;
        break ;

    case C_STRING :  return ;

    case C_MBSTRN :
    case C_STRNUM :  break ;

    default :  bozo("bad type on cast") ;
  }
  cp->type = C_STRING ;
}

void  cast_to_RE( cp )
  register CELL *cp ;
{ register PTR p ;

  if ( cp->type < C_STRING )  cast1_to_s(cp) ;

  p = re_compile( string(cp) ) ;
  free_STRING( string(cp) ) ;
  cp->type = C_RE ;
  cp->ptr = p ;

}

void  cast_for_split(cp)
  register CELL *cp ;
{
  static char meta[] = "^$.*+?|[]()" ;
  static char xbuff[] = "\\X" ;
  int c ;
  unsigned len ;
    
  if ( cp->type < C_STRING )  cast1_to_s(cp) ;

  if ( (len = string(cp)->len) == 1 )
  {
        if ( (c = string(cp)->str[0]) == ' ' )
        { free_STRING(string(cp)) ;
          cp->type = C_SPACE ; 
          return ; 
        }
        else
        if ( strchr(meta, c) )
        { xbuff[1] = c ;
          free_STRING(string(cp)) ;
          cp->ptr = (PTR) new_STRING(xbuff) ;
        }
  }
  else
  if ( len == 0 ) 
  { free_STRING(string(cp)) ;
    cp->type = C_SNULL ; 
    return ; 
  }

  cast_to_RE(cp) ;
}

/* input: cp-> a CELL of type C_MBSTRN (maybe strnum)
   test it -- casting it to the appropriate type
   which is C_STRING or C_STRNUM
*/

void check_strnum( cp )
  CELL *cp ;
{ char *test ;
  register unsigned char *s , *q ;

  cp->type = C_STRING ; /* assume not C_STRNUM */
  s = (unsigned char *) string(cp)->str ;
  q = s + string(cp)->len ;
  while ( scan_code[*s] == SC_SPACE )  s++ ;
  if ( s == q )  return ;

  while ( scan_code[ q[-1] ] == SC_SPACE )  q-- ;
  if ( scan_code[ q[-1] ] != SC_DIGIT &&
       q[-1] != '.' )   return ;

  switch ( scan_code[*s] )
  {
    case SC_DIGIT :
    case SC_PLUS  :
    case SC_MINUS :
    case SC_DOT   :

#if FPE_TRAPS_ON
             errno = 0 ;
             cp->dval  = strtod((char *)s, &test) ;
             if ( errno && cp->dval != 0.0 )
                rt_error(
                "overflow converting %s to double" , s) ;
#else
             cp->dval = strtod((char *)s, &test) ;
#endif

             if ((char *) q == test )  cp->type = C_STRNUM ;
  }
}

/* cast a CELL to a replacement cell */

void cast_to_REPL( cp )
  register CELL *cp ;
{ register STRING *sval ;

  if ( cp->type < C_STRING )  cast1_to_s(cp) ;
  sval = (STRING *) cp->ptr ;

  (void) cellcpy(cp, repl_compile(sval)) ;
  free_STRING(sval) ;
}


#if   HAVE_STRTOD==0

static char d_str[] =
"^[ \t]*[-+]?([0-9]+\\.?|\\.[0-9])[0-9]*([eE][-+]?[0-9]+)?" ;

static PTR d_ptr ;

void strtod_init()
{ STRING *sval = new_STRING(d_str) ;

  d_ptr = re_compile(sval) ;
  free_STRING(sval) ;
}

double strtod( s, endptr)
  char *s , **endptr ;
{ double atof() ;

  if ( endptr )
  { unsigned len ;

    (void) REmatch(s, d_ptr, &len) ;
    *endptr = s + len ;
  }
  return  atof(s) ;
}
#endif  /* HAVE_STRTOD==0 */

#if   HAVE_FMOD==0

#if SW_FP_CHECK   /* this is V7 and XNX23A specific */

double  fmod(x, y)
  double x, y ;
{ double modf() ;
  double dtmp, ipart ;

  clrerr();
  dtmp = x / y;
  fpcheck();
  return modf(dtmp, &ipart) * y ;
}

#else

double  fmod(x, y)
  double x, y ;
{ double modf() ;
  double ipart ;

  return modf(x/y, &ipart) * y ;
}

#endif
#endif  



