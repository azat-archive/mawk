
/********************************************
msc_dos.h
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the AWK programming language.

Mawk is distributed without warranty under the terms of
the GNU General Public License, version 2, 1991.
********************************************/

/* Microsoft C 6.0A under MSDOS */

#ifndef   CONFIG_H
#define   CONFIG_H      1

#define   MSC			1
#define   MSDOS                 1

#define   HAVE_PROTOS           1
#define   HAVE_STDARG_H         1
#define   HAVE_STDLIB_H		1

#define   FPE_TRAPS_ON		1
#define   NOINFO_SIGFPE		1

#ifndef   HAVE_SMALL_MEMORY   /* allow large model override */
#define   HAVE_SMALL_MEMORY     1
#endif

#include "config/Idefault.h"


#endif  /* CONFIG_H  */
