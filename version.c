
/********************************************
version.c
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the AWK programming language.

Mawk is distributed without warranty under the terms of
the GNU General Public License, version 2, 1991.
********************************************/

/*$Log:	version.c,v $
 * Revision 3.13.1.3  91/09/20  06:47:42  brennan
 * patchlevel.h is now patchlev.h
 * 
 * Revision 3.13.1.2  91/09/15  12:00:03  brennan
 * patchlev.h for DOS
 * 
 * Revision 3.13.1.1  91/09/14  17:24:29  brennan
 * VERSION 1.0
 * 
 * Revision 3.13  91/08/15  07:24:12  brennan
 * use patchlevel.h
 * 
 * Revision 3.12  91/08/13  06:52:17  brennan
 * VERSION .9994
 * 
 * Revision 3.11  91/07/19  07:51:37  brennan
 * escape sequence now recognized in command line assignments
 * 
 * Revision 3.10  91/07/17  15:12:51  brennan
 * Jun changed to Jul
 * 
 * Revision 3.9  91/07/17  10:44:01  brennan
 * changes in command line files -- dictated by posix;
 * Not a big deal, but better this way
 * version 0.9992
 * 
 * Revision 3.8  91/06/29  09:47:37  brennan
 * Only track NR if needed
 * version 0.9991
 * 
 * Revision 3.7  91/06/28  04:17:46  brennan
 * VERSION 0.999
 * 
 * Revision 3.6  91/06/26  05:57:46  brennan
 * fixed alignment bug , only showed on some machines, V9981
 * 
 * Revision 3.5  91/06/24  07:41:55  brennan
 * small bug fix in field and change wrt \'
 * 
 * Revision 3.4  91/06/19  10:24:43  brennan
 * changes for xenix_r2, call this version 0.997
 * 
 * Revision 3.3  91/06/15  09:06:55  brennan
 * new version
 * 
 * Revision 3.2  91/06/10  15:59:50  brennan
 * changes for V7
 * 
 * Revision 3.1  91/06/07  10:28:29  brennan
 * VERSION 0.995
 * 
 * Revision 1.4  91/06/04  06:54:25  brennan
 * new version
 * 
 * Revision 1.3  91/06/03  07:52:53  brennan
 * new version
 * 
 * Revision 1.2  91/05/30  09:05:13  brennan
 * input buffer can grow dynamically
 * 
 * Revision 1.1  91/05/29  13:46:42  brennan
 * Initial revision
 * 
*/

#include "mawk.h"

#include "patchlev.h"

#define  VERSION_STRING  \
  "mawk 1.0%s%s Sep 1991, Copyright (C) Michael D. Brennan\n\n"

#define  DOS_STRING	""

/* If use different command line syntax for MSDOS
   mark that in VERSION  */

#if  MSDOS  &&  ! HAVE_REARGV
#undef   DOS_STRING
#define  DOS_STRING	"DOS"
#endif

#ifdef THINK_C
#undef DOS_STRING
#define DOS_STRING ":Mac"
#endif

/* print VERSION and exit */
void print_version()
{ static char fmt[] = "%-20s%4d\n" ;

  printf(VERSION_STRING, PATCH_STRING, DOS_STRING) ;
  fflush(stdout) ;
  fprintf(stderr, "internal limits:\n") ;
  fprintf(stderr, fmt, "eval stack", EVAL_STACK_SIZE) ;
  fprintf(stderr, fmt, "split", MAX_SPLIT) ;
  fprintf(stderr, fmt, "fields", MAX_FIELD) ;
  fprintf(stderr, fmt, "sprintf", SPRINTF_SZ) ;
  exit(0) ;
}
