
/********************************************
bi_vars.h
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the Awk programming language as defined in
Aho, Kernighan and Weinberger, The AWK Programming Language,
Addison-Wesley, 1988.

See the accompaning file, LIMITATIONS, for restrictions
regarding modification and redistribution of this
program in source or binary form.
********************************************/


/* $Log:	bi_vars.h,v $
 * Revision 2.1  91/04/08  08:26:30  brennan
 * VERSION 0.97
 * 
*/


/* bi_vars.h */

#ifndef  BI_VARS_H
#define  BI_VARS_H  1

#define  VERSION_STRING  \
  "mawk 0.97 Mar 1991, Copyright (C) Michael D. Brennan"

/* If use different command line syntax for DOS
   mark that in VERSION  */

#if  DOS  &&  ! HAVE_REARGV
#undef   VERSION_STRING
#define  VERSION_STRING  \
  "mawk 0.97DOS Mar 1991, Copyright (C) Michael D. Brennan"
#endif

/* builtin variables NF, RS, FS, OFMT are stored
   internally in field[], so side effects of assignment can
   be handled 
*/

#define  ARGC      0
#define  FILENAME  1
#define  NR        2  /* NR must be exactly one in front of FNR */
#define  FNR       3
#define  OFS       4
#define  ORS       5
#define  RLENGTH   6
#define  RSTART    7
#define  SUBSEP    8
#define  VERSION   9
#define  NUM_BI_VAR  10

extern CELL bi_vars[NUM_BI_VAR] ;


#endif
