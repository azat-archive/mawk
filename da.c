
/********************************************
da.c
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the Awk programming language as defined in
Aho, Kernighan and Weinberger, The AWK Programming Language,
Addison-Wesley, 1988.

See the accompaning file, LIMITATIONS, for restrictions
regarding modification and redistribution of this
program in source or binary form.
********************************************/


/* $Log:	da.c,v $
 * Revision 2.1  91/04/08  08:22:50  brennan
 * VERSION 0.97
 * 
*/


/*  da.c  */
/*  disassemble code */ 


#include  "mawk.h"
#include  "code.h"
#include  "bi_funct.h"
#include  "repl.h"
#include  "field.h"

char *PROTO(find_bi_name, (PF_CP) ) ;

void  da(start, fp)
  INST *start ;
  FILE *fp ;
{ CELL *cp ;
  register INST *p = start ;

  while ( 1 )
  { /* print the relative code address (label) */
    fprintf(fp,"%03d ", p - start) ;

    switch( p++->op )
    {
      case _HALT :  fprintf(fp,"halt\n") ; return ;
      case _STOP :  fprintf(fp,"stop\n") ; break  ;
      case _STOP0 : fprintf(fp, "stop0\n") ; break ;

      case _PUSHC :
            cp = (CELL *) p++->ptr ;
            switch( cp->type )
            { case C_DOUBLE :
                  fprintf(fp,"pushc\t%.6g\n" ,  cp ->dval) ;
                  break ;

              case C_STRING :
                  fprintf(fp,"pushc\t\"%s\"\n" ,
                          ((STRING *)cp->ptr)->str) ;
                  break ;

              case C_RE :
                  fprintf(fp,"pushc\t0x%x\t/%s/\n" , cp->ptr ,
                    re_uncompile(cp->ptr) ) ;
                  break ;

              case C_SPACE : 
                  fprintf(fp, "pushc\tspace split\n") ;
                  break ;

              case C_SNULL : 
                  fprintf(fp, "pushc\tnull split\n") ;
                  break ;
              case C_REPL  :
                  fprintf(fp, "pushc\trepl\t%s\n" ,
                        repl_uncompile(cp) ) ;
                  break ;
              case C_REPLV :
                  fprintf(fp, "pushc\treplv\t%s\n" ,
                        repl_uncompile(cp) ) ;
                  break ;
                  
              default :
                  fprintf(fp,"pushc\tWEIRD\n") ;  ;
                  break ;
            }
            break ;

      case _PUSHA :
            fprintf(fp,"pusha\t0x%x\n", p++ -> ptr) ;
            break ;

      case _PUSHI :
            if ( (CELL *)p->ptr == field )
                fprintf(fp, "pushi\t$0\n") ;
            else fprintf(fp,"pushi\t0x%x\n", p -> ptr) ;
            p++ ;
            break ;

      case  L_PUSHA :
            fprintf( fp, "l_pusha\t%d\n", p++->op) ;
            break ;

      case  L_PUSHI :
            fprintf( fp, "l_pushi\t%d\n", p++->op) ;
            break ;

      case  LAE_PUSHI :
            fprintf( fp, "lae_pushi\t%d\n", p++->op) ;
            break ;

      case  LAE_PUSHA :
            fprintf( fp, "lae_pusha\t%d\n", p++->op) ;
            break ;

      case  LA_PUSHA :
            fprintf( fp, "la_pusha\t%d\n", p++->op) ;
            break ;

      case F_PUSHA :
            fprintf(fp,"f_pusha\t$%d\n" , (CELL *) p++->ptr - field ) ;
            break ;

      case F_PUSHI :
            fprintf(fp,"f_pushi\t$%d\n" , (CELL *) p++->ptr - field ) ;
            break ;

      case FE_PUSHA :
            fprintf(fp,"fe_pusha\n" ) ;
            break ;

      case FE_PUSHI :
            fprintf(fp,"fe_pushi\n" ) ;
            break ;

      case AE_PUSHA :
            fprintf(fp,"ae_pusha\t0x%x\n" , p++->ptr) ;
            break ;

      case AE_PUSHI :
            fprintf(fp,"ae_pushi\t0x%x\n" , p++->ptr) ;
            break ;

      case A_PUSHA :
            fprintf(fp,"a_pusha\t0x%x\n" , p++->ptr) ;
            break ;

      case A_TEST :
            fprintf(fp,"a_test\n" ) ;
            break ;

      case A_DEL :
            fprintf(fp,"a_del\n" ) ;
            break ;

      case A_CAT :
            fprintf(fp,"a_cat\t%d\n", p++->op ) ;
            break ;

      case _POP :
            fprintf(fp,"pop\n") ;
            break ;

      case  _ADD :
            fprintf(fp,"add\n") ; break ;

      case  _SUB :
            fprintf(fp,"sub\n") ; break ;
      case  _MUL :
            fprintf(fp,"mul\n") ; break ;
      case  _DIV :
            fprintf(fp,"div\n") ; break ;
      case  _MOD :
            fprintf(fp,"mod\n") ; break ;
      case  _POW :
            fprintf(fp,"pow\n") ; break ;
      case  _NOT :
            fprintf(fp,"not\n") ; break ;
      case  _UMINUS :
            fprintf(fp,"uminus\n") ; break ;
      case  _UPLUS :
            fprintf(fp,"plus\n") ; break ;
      case  _DUP :
            fprintf(fp,"dup\n") ; break ;
      case  _TEST :
            fprintf(fp,"test\n") ; break ;

      case  _CAT  :
            fprintf(fp,"cat\n") ; break ;

      case  _ASSIGN :
            fprintf(fp,"assign\n") ; break ;
      case  _ADD_ASG :
            fprintf(fp,"add_asg\n") ; break ;
      case  _SUB_ASG :
            fprintf(fp,"sub_asg\n") ; break ;
      case  _MUL_ASG :
            fprintf(fp,"mul_asg\n") ; break ;
      case  _DIV_ASG :
            fprintf(fp,"div_asg\n") ; break ;
      case  _MOD_ASG :
            fprintf(fp,"mod_asg\n") ; break ;
      case  _POW_ASG :
            fprintf(fp,"pow_asg\n") ; break ;

      case  F_ASSIGN :
            fprintf(fp,"f_assign\n") ; break ;
      case  F_ADD_ASG :
            fprintf(fp,"f_add_asg\n") ; break ;
      case  F_SUB_ASG :
            fprintf(fp,"f_sub_asg\n") ; break ;
      case  F_MUL_ASG :
            fprintf(fp,"f_mul_asg\n") ; break ;
      case  F_DIV_ASG :
            fprintf(fp,"f_div_asg\n") ; break ;
      case  F_MOD_ASG :
            fprintf(fp,"f_mod_asg\n") ; break ;
      case  F_POW_ASG :
            fprintf(fp,"f_pow_asg\n") ; break ;

      case  _PUSHINT :
            fprintf(fp,"pushint\t%d\n" , p++ -> op ) ;
            break ;

      case  _BUILTIN  :
            fprintf(fp,"%s\n" , 
                    find_bi_name( (PF_CP) p++ -> ptr ) ) ;
            break ;

      case  _PRINT :
            fprintf(fp,"%s\n", 
            (PF_CP) p++ -> ptr == bi_printf
                ? "printf" : "print") ;
            break ;
      
      case  _POST_INC :
            fprintf(fp,"post_inc\n") ; break ;

      case  _POST_DEC :
            fprintf(fp,"post_dec\n") ; break ;

      case  _PRE_INC :
            fprintf(fp,"pre_inc\n") ; break ;

      case  _PRE_DEC :
            fprintf(fp,"pre_dec\n") ; break ;

      case  F_POST_INC :
            fprintf(fp,"f_post_inc\n") ; break ;

      case  F_POST_DEC :
            fprintf(fp,"f_post_dec\n") ; break ;

      case  F_PRE_INC :
            fprintf(fp,"f_pre_inc\n") ; break ;

      case  F_PRE_DEC :
            fprintf(fp,"f_pre_dec\n") ; break ;

      case  _JMP :
      case  _JNZ :
      case  _JZ  :
          { int j = (p-1)->op ;
            char *s = j == _JMP ? "jmp" : 
                      j == _JNZ ? "jnz" : "jz" ;

            fprintf(fp,"%s\t\t%03d\n" , s ,
              (p - start) + p->op - 1 ) ;
            p++ ;
            break ;
          }
    
      case  _EQ  :
            fprintf(fp,"eq\n") ; break ;

      case  _NEQ  :
            fprintf(fp,"neq\n") ; break ;

      case  _LT  :
            fprintf(fp,"lt\n") ; break ;

      case  _LTE  :
            fprintf(fp,"lte\n") ; break ;

      case  _GT  :
            fprintf(fp,"gt\n") ; break ;

      case  _GTE  :
            fprintf(fp,"gte\n") ; break ;

      case  _MATCH :
            fprintf(fp,"match_op\n") ; break ;

      case  A_LOOP :
            fprintf(fp,"a_loop\t%03d\n", p-start+p[1].op) ;
            p += 2 ;
            break ;

      case  _EXIT  :
            fprintf(fp, "exit\n") ; break ;

      case  _EXIT0  :
            fprintf(fp, "exit0\n") ; break ;

      case  _NEXT  :
            fprintf(fp, "next\n") ; break ;

      case  _RET  :
            fprintf(fp, "ret\n") ; break ;
      case  _RET0 :
            fprintf(fp, "ret0\n") ; break ;

      case  _CALL :
            fprintf(fp, "call\t%s\t%d\n", 
                ((FBLOCK*)p->ptr)->name , p[1].op) ;
            p += 2 ;
            break ;

      case  _RANGE :
            fprintf(fp, "range\t%03d %03d %03d\n",
              /* label for pat2, action, follow */
              p - start + p[1].op ,
              p - start + p[2].op ,
              p - start + p[3].op ) ;
            p += 4 ; 
            break ;
      default :
            fprintf(fp,"bad instruction\n") ;
            return ;
    }
  }
}

static struct {
PF_CP action ;
char *name ;
} special_cases[] = {
bi_length, "length",
bi_split, "split",
bi_match, "match",
bi_getline,"getline",
bi_sub, "sub",
bi_gsub , "gsub",
(PF_CP) 0, (char *) 0 } ;

static char *find_bi_name( p )
  PF_CP p ;
{ BI_REC *q ;
  int i ;

  for( q = bi_funct ; q->name ; q++ )
    if ( q->fp == p )  /* found */
        return q->name ;
  /* next check some special cases */
  for( i = 0 ; special_cases[i].action ; i++)
    if ( special_cases[i].action == p )
        return  special_cases[i].name ;

  return  "unknown builtin" ;
}

static struct fdump {
struct fdump *link ;
FBLOCK  *fbp ;
}  *fdump_list ;  /* linked list of all user functions */

void add_to_fdump_list( fbp )
  FBLOCK *fbp ;
{ struct fdump *p = (struct fdump *)zmalloc(sizeof(struct fdump)) ;
  p->fbp = fbp ;
  p->link = fdump_list ;  fdump_list = p ;
}

void  fdump()
{
  register struct fdump *p, *q = fdump_list ;

  while ( p = q )
  { q = p->link ;
    fprintf(stderr, "function %s\n" , p->fbp->name) ;
    da(p->fbp->code, stderr) ;
    zfree(p, sizeof(struct fdump)) ;
  }
}
