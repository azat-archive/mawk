
/********************************************
scan.c
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the Awk programming language as defined in
Aho, Kernighan and Weinberger, The AWK Programming Language,
Addison-Wesley, 1988.

See the accompaning file, LIMITATIONS, for restrictions
regarding modification and redistribution of this
program in source or binary form.
********************************************/


/* $Log:	scan.c,v $
 * Revision 2.2  91/04/09  12:39:27  brennan
 * added static to funct decls to satisfy STARDENT compiler
 * 
 * Revision 2.1  91/04/08  08:23:51  brennan
 * VERSION 0.97
 * 
*/


#include  "mawk.h"
#include  "sizes.h"
#include  "scan.h"
#include  "memory.h"
#include  "field.h"
#include  "init.h"
#include  "fin.h"
#include  "repl.h"
#include  <fcntl.h>
#include  <string.h>
#include  "files.h"


/* static functions */
static void PROTO(buff_create, (char *) ) ;
static int PROTO(slow_next, (void) ) ;
static void PROTO(eat_comment, (void) ) ;
static double PROTO(collect_decimal, (int, int *) ) ;
static int PROTO(collect_string, (void) ) ;
static int  PROTO(collect_RE, (void) ) ;
static char *PROTO(rm_escape, (char *) ) ;


/*-----------------------------
  program file management
 *----------------------------*/

static  unsigned char *buffer ;
static  unsigned char *buffp ;  
    /* unsigned so it works with 8 bit chars */
static  int  program_fd = -1  ; 
static  int  eof_flag ;


static void buff_create(s)
  char *s ;
{
  /* If program_fd == -1, program came from command line and s
     is it, else s is a filename */

  if ( program_fd == -1 )
  { buffer = buffp = (unsigned char *) s ; eof_flag = 1 ; }
  else /* s is a filename, open it */
  {
    if ( s[0] == '-' && s[1] == 0 ) program_fd = 0 ;
    else
    if ( (program_fd = open(s, O_RDONLY, 0)) == -1 )
    { errmsg( errno, "cannot open %s", s) ; mawk_exit(1) ; }

    buffp = buffer = (unsigned char *) zmalloc( BUFFSZ+1 ) ;

    eof_flag = fillbuff(program_fd, buffer, BUFFSZ) < BUFFSZ ;
  }
}

void scan_cleanup()
{ 
  if ( program_fd >= 0 ) zfree(buffer, BUFFSZ+1) ;
  if ( program_fd > 0 )  (void) close(program_fd) ;
  scan_code['\n'] = SC_SPACE ;
}


void  scan_init(flag, p)
  int flag ; /* on if program is from the command line */
  char *p ;
{ 
  if ( ! flag ) program_fd = 0 ;
  buff_create(p) ;

  eat_nl() ; /* scan to first token */
  if ( next() == 0 )  
  { errmsg(0, "no program") ; mawk_exit(1) ; }
  un_next() ;
}

/*--------------------------------
  global variables shared by yyparse() and yylex()
 *-------------------------------*/

int  current_token = -1 ; 
unsigned  token_lineno ;
unsigned  compile_error_count ;
int   paren_cnt ;
int   brace_cnt ;
int   print_flag ;  /* changes meaning of '>' */
int   getline_flag ; /* changes meaning of '<' */

extern  YYSTYPE  yylval ;

/*----------------------------------------
 file reading functions
 next() and un_next(c) are macros in scan.h

 *---------------------*/

static  unsigned lineno = 1 ;

/* used to help distinguish / as divide or start of RE  */

static int can_precede_re[] =
{ MATCH, NOT_MATCH, COMMA, RBRACE, 
LPAREN, NOT, P_OR, P_AND, NL,  -1 } ;

/* read one character -- slowly */
static int slow_next()
{ 
  if ( *buffp == 0  )
      if ( !eof_flag ) 
      { buffp = buffer ;
        eof_flag = fillbuff(program_fd, buffer,BUFFSZ) < BUFFSZ ;
      }

  return *buffp++ ; /* note can un_next() , eof which is zero */
}

static void eat_comment()
{ register int c ;

  while ( (c = next()) != '\n' && scan_code[c] ) ;
  un_next() ;
}

void eat_nl() /* eat all space including newlines */
{
  while ( 1 )
    switch( scan_code[next()] )
    { 
      case SC_COMMENT : 
         eat_comment() ;
         break ;
         
      case  SC_NL  :   lineno++ ;
      /* fall thru  */
      case  SC_SPACE  :   break ;
      default :  
          un_next() ; return ;
    }
}

int yylex()
{ 
  register int c ;

  token_lineno = lineno ;

reswitch:

    switch( scan_code[c = next()] )
    {
      case  0  :  /* if no terminator on the line put one */
          if ( (c = current_token) == RBRACE ||
                c == NL || c == SEMI_COLON ) ct_ret(EOF) ;
          else
          { un_next() ;  ct_ret(NL) ; }
          
      case  SC_SPACE  :   goto reswitch ;

      case  SC_COMMENT :
          eat_comment() ; goto reswitch ;

      case  SC_NL  : 
          lineno++ ; eat_nl() ;
          ct_ret(NL) ;

      case SC_ESCAPE :
          while ( scan_code[ c = next() ] == SC_SPACE ) ;
          if ( c == '\n')
          { token_lineno = ++lineno ; goto reswitch ; }
          if ( c == 0 )  ct_ret(EOF) ;
          un_next() ;
          yylval.ival = '\\' ;
          ct_ret(UNEXPECTED) ;


      case  SC_SEMI_COLON  : 
          eat_nl() ;
          ct_ret(SEMI_COLON) ;

      case  SC_LBRACE :  
          eat_nl() ; brace_cnt++ ;
          ct_ret(LBRACE) ;

      case  SC_PLUS  :
          test2_ret('+', INC, '=', ADD_ASG, PLUS ) ;

      case  SC_MINUS :
          test2_ret('-', DEC, '=', SUB_ASG, MINUS ) ;

      case  SC_COMMA :  eat_nl() ; ct_ret(COMMA) ;

      case  SC_MUL  :  test1_ret('=', MUL_ASG, MUL) ;
      case  SC_DIV :   
          { int *p = can_precede_re ;

            do
                if ( *p == current_token )
                    ct_ret( collect_RE() ) ;
            while ( *p++ != -1 ) ;

            test1_ret( '=', DIV_ASG , DIV ) ;
          }

      case  SC_MOD  :  test1_ret('=', MOD_ASG, MOD) ;
      case  SC_POW :   test1_ret('=' , POW_ASG, POW) ;
      case  SC_LPAREN : 
          paren_cnt++ ;
          ct_ret(LPAREN) ;

      case  SC_RPAREN : 
          if ( --paren_cnt < 0 )
          { compile_error( "extra ')'" ) ;
            paren_cnt = 0 ;
            goto reswitch ; }

          ct_ret(RPAREN) ;

      case  SC_LBOX   : ct_ret(LBOX) ;
      case  SC_RBOX   : ct_ret(RBOX) ;

      case  SC_MATCH  : ct_ret(MATCH) ;

      case  SC_EQUAL  :
          test1_ret( '=', EQ, ASSIGN ) ;

      case  SC_NOT : /* !  */
          test2_ret('=', NEQ, '~', NOT_MATCH, NOT ) ;

      case  SC_LT  :  /* '<' */
          if ( getline_flag )
          { getline_flag = 0 ; ct_ret(IO_IN) ; }
          else
          { ct_ret( ifnext('=', LTE , LT) ) ; }

      case  SC_GT  :  /* '>' */
          if ( print_flag && paren_cnt == 0 )
          { print_flag = 0 ;
            /* there are 3 types of IO_OUT 
               -- build the error string in temp_buff */
            temp_buff.string_buff[0] = '>' ;
            if ( next() == '>' ) 
            { 
              yylval.ival = F_APPEND ;
              temp_buff.string_buff[1] = '>' ;
              temp_buff.string_buff[2] =  0 ;
            }
            else
            { un_next() ; 
              yylval.ival = F_TRUNC ; 
              temp_buff.string_buff[1] = 0 ;
            }
            return current_token = IO_OUT ;
          }

          ct_ret( ifnext('=', GTE , GT) ) ;

      case  SC_OR :
          if ( next() == '|' ) 
          { eat_nl() ; ct_ret(brace_cnt?OR:P_OR) ; }
          else 
          { un_next() ; 

            if ( print_flag && paren_cnt == 0 )
            { print_flag = 0 ; 
              yylval.ival = PIPE_OUT;
              temp_buff.string_buff[0] = '|' ;
              temp_buff.string_buff[1] = 0 ;
              ct_ret(IO_OUT) ;
            }
            else  ct_ret(PIPE) ;
          }

      case  SC_AND :
          if ( next() == '&' )  
          { eat_nl() ; ct_ret(brace_cnt?AND:P_AND) ; }
          else 
          { un_next() ; yylval.ival = '&' ; ct_ret(UNEXPECTED) ; }

      case  SC_QMARK  :  ct_ret(QMARK) ;
      case  SC_COLON  :  ct_ret(COLON) ;
      case  SC_RBRACE :
          if ( --brace_cnt < 0 )
          { compile_error("extra '}'" ) ;
            brace_cnt = 0 ; goto reswitch ; }

          if ( (c = current_token) == NL || c == SEMI_COLON 
               || c == SC_FAKE_SEMI_COLON  || c == RBRACE  )
          { eat_nl() ; ct_ret(RBRACE) ; }

          brace_cnt++ ; un_next() ;
          current_token = SC_FAKE_SEMI_COLON ;
          return  SEMI_COLON ;

      case  SC_DIGIT  :
      case  SC_DOT    :
          { double d ;
            int flag ;

            if ( (d = collect_decimal(c, &flag)) == 0.0 )
                if ( flag )  ct_ret(flag) ;
                else  yylval.cp = &cell_zero ;
            else if ( d == 1.0 ) yylval.cp = &cell_one ;
            else 
            { yylval.cp = new_CELL() ;
              yylval.cp->type = C_DOUBLE ;
              yylval.cp->dval = d ; 
            }
            ct_ret( CONSTANT ) ;
          }

      case  SC_DOLLAR :  /* '$' */
          { double d ;
            int flag ;

            while ( scan_code[c = next()] == SC_SPACE )  ;
            if ( scan_code[c] != SC_DIGIT &&
                 scan_code[c] != SC_DOT )
            { un_next() ; ct_ret(DOLLAR) ; }
            /* compute field address at compile time */
            if ( (d = collect_decimal(c, &flag)) == 0.0 )
                if ( flag )  ct_ret(flag) ; /* an error */
                else  yylval.cp = &field[0] ;
            else
            { int k = (int) d ;

              if ( k > MAX_FIELD )
              { compile_error(
                   "maximum field index(%d) exceeded" , k ) ;
                k = MAX_FIELD ;
              }
              else  yylval.cp = &field[k] ;
            }

            ct_ret(FIELD) ;
          }

      case  SC_DQUOTE :
          return current_token = collect_string() ;

      case  SC_IDCHAR : /* collect an identifier */
            { unsigned char *p =
                    (unsigned char *)temp_buff.string_buff + 1 ;
              SYMTAB *stp ;

              temp_buff.string_buff[0] = c ;

              while ( 
                (c = scan_code[ *p++ = next()]) == SC_IDCHAR ||
                       c == SC_DIGIT )  ;
              
              un_next() ; * --p = 0 ;

              switch( (stp = find(temp_buff.string_buff))->type )
              { case ST_NONE :  
                  /* check for function call before defined */
                      if ( next() == '(' )
                      { stp->type = ST_FUNCT ;
                        stp->stval.fbp = (FBLOCK *)
                                zmalloc(sizeof(FBLOCK)) ;
                        stp->stval.fbp->name = stp->name ;
                        stp->stval.fbp->code = (INST *) 0 ;
                        yylval.fbp = stp->stval.fbp ;
                        current_token = FUNCT_ID ;
                      }
                      else
                      { yylval.stp = stp ;
                        current_token = ID ;
                      }
                      un_next() ;
                      break ;
                        
                case ST_VAR :
                case  ST_ARRAY :
                case  ST_LOCAL_NONE :
                case  ST_LOCAL_VAR :
                case  ST_LOCAL_ARRAY :

                      yylval.stp = stp ;
                      current_token = ID ;
                      break ;

                case ST_FUNCT :
                      yylval.fbp = stp->stval.fbp ;
                      current_token = FUNCT_ID ;
                      break ;

                case ST_KEYWORD :  
                      current_token = stp->stval.kw ;
                      break ;

                case  ST_BUILTIN :
                      yylval.bip = stp->stval.bip ;
                      current_token = BUILTIN ;
                      break ;

                case  ST_FIELD  :
                      yylval.cp = stp->stval.cp ;
                      current_token = FIELD ;
                      break ;

                case  ST_LENGTH  :
                    { CELL *bi_length() ;
                      static BI_REC length_bi_rec =
                      { "length", bi_length, 1, 1 } ;

                      while ( scan_code[ c = next() ] == SC_SPACE ) ;
                      un_next() ;

                      if ( c == '(' )
                      { yylval.bip = &length_bi_rec ;
                        current_token = BUILTIN ;
                      }
                      else current_token = LENGTH ;
                    }
                    break ;

                default : 
                      bozo("find returned bad st type") ;
              }
              return  current_token  ;
            }


      case  SC_UNEXPECTED :
            yylval.ival = c & 0xff ;
            ct_ret(UNEXPECTED) ;
    }
    return  0 ; /* never get here make lint happy */
}

/* collect a decimal constant in temp_buff.
   Return the value and error conditions by reference */

static double collect_decimal(c, flag)
  int c ; int *flag ;
{ register unsigned char *p = (unsigned char*) temp_buff.string_buff + 1;
  unsigned char *endp ;
  double d ;

  *flag = 0 ;
  temp_buff.string_buff[0] = c ;

  if ( c == '.' )
  { if ( scan_code[*p++ = next()] != SC_DIGIT )
    { *flag = UNEXPECTED ; yylval.ival = '.' ;
      return 0.0 ; }
  }
  else
  {  while ( scan_code[*p++ = next()] == SC_DIGIT ) ;
     if ( p[-1] != '.' )
     { un_next() ; p-- ; }
  }
  /* get rest of digits after decimal point */
  while ( scan_code[*p++ = next()] == SC_DIGIT )  ;

  /* check for exponent */
  if ( p[-1] != 'e' && p[-1] != 'E' )
  { un_next() ; * --p = 0 ; }
  else  /* get the exponent */
    if ( scan_code[*p = next()] != SC_DIGIT &&
         *p != '-' && *p != '+' )
    { *++p = 0 ; *flag = BAD_DECIMAL ;
      return 0.0 ; }
    else  /* get the rest of the exponent */
    { p++ ;
      while ( scan_code[*p++ = next()] == SC_DIGIT )  ;
      un_next() ; * --p = 0 ;
    }

  errno = 0 ; /* check for overflow/underflow */
  d = strtod( temp_buff.string_buff, &endp ) ;
  if ( errno )
      compile_error( "%s : decimal %sflow" , temp_buff.string_buff,
        d == 0.0 ? "under" : "over") ;
  if ( endp != p )
  { *flag = BAD_DECIMAL ; return 0.0 ; }
  return d ;
}

/*----------  process escape characters ---------------*/

static char hex_val['f' - 'A' + 1] = {
10,11,12,13,14,15, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 0,
10,11,12,13,14,15 } ;

#define isoctal(x)  ((x)>='0'&&(x)<='7')

#define  hex_value(x)   hex_val[(x)-'A']

#define ishex(x) (scan_code[x] == SC_DIGIT ||\
                  'A' <= (x) && (x) <= 'f' && hex_value(x))

static int PROTO(octal, (char **)) ;
static int PROTO(hex, (char **)) ;

/* process one , two or three octal digits
   moving a pointer forward by reference */
static int octal( start_p )
  char **start_p ;
{ register char *p = *start_p ;
  register unsigned x ;

  x = *p++ - '0' ;
  if ( isoctal(*p) )
  {
    x = (x<<3) + *p++ - '0' ;
    if ( isoctal(*p) )   x = (x<<3) + *p++ - '0' ;
  }
  *start_p = p ;
  return  x & 0xff ;
}

/* process one or two hex digits
   moving a pointer forward by reference */

static int  hex( start_p )
  unsigned char **start_p ;
{ register unsigned char *p = *start_p ;
  register unsigned x ;
  unsigned t ;

  if ( scan_code[*p] == SC_DIGIT )
        x = *p++ - '0' ;
  else  x = hex_value(*p++) ;

  if ( scan_code[*p] == SC_DIGIT )
        x = (x<<4) + *p++ - '0' ;
  else
  if ( 'A' <= *p && *p <= 'f' && (t = hex_value(*p)) )
  { x = (x<<4) + t ; p++ ; }

  *start_p = p ;
  return x ;
}

static char escape_test[] = 
  "n\nt\tb\br\rf\fa\07v\013\\\\\"\"\'\'" ;

/* process the escape characters in a string, in place . */

static char *rm_escape(s)
  char *s ;
{ register char *p, *q ;
  char *t ;

  q = p = s ;

  while ( *p )
      if ( *p == '\\' )
      { 
        if ( t = strchr(escape_test, * ++p) )
        { 
          p++ ; *q++ = t[1] ; 
        }
        else
        if ( isoctal(*p) ) 
        {
          t = p ;  *q++ = octal(&t) ; p = t ;
        }
        else
        if ( *p == 'x' && ishex(*(unsigned char*)(p+1)) )
        {
          t = p+1 ; *q++ = hex(&t) ; p = t ;
        }
        else  /* not an escape sequence */
        { 
          *q++ = '\\' ; *q++ = *p++ ;
        }
      }
      else  *q++ = *p++ ;

  *q = 0 ;
  return s ;
}

static  int  collect_string()
{ register unsigned char *p = (unsigned char *)temp_buff.string_buff ;
  int c ;
  int e_flag = 0 ; /* on if have an escape char */

  while ( 1 )
      switch( scan_code[ *p++ = next() ] )
      { case  SC_DQUOTE : /* done */
              * --p = 0 ;  goto out ;

        case  SC_NL :
              p[-1] = 0 ;
              /* fall thru */

        case  0 :   /* unterminated string */
              compile_error(
              "runaway string constant \"%.10s ..." ,
              temp_buff.string_buff, token_lineno ) ;
              mawk_exit(1) ;

        case SC_ESCAPE :
              if ( (c = next()) == '\n' )
              { p-- ; lineno++ ; }
              else  
                if ( c == 0 )  un_next() ;   
                else 
                { *p++ = c ; e_flag = 1 ; }

              break ;

        default : break ;
      }

out:
    yylval.cp = new_CELL() ;
    yylval.cp->type = C_STRING ;
    yylval.cp->ptr = (PTR) new_STRING(
         e_flag ? rm_escape( temp_buff.string_buff ) 
                : temp_buff.string_buff ) ;
    return  CONSTANT ;
}


static  int  collect_RE()
{ register unsigned char *p = (unsigned char*) temp_buff.string_buff ;
  int c ;
  STRING *sval ;

  while ( 1 )
      switch( scan_code[ *p++ = next() ] )
      { case  SC_DIV : /* done */
              * --p = 0 ;  goto out ;

        case  SC_NL :
              p[-1] = 0 ;
              /* fall thru */

        case  0 :   /* unterminated re */
              compile_error(
              "runaway regular expression /%.10s ..." ,
              temp_buff.string_buff, token_lineno ) ;
              mawk_exit(1) ;

        case SC_ESCAPE :
              switch( c = next() )
              { case '/' :  
                      p[-1] = '/' ; break ;

                case '\n' :
                      p-- ;  break ;

                case  0   :
                      un_next() ;  break ;

                default :
                      *p++ = c ; break ;
              }
              break ;
      }

out:
  /* now we've got the RE, so compile it */
  sval = new_STRING( temp_buff.string_buff ) ;
  yylval.cp = new_CELL() ;
  yylval.cp->type = C_RE ;
  yylval.cp->ptr = re_compile(sval) ;
  free_STRING(sval) ;
  return RE ;
}

