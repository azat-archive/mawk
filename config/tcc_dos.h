
/********************************************
tcc_dos.h
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the AWK programming language.

Mawk is distributed without warranty under the terms of
the GNU General Public License, version 2, 1991.
********************************************/

/* Turbo C under MSDOS */

/* $Log:	tcc_dos.h,v $
 * Revision 4.1  91/09/25  11:41:51  brennan
 * VERSION 1.0
 * 
 * Revision 3.2  91/08/13  09:04:19  brennan
 * VERSION .9994
 * 
 * Revision 3.1  91/06/07  10:38:51  brennan
 * VERSION 0.995
 * 
*/

#ifndef   CONFIG_H
#define   CONFIG_H      1

#define   MSDOS                 1

#define   HAVE_PROTOS           1
#define   HAVE_STDARG_H         1
#define   HAVE_STDLIB_H		1

#define   TURN_OFF_FPE_TRAPS()    _control87(0x3f,0x3f)

#ifndef   HAVE_SMALL_MEMORY   /* allow large model override */
#define   HAVE_SMALL_MEMORY     1
#endif

#include "config/Idefault.h"


#endif  /* CONFIG_H  */
