
/********************************************
field.c
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the Awk programming language as defined in
Aho, Kernighan and Weinberger, The AWK Programming Language,
Addison-Wesley, 1988.

See the accompaning file, LIMITATIONS, for restrictions
regarding modification and redistribution of this
program in source or binary form.
********************************************/

/* $Log:	field.c,v $
 * Revision 2.2  91/04/09  12:39:00  brennan
 * added static to funct decls to satisfy STARDENT compiler
 * 
 * Revision 2.1  91/04/08  08:23:01  brennan
 * VERSION 0.97
 * 
*/


/* field.c */

#include "mawk.h"
#include "field.h"
#include "init.h"
#include "memory.h"
#include "scan.h"
#include "bi_vars.h"
#include "repl.h"
#include "regexp.h"

CELL  field[NUM_FIELDS] ;

/* statics */
static void PROTO( build_field0, (void) ) ;
static void PROTO( xcast, (CELL *) ) ;
static void PROTO( set_rs_shadow, (void) ) ;

/* the order is important here, must be the same as
   postion after field[MAX_FIELD] */

static char *biv_field_names[] = {
"NF" , "RS" , "FS", "OFMT" } ;

/* a description of how to split based on RS.
   If RS is changed, so is rs_shadow */
SEPARATOR rs_shadow = {SEP_CHAR, '\n'} ;
/* a splitting CELL version of FS */
CELL fs_shadow = {C_SPACE} ;
int   nf ;  
  /* nf holds the true value of NF.  If nf < 0 , then
     NF has not been computed, i.e., $0 has not been split
  */

static void set_rs_shadow()
{ CELL c ;
  STRING  *sval ;
  char *s ;
  unsigned len ;

  if ( rs_shadow.type == SEP_STR )  free_STRING((STRING*) rs_shadow.ptr) ;

  cast_for_split( cellcpy(&c, field+RS) ) ;
  switch( c.type )
  {
    case C_RE :
        if ( s = is_string_split(c.ptr, &len) )
            if ( len == 1 )
            { rs_shadow.type = SEP_CHAR ;
              rs_shadow.c = s[0] ;
            }
            else
            { rs_shadow.type = SEP_STR ;
              rs_shadow.ptr = (PTR) new_STRING(s) ;
            }
        else
        { rs_shadow.type = SEP_RE ;
          rs_shadow.ptr = c.ptr ;
        }
        break ;

    case C_SPACE :
        rs_shadow.type = SEP_CHAR ;
        rs_shadow.c = ' ' ;
        break ;

    case C_SNULL : /* RS becomes one or more blank lines */
        rs_shadow.type = SEP_RE ;
        sval = new_STRING( "\n([ \t]*\n)+" ) ;
        rs_shadow.ptr = re_compile(sval) ;
        free_STRING(sval) ;
        break ;

    default : bozo("bad cell in set_rs_shadow") ;
  }
}

void  field_init()
{ char **p = biv_field_names ;
  SYMTAB *st_p ;
  int i ;

  for(i = NF  ; i < NUM_FIELDS ; p++ , i++ )
  {
    st_p = insert( *p ) ;
    st_p->type = ST_FIELD ;
    st_p->stval.cp = &field[i] ;
  }

  field[NF].type = C_DOUBLE ;
  field[NF].dval = 0.0 ;
  field[0].type = C_STRING ;
  field[0].ptr = (PTR) & null_str ;
  null_str.ref_cnt++ ;

  field[RS].type = C_STRING ;
  field[RS].ptr =  (PTR) new_STRING( "\n" ) ;
  /* rs_shadow already set */

  field[FS].type = C_STRING ;
  field[FS].ptr = (PTR) new_STRING( " " ) ;
  /* fs_shadow is already set */

  field[OFMT].type = C_STRING ;
  field[OFMT].ptr = (PTR) new_STRING( "%.6g" ) ;
}



void  set_field0( s, len)
  char *s ;
  unsigned len ;
{ 
  cell_destroy( & field[0] ) ;
  nf = -1 ;

  if ( len )
  {
    field[0].type = C_MBSTRN ;
    field[0].ptr = (PTR) new_STRING( (char *) 0, len) ;
    (void) memcpy( string(&field[0])->str, s, len ) ;
  }
  else
  {
    field[0].type = C_STRING ;
    field[0].ptr = (PTR) &null_str ;
    null_str.ref_cnt++ ;
  }
}



/* split field[0] into $1, $2 ... and set NF  */

void  split_field0()
{ register int i ;
  CELL  c ;
  int  cast_flag ; /* we had to cast field[0] */
  char *s ;
  unsigned len ;

  if ( fs_shadow.type == C_SNULL ) /* FS == "" */
  { cell_destroy(field+1) ;
    (void) cellcpy(field+1, field+0) ;
    cell_destroy(field+NF) ;
    field[NF].type = C_DOUBLE ; field[NF].dval = 1.0 ;
    return ;
  }

  if ( field[0].type < C_STRING )
  { cast1_to_s(cellcpy(&c, field+0)) ;
    s = string(&c)->str ;
    len = string(&c)->len ;
    cast_flag = 1 ;
  }
  else  
  { s = string(field)->str ; 
    len = string(field)->len ;
    cast_flag = 0 ; 
  }

  nf = len == 0 ? 0 :
       fs_shadow.type == C_SPACE
       ? space_split(s) : re_split(s, fs_shadow.ptr) ;

  cell_destroy(field+NF) ;
  field[NF].type = C_DOUBLE ;
  field[NF].dval = (double) nf ;

  for( i = 1 ; i <= nf ; i++ )
  {
    cell_destroy(field+i) ;
    field[i].ptr = temp_buff.ptr_buff[i-1] ;
    field[i].type = C_MBSTRN ;
  }

  if ( cast_flag )  free_STRING( string(&c) ) ;
}

/*
  assign CELL *cp to field[i]
  and take care of all side effects
*/

void  field_assign( i, cp)
  register int i ;
  CELL *cp ;
{ register int j ;
  CELL c ;

  /* update fields not up to date */
  if ( nf < 0 )  split_field0() ;

  switch( i )
  {
    case 0 :
        cell_destroy(field) ;
        nf = -1 ;
        (void) cellcpy(field, cp) ;
        break ; ;

    case  NF :
        cell_destroy(field+NF) ;
        (void) cellcpy(field+NF, cellcpy(&c,cp) ) ;
        if ( c.type != C_DOUBLE )  cast1_to_d(&c) ;

        if ( (j = (int) c.dval) < 0 )
            rt_error("negative value assigned to NF") ;
        if ( j > MAX_FIELD )
            rt_overflow("MAX_FIELD", MAX_FIELD) ;

        if ( j > nf )
            for ( i = nf+1 ; i <= j ; i++ )
            { cell_destroy(field+i) ;
              field[i].type = C_STRING ;
              field[i].ptr = (PTR) &null_str ;
              null_str.ref_cnt++ ;
            }

        nf = j ;
        build_field0() ;
        break ;

    case  RS :
        cell_destroy(field+RS) ;
        (void) cellcpy(field+RS, cp) ;
        set_rs_shadow() ;
        break ;

    case  FS :
        cell_destroy(field+FS) ;
        cast_for_split( cellcpy(&fs_shadow, cellcpy(field+FS, cp)) ) ;
        break ;

    case OFMT : 
        /* If the user does something stupid with OFMT, we could crash.
           We'll make an attempt to protect ourselves here.  This is
           why OFMT is made a field.

           The ptr of OFMT always has a valid STRING, even if assigned
           a DOUBLE or NOINIT
        */

        free_STRING( string(field+OFMT) ) ;
        (void) cellcpy(field+OFMT, cp) ;
        if ( field[OFMT].type < C_STRING ) /* !! */
             field[OFMT].ptr = (PTR) new_STRING( "%.6g" ) ;
        else
        {
          /* It's a string, but if it's really goofy it could still
             damage us. Test it . */
          temp_buff.string_buff[256] = 0 ;
          (void) sprintf(temp_buff.string_buff, 
             string(field+OFMT)->str, 3.1459) ;
          if ( temp_buff.string_buff[256] )
                rt_error("OFMT assigned unusable value") ;
        }
        break ;
           

    default:

#ifdef DEBUG 
        if ( i < 0 )
            bozo("negative field index in field_assign") ;
        if ( i > MAX_FIELD )
            bozo("large field index in field_assign") ;
#endif

        cell_destroy(field+i) ;
        (void) cellcpy(field+i, cp) ;
        if ( i > nf )
        { for ( j = nf+1 ; j < i ; j++ )
          { cell_destroy(field+j) ;
            field[j].type = C_STRING ;
            field[j].ptr = (PTR) &null_str ;
            null_str.ref_cnt++ ;
          }
          nf = i ;
          cell_destroy(field+NF) ;
          field[NF].type = C_DOUBLE ;
          field[NF].dval = (double) i ;
        }

        build_field0() ;

  }
}


/* get the string rep of a double without changing its
   type */

static void  xcast( cp )
  register CELL *cp ;
{
  switch ( cp->type )
  {
    case C_NOINIT :
        cp->ptr = (PTR) &null_str ;
        null_str.ref_cnt++ ;
        break ;

    case C_DOUBLE :
        (void) sprintf(temp_buff.string_buff, 
          string(field+OFMT)->str, cp->dval) ;
        cp->ptr = (PTR) new_STRING(temp_buff.string_buff) ;
        break ;
  }
}


/* construct field[0] from the other fields */

static void  build_field0()
{ 


#ifdef DEBUG
  if ( nf < 0 )  
      bozo("nf <0 in build_field0") ;
#endif

  cell_destroy( field+0 ) ;

  if ( nf == 0 )
  { field[0].type = C_STRING ;
    field[0].ptr = (PTR) &null_str ;
    null_str.ref_cnt++ ;
  }
  else
  if ( nf == 1 )  (void) cellcpy(field, field+1) ;

  else
  { CELL  ofs ;
    char *ofs_str ;
    unsigned ofs_len , len0 = 0 ;
    register int i ;
    register char *p, *q ;


    (void) cellcpy(& ofs , bi_vars + OFS) ;
    if ( ofs.type < C_STRING )  cast1_to_s(&ofs) ;
    ofs_str = string(&ofs)->str ;
    ofs_len = string(&ofs)->len ;


    for( i = 1 ; i <= nf ; i++ )
    {
      if ( field[i].type < C_STRING )  xcast(field+i) ;
      len0 += string(field+i)->len + ofs_len ;
    }
    len0 -= ofs_len ;

    field[0].type = C_STRING ;
    field[0].ptr = (PTR) new_STRING((char *) 0, len0) ;

    p = string(field)->str ;
    for( i = 1 ; i < nf ; i++ )
    {
      (void) memcpy(p, string(field+i)->str, string(field+i)->len) ;
      p += string(field+i)->len ;

      /* add the separator */
      q = ofs_str ;
      while ( *p++ = *q++ ) ;  p-- ;

      /* if not really string undo the xcast */
      if ( field[i].type < C_STRING )
              free_STRING( string(field+i) ) ;
    }
    /* do the last piece */
    (void) memcpy(p, string(field+i)->str, string(field+i)->len) ;
    if ( field[i].type < C_STRING )
              free_STRING( string(field+i) ) ;
    
    free_STRING( string(&ofs) ) ;
  } 
}
