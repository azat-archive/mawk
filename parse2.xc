
/********************************************
parse2.xc
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the Awk programming language as defined in
Aho, Kernighan and Weinberger, The AWK Programming Language,
Addison-Wesley, 1988.

See the accompaning file, LIMITATIONS, for restrictions
regarding modification and redistribution of this
program in source or binary form.
********************************************/


/* $Log:	parse2.xc,v $
Revision 3.4.1.1  91/09/25  13:26:16  brennan
VERSION 1.0

Revision 3.4  91/06/28  04:19:26  brennan
VERSION 0.999

Revision 3.3  91/06/26  14:13:00  brennan
#ifdef YYBYACC was in the wrong spot

Revision 3.2  91/06/25  07:07:23  brennan
changed name to parse2.xc, so people won't try to compile it

 * Revision 3.1  91/06/07  10:28:05  brennan
 * VERSION 0.995
 * 
 * Revision 1.1  91/06/03  15:39:31  brennan
 * Initial revision
 * 
*/

/* If using Berkeley yacc, we can put the parser table 
   memory to the zmalloc pool.  This is kind of ugly and
   with paged vm probably a nop, but for DOS and MINIX and ??
   it frees a considerably amount of memory.

   This file is part of parse.c via
      cat  y.tab.c  parse2.xc  > parse.c
*/

static struct yacc_mem   yacc_mem[] = 
{
0 , 0 ,  /* don't remove this */

#ifndef THINK_C
#ifdef   YYBYACC
(PTR) yycheck, sizeof(yycheck)/8,
(PTR) yytable, sizeof(yytable)/8,
(PTR) yyvs , sizeof(yyvs)/8,
(PTR) yyss, sizeof(yyss)/8,
(PTR) yydefred, sizeof(yydefred)/8,
(PTR) yydgoto, sizeof(yydgoto)/8,
(PTR) yygindex, sizeof(yygindex)/8,
(PTR) yylen, sizeof(yylen)/8,
(PTR) yylhs, sizeof(yylhs)/8,
(PTR) yyrindex, sizeof(yyrindex)/8,
(PTR) yysindex, sizeof(yysindex)/8,
#endif
#endif

0,0 } ;

struct yacc_mem  *yacc_memp = yacc_mem ;
