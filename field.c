
/********************************************
field.c
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the AWK programming language.

Mawk is distributed without warranty under the terms of
the GNU General Public License, version 2, 1991.
********************************************/

/* $Log:	field.c,v $
 * Revision 3.5.1.1  91/09/14  17:23:11  brennan
 * VERSION 1.0
 * 
 * Revision 3.5  91/08/13  06:51:16  brennan
 * VERSION .9994
 * 
 * Revision 3.4  91/07/17  15:11:39  brennan
 * pass length of s to space_split
 * 
 * Revision 3.3  91/06/28  04:16:35  brennan
 * VERSION 0.999
 * 
 * Revision 3.2  91/06/24  10:05:27  brennan
 * FS="" and $0="", gave NF=1 should be 0
 * 
 * Revision 3.1  91/06/07  10:27:22  brennan
 * VERSION 0.995
 * 
 * Revision 2.6  91/05/28  15:17:46  brennan
 * removed STRING_BUFF back to temp_buff.string_buff
 * 
 * Revision 2.5  91/05/28  09:04:42  brennan
 * removed main_buff
 * 
 * Revision 2.4  91/05/08  12:23:17  brennan
 * RS = "" is now the same as RS = "\n\n+"
 * 
 * Revision 2.3  91/04/24  13:49:56  brennan
 * eliminated unnecessary calculation of fields in field_assign()
 * 
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

union tbuff  temp_buff ;
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
        sval = new_STRING( "\n\n+" ) ;
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
    (void) memcpy( string(&field[0])->str, s, SIZE_T(len) ) ;
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
{ register CELL *cp ;
  register int i ;
  CELL  c ;  /* copy field[0] here if not string */


  if ( field[0].type < C_STRING )
  { cast1_to_s(cellcpy(&c, field+0)) ;
    cp = &c ;
  }
  else   cp = &field[0] ;

  if ( string(cp)->len == 0 )  nf = 0 ;
  else
  {
    switch( fs_shadow.type )
    {
      case   C_SNULL :  /* FS == "" */
	  nf = 1 ;
	  cell_destroy(field+NF) ;
	  field[NF].type = C_DOUBLE ;
	  field[NF].dval = 1.0 ;
	  field[1].type = C_MBSTRN ;
	  field[1].ptr = cp->ptr ;

	  if ( cp == field )  string(cp)->ref_cnt++ ;
	  /* else we gain one ref_cnt and lose one for a wash */

	  return ;

      case  C_SPACE :
	  nf = space_split(string(cp)->str, string(cp)->len) ;
	  break ;

      default :
	  nf = re_split(string(cp)->str, fs_shadow.ptr) ;
	  break ;
    }
        
    if ( nf > MAX_FIELD )
        rt_overflow("maximum number of fields", MAX_FIELD) ;
  }
  
  cell_destroy(field+NF) ;
  field[NF].type = C_DOUBLE ;
  field[NF].dval = (double) nf ;

  for( i = 1 ; i <= nf ; i++ )
  {
    cell_destroy(field+i) ;
    field[i].ptr = temp_buff.ptr_buff[i-1] ;
    field[i].type = C_MBSTRN ;
  }

  if ( cp == &c )  free_STRING( string(cp) ) ;
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

  /* update fields not up to date , unless we are
     assigning to $0 */
  if ( i != 0 && nf < 0 )  split_field0() ;

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
      (void) memcpy(p, string(field+i)->str, SIZE_T(string(field+i)->len)) ;
      p += string(field+i)->len ;

      /* add the separator */
      q = ofs_str ;
      while ( *p++ = *q++ ) ;  p-- ;

      /* if not really string undo the xcast */
      if ( field[i].type < C_STRING )
              free_STRING( string(field+i) ) ;
    }
    /* do the last piece */
    (void) memcpy(p, string(field+i)->str, SIZE_T(string(field+i)->len)) ;
    if ( field[i].type < C_STRING )
              free_STRING( string(field+i) ) ;
    
    free_STRING( string(&ofs) ) ;
  } 
}
