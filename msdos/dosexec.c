
/********************************************
dosexec.h
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the AWK programming language.

Mawk is distributed without warranty under the terms of
the GNU General Public License, version 2, 1991.
********************************************/

/*$Log:	dosexec.c,v $
 * Revision 1.2  91/11/16  10:27:18  brennan
 * BINMODE 
 * 
 * Revision 1.1  91/10/29  09:45:56  brennan
 * Initial revision
 * 
*/

/* system() and pipes() for MSDOS */

#include "mawk.h"

#if MSDOS
#include "memory.h"
#include "files.h"
#include "fin.h"

#include <process.h>

static void PROTO(get_shell, (void)) ;

static char *shell ;     /* e.g.   "c:\\sys\\command.com"  */
static char *command_opt ;  /*  " /c"  */

static void get_shell()
{ char *s , *p ;
  int len ;

  if ( !(s = getenv("MAWKSHELL")) )
  {
    errmsg(0, "MAWKSHELL not set in environment") ; exit(1) ;
  }
  p = s ;
  while ( *p != ' ' && *p != '\t' ) p++ ;
  len = p - s ;
  shell = (char *) zmalloc( len + 1 ) ;
  (void) memcpy(shell, s, len) ;  shell[len] = 0 ;
  command_opt = p ;
}


/* large model use spawnl() so we don't have to understand
   far memory management

   small model use assembler routine xDOSexec() to save code
   space
*/

int DOSexec( command )
  char *command ;
{
#if  SM_DOS
  extern int PROTO(xDOSexec, (char *, void*)) ;
  struct {
    int env_seg ;
    char far *cmdline ;
    unsigned fcb[4] ;
  } block ;
  int len ;
#endif

  char xbuff[256] ;

  if ( ! shell )  get_shell() ;

  (void) sprintf(xbuff+1, "%s %s", command_opt, command) ;

#if  SM_DOS
  block.env_seg = 0 ;
  block.cmdline = xbuff ;
  (void) memset(block.fcb, 0xff, sizeof(block.fcb)) ;
  len = strlen(xbuff+1) ;
  if ( len > 126 ) len = 126 ;
  xbuff[0] = len ;
  xbuff[len+1] = '\r' ;
#endif

  (void) fflush(stderr) ; (void) fflush(stdout) ;

#if SM_DOS
  return xDOSexec(shell, &block) ;
#else
  return   spawnl(P_WAIT, shell, shell, xbuff+1, (char *) 0 ) ;
#endif
}


static int next_tmp ;
static char *tmpdir ;

/* put the name of a temp file in string buff */

char *tmp_file_name( id )
  int id ;
{
  (void) sprintf(string_buff, "%sMAWK%04X.TMP", tmpdir, id) ;
  return string_buff ;
}

PTR  get_pipe( command, type, tmp_idp)
  char *command ;
  int type, *tmp_idp ;
{
  PTR  retval ;
  char *tmpfile ;

  if ( ! tmpdir )
  {
    tmpdir = getenv("MAWKTMPDIR") ;
    if ( ! tmpdir )  tmpdir = "" ;
  }

  *tmp_idp = next_tmp ;
  tmpfile = tmp_file_name(next_tmp) ;

  if ( type == PIPE_OUT )  
      retval = (PTR) fopen(tmpfile, (binmode()&2)? "wb":"w") ;
  else
  { char xbuff[256] ;
    
    sprintf(xbuff, "%s > %s" , command, tmpfile) ;
    DOSexec(xbuff) ;
    retval = (PTR) FINopen(tmpfile, 0) ;
  }

  next_tmp++ ;
  return retval ;
}

/* closing a fake pipes involves running the out pipe 
   command
*/

int close_fake_outpipe(command, tid)
  char *command ;
  int tid ; /* identifies the temp file */
{
  char *tmpname = tmp_file_name(tid) ;
  char xbuff[256] ;
  int retval ;

  sprintf(xbuff, "%s < %s", command, tmpname) ;
  retval = DOSexec(xbuff) ;
  (void) unlink(tmpname) ;
  return retval ;
}

#endif  /* MSDOS */



