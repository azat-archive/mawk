
/********************************************
fin.c
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the Awk programming language as defined in
Aho, Kernighan and Weinberger, The AWK Programming Language,
Addison-Wesley, 1988.

See the accompaning file, LIMITATIONS, for restrictions
regarding modification and redistribution of this
program in source or binary form.
********************************************/

/*$Log:	fin.c,v $
 * Revision 2.1  91/04/08  08:23:09  brennan
 * VERSION 0.97
 * 
*/

/* fin.c */

#include "mawk.h"
#include "fin.h"
#include "memory.h"
#include "bi_vars.h"
#include "field.h"
#include "symtype.h"
#include "scan.h"
#include <fcntl.h>

extern int errno ;
int PROTO(isatty, (int) ) ;
FILE *PROTO(fdopen, (int, char*) ) ;

/* statics */
int  PROTO( next_main, (void) ) ;
int  PROTO( is_cmdline_assign, (char *) ) ;

FIN  *FINdopen( fd, main_flag )
  int fd , main_flag ;
{ register FIN *fin = (FIN *) zmalloc( sizeof(FIN) ) ;

  fin->fd = fd ;

  if ( main_flag )
  { fin->flags = MAIN_FLAG ;
    fin->buffp = fin->buff = main_buff ;
  }
  else
  { 
    fin->flags = 0 ;
    fin->buffp = fin->buff = (char *) zmalloc(BUFFSZ+1) ;
  }
  *fin->buffp = 0 ;

  if ( isatty(fd) && rs_shadow.type == SEP_CHAR 
       && rs_shadow.c == '\n' )
  {
    /* interactive, i.e., line buffer this file */
    if ( fd == 0 ) fin->fp = stdin ;
    else
    if ( !(fin->fp = fdopen(fd, "r")) )
    { errmsg(errno, "fdopen failed") ; exit(1) ; }
  }
  else  fin->fp = (FILE *) 0 ;

  return fin ;
}

FIN  *FINopen( filename, main_flag )
  char *filename ;
  int main_flag ;
{ int fd ;

  if ( (fd = open( filename , O_RDONLY, 0 )) == -1 )
  { errmsg( errno, "cannot open %s" , filename ) ;
    return (FIN *) 0 ; }

  else  return  FINdopen( fd, main_flag ) ;
}

void  FINclose( fin )
  register FIN *fin ;
{
  if ( ! (fin->flags & MAIN_FLAG) )  
        zfree(fin->buff, BUFFSZ+1) ;

  if ( fin->fd )  
        if ( fin->fp )  (void) fclose(fin->fp) ;
        else  (void) close(fin->fd) ;

  zfree( fin , sizeof(FIN) ) ;
}

/* return one input record as determined by RS,
   from input file (FIN)  fin
*/

char *FINgets( fin, len_p )
  FIN *fin ;
  unsigned *len_p ;
{ register char *p, *q ;
  unsigned match_len ;
  unsigned r ;

restart :

  if ( ! (p = fin->buffp)[0] )  /* need a refill */
  { 
    if ( fin->flags & EOF_FLAG )
        if ( (fin->flags & MAIN_FLAG) && next_main() )  goto restart ;
        else
        { *len_p = 0 ; return (char *) 0 ; }
        
    if ( fin->fp ) /* line buffering */
      if ( ! fgets(fin->buff, BUFFSZ+1, fin->fp) )
      {
        fin->flags |= EOF_FLAG ;
        fin->buff[0] = 0 ;
        fin->buffp = fin->buff ;
        goto restart ;  /* might be main_fin */
      }
      else  /* return this line */
      {
        if ( !(p = strchr(fin->buff, '\n')) )
             p = fin->buff + BUFFSZ + 1 ; /* unlikely to occur */

        *p = 0 ; *len_p = p - fin->buff ;
        fin->buffp = p ;
        return  fin->buff ;
      }
    else  /* block buffering */
    {
      if ( (r = fillbuff(fin->fd, fin->buff, BUFFSZ)) == 0 )
      {
        fin->flags |= EOF_FLAG ;
        fin->buffp = fin->buff ;
        goto restart ; /* might be main */
      }
      else
      if ( r < BUFFSZ )  fin->flags |= EOF_FLAG ;

      p = fin->buffp = fin->buff ;
    }
  }

retry: 

  switch( rs_shadow.type )
  {
    case SEP_CHAR :
            q = strchr(p, rs_shadow.c) ;
            match_len = 1 ;
            break ;

    case SEP_STR  :
            q = str_str(p, ((STRING *) rs_shadow.ptr)->str,
                match_len = ((STRING *) rs_shadow.ptr)->len ) ;
            break ;

    case SEP_RE :
            q = re_pos_match(p, rs_shadow.ptr, &match_len) ;
            /* if the match is at the end, there might be more
               still to be read */
            if ( q && q[match_len] == 0 &&
                 p != fin->buff )  q = (char *) 0 ;
            break ;
            
    default :
            bozo("type of rs_shadow") ;
  }
  if ( q )
  {  /* the easy and normal case */
     *q = 0 ;  *len_p = q - p ;
     fin->buffp = q + match_len  ;
     return p ;
  }

  if ( p == fin->buff )  /* last line or one huge (truncated) line */
  { *len_p = r = strlen(p) ;  fin->buffp = p + r ;
    /* treat truncated case as overflow */
    if ( r == BUFFSZ ) 
    { /* overflow, update NR and FNR */
      cast2_to_d(bi_vars+NR) ;
      bi_vars[NR].dval += 1.0 ;
      bi_vars[FNR].dval += 1.0 ;
      rt_overflow("maximum record length" , BUFFSZ) ;
    }
     return p ;
  }

  /* move a partial line to front of buffer and try again */
  p = (char *) memcpy( fin->buff, p, r = strlen(p) ) ;
  q = p+r ; 
  if ( fin->flags & EOF_FLAG ) *q = 0 ;
  else 
  { unsigned rr = BUFFSZ - r ;

    if ( (r = fillbuff(fin->fd, q, rr)) < rr ) fin->flags |= EOF_FLAG ;
  }
  goto retry ;
}

/*--------
  target is big enough to hold size + 1 chars 
  on exit the back of the target is zero terminated
 *--------------*/
unsigned  fillbuff(fd, target, size)
  int fd ;
  register char *target ;
  unsigned size ;
{ register int r ;
  unsigned entry_size = size ;

  while ( size )
    switch( r = read(fd, target, size) )
    { case -1 :
        errmsg(errno, "read error on file") ;
        exit(1) ;

      case 0 :
        goto out ;

      default :
        target += r ; size -= r ;
        break ;
    }

out :
  *target = 0 ;
  return  entry_size - size ;
}

/* main_fin is a handle to the main input stream
   == -1 if never tried to open 
   == 0  if end of stream
      otherwise active    */
FIN *main_fin = (FIN *) -1 ;
ARRAY   Argv ;   /* to the user this is ARGV  */
static int argi = 1 ;  /* index of next ARGV[argi] to try to open */

int  open_main()  /* boolean return, true if main is open */
{ 
  if ( bi_vars[ARGC].type == C_DOUBLE && bi_vars[ARGC].dval == 1.0 )
  { cell_destroy( bi_vars + FILENAME ) ;
    bi_vars[FILENAME].type = C_STRING ;
    bi_vars[FILENAME].ptr = (PTR) new_STRING( "-") ;
    main_fin = FINdopen(0, 1) ;
    return  1 ;
  }
  else    return next_main() ;
}

static  int  next_main()
{ char xbuff[16] ;
  register CELL *cp ;
  STRING *sval ;
  CELL   argc ;  /* temp copy of ARGC */
  CELL   tc ;    /* copy of ARGV[argi] */
  double d_argi ;

#ifdef  DEBUG
  if ( ! main_fin ) bozo("call to next_main with dead main") ;
#endif

  tc.type = C_NOINIT ;

  if ( main_fin != (FIN *)-1 )  FINclose(main_fin) ;
  cell_destroy( bi_vars + FILENAME ) ;
  cell_destroy( bi_vars + FNR ) ;
  bi_vars[FNR].type = C_DOUBLE ;
  bi_vars[FNR].dval = 0.0 ;

  if ( cellcpy(&argc, &bi_vars[ARGC])->type != C_DOUBLE )
          cast1_to_d(&argc) ;
  xbuff[1] = 0 ;
  d_argi = (double) argi ;

  while ( d_argi < argc.dval )
  {
    if ( argi < 10 )  xbuff[0] = argi + '0' ;
    else  (void) sprintf(xbuff, "%u", argi) ;

    argi++ ; d_argi += 1.0 ;
    sval = new_STRING(xbuff) ;

    /* the user might have changed ARGC or deleted 
       ARGV[argi] -- test for existence without side effects */
    
    if ( ! array_test(Argv, sval) )
    { free_STRING(sval) ; continue ; }

    cp = array_find( Argv, sval, 0) ;
    free_STRING(sval) ;

    /* make a copy so we can cast w/o side effect */
    cell_destroy(&tc) ;
    cp = cellcpy(&tc, cp) ;
    if ( cp->type < C_STRING )  cast1_to_s(cp) ;
    if ( string(cp)->len == 0 )  continue ;

    if ( string(cp)->len == 1 && string(cp)->str[0] == '-' )
    { /* input from stdin */
      main_fin = FINdopen(0,1) ;
    }
    else  /* it might be a command line assignment */
    if ( is_cmdline_assign(string(cp)->str) )  continue ;

    else /* try to open it */
    if ( ! (main_fin = FINopen( string(cp)->str, 1 )) ) continue ;

    /* success */
    (void) cellcpy( &bi_vars[FILENAME] , cp ) ;
    free_STRING( string(cp) ) ;
    return 1 ;
  }
  /* failure */
  bi_vars[FILENAME].type = C_STRING ;
  bi_vars[FILENAME].ptr = (PTR) new_STRING( "" ) ;
  main_fin = (FIN *) 0 ;
  cell_destroy(&tc) ;
  return 0 ;
}
    

static int is_cmdline_assign(s)
  char *s ;
{ char *q;
  unsigned char *p ;
  int c ;
  SYMTAB *stp ;
  CELL *cp ;

  if ( scan_code[*(unsigned char *)s] != SC_IDCHAR 
       || !(q = strchr(s,'=')) ) return 0 ;

  p = (unsigned char *)s+1 ;
  while ( (c = scan_code[*p]) == SC_IDCHAR || c == SC_DIGIT ) p++ ;

  if ( (char *)p < q )  return 0 ;

  *q = 0 ;
  stp = find(s) ;

  switch( stp->type )
  {
    case  ST_NONE :
        stp->type = ST_VAR ;
        stp->stval.cp = cp = new_CELL() ;
        break ;

    case  ST_VAR :
        cp = stp->stval.cp ;
        break ;

    default :
        rt_error(
          "cannot command line assign to %s\n\t- type clash or keyword" 
          , s ) ;
  }
  
  *q++ = '=' ;
  cp->ptr = (PTR) new_STRING(q) ;
  check_strnum(cp) ;
  return 1 ;
}
