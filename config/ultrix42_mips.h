
/********************************************
ultrix42_mips.h
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the AWK programming language.

Mawk is distributed without warranty under the terms of
the GNU General Public License, version 2, 1991.
********************************************/


/* $Log:	ultrix42_mips.h,v $
 * Revision 1.1  91/10/29  10:48:48  brennan
 * Initial revision
 * 
*/


#ifndef   CONFIG_H
#define   CONFIG_H	1

#define HAVE_PROTOS	1
#define HAVE_STDLIB_H   1
#define HAVE_STDARG_H   1
#define HAVE_PRINTF_HD  1

/* The only reason __STDC__ is not turned on is 
   compiler doesn't recognize const  */

#include "config/Idefault.h"
#endif /* CONFIG_H  */
