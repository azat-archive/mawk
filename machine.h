
/********************************************
machine.h
copyright 1991, Michael D. Brennan

This is a source file for mawk, an implementation of
the Awk programming language as defined in
Aho, Kernighan and Weinberger, The AWK Programming Language,
Addison-Wesley, 1988.

See the accompaning file, LIMITATIONS, for restrictions
regarding modification and redistribution of this
program in source or binary form.
********************************************/

/*$Log:	machine.h,v $
 * Revision 2.2  91/04/09  12:39:14  brennan
 * added static to funct decls to satisfy STARDENT compiler
 * 
 * Revision 2.1  91/04/08  08:23:25  brennan
 * VERSION 0.97
 * 
*/


/* I've attempted to isolate machine/system dependencies here.

   Floating point exceptions are the biggest hassle.
   If you have IEEE754 floating point, turn off floating point
   traps and let the INFs and NANs go berserk.  This should be
   the default (see page 14 of IEEE754), but ANSI C seems to imply
   they should be on (i.e., one standards committee does not talk to
   the other).  Anyway, define a macro TURNOFF_FPE_TRAPS() which will
   probably be a 1 liner.

   If you cannot turn off floating exceptions, check out
   fpe_catch() in matherr.c and modify as needed for your machine.
   Also you may need to define FPE_ZERODIVIDE and FPE_OVERFLOW.

   If you have SysV like matherr(), use it.
   If you have SysV compatible math lib , use it.
   You might need to supply a macro to replace drand48(), otherwise.
   (See BSD43 for no IEEE754, no matherr(), no fmod(),
    no strtod(), no drand48())

   If you have to be conservative with memory (e.g., small model
   MsDos), a small evaluation stack (16-32) is plenty.
   Recursive functions calls are the only reason you need a big
   stack.  The default for MsDos uses 64 which allows some
   recursion without killing too many memory CELLs.
*/

/*  MsDOS --
    If you use command.com as the shell, entering programs on the
    command line is hopeless.  Command.com will always glom onto
    | or < or > as redirection.

    If you use a Unix style shell under DOS, then you need to
    write 

        void  reargv(int *argc, char ***argv)

    which gets the arguments from your shell, and then

    #define   HAVE_REARGV	1

    See README in dos directory
    and MsDos section of manual.
*/

#ifndef    MACHINE_H
#define    MACHINE_H


#ifdef  sun   /* sun3 or sun4 with SUNOS 4.0.3 */
#define  FPE_TRAPS              0
#define  TURNOFF_FPE_TRAPS()    /* empty, default is off */      
#define  HAVE_MATHERR           1
#endif

#ifdef    __TURBOC__
#define   DOS    		1
#define   SMALL_EVAL_STACK      1
#define  FPE_TRAPS              0
#define  TURNOFF_FPE_TRAPS()    _control87(0x3f,0x3f)
#define  HAVE_MATHERR           1
#endif

#ifdef   ULTRIX  /* V4.1 on a vax 3600 */
#define  HAVE_VOID_PTR          1
#define  HAVE_MATHERR           1
#define   FPE_ZERODIVIDE   FPE_FLTDIV_FAULT
#define   FPE_OVERFLOW     FPE_FLTOVF_FAULT
#endif

#ifdef    BSD43   /* on a vax */
#define   NO_STRTOD             1
#define   NO_FMOD               1
#define   srand48(x)    srandom(x)
#define   drand48() (((double)random())/((double)(unsigned)0x80000000))
#define   vfprintf(s,f,a)  _doprnt(f,a,s)
#define   FPE_ZERODIVIDE   FPE_FLTDIV_FAULT
#define   FPE_OVERFLOW     FPE_FLTOVF_FAULT
#endif

#ifdef   STARDENT  /* Stardent 3000, SysV R3.0 */
#define  HAVE_MATHERR	1
#define  FPE_TRAPS	0
#define  TURNOFF_FPE_TRAPS()    /* nothing */
#define  HAVE_VOID_PTR    1
#endif

/*  the defaults    */
#ifndef   HAVE_VOID_PTR
#define   HAVE_VOID_PTR         0  /* no void * */
#endif

#ifndef   FPE_TRAPS
#define   FPE_TRAPS             1  
    /* floating point errors generate exceptions  */
#endif

#ifndef   HAVE_MATHERR
#define   HAVE_MATHERR          0
    /*  SysV style matherr() is not available */
#endif

#ifndef  NO_STRTOD
#define  NO_STRTOD              0 /* default is have */
#endif

#ifndef  SMALL_EVAL_STACK
#define  SMALL_EVAL_STACK       0
#endif

#ifndef  NO_FMOD
#define  NO_FMOD                0 /* default is to have fmod() */
#endif

#define   STDC_MATHERR          (FPE_TRAPS && ! HAVE_MATHERR)

#ifndef   DOS
#define   DOS	0
#endif

#if   DOS 

#ifndef  HAVE_REARGV
#define  HAVE_REARGV	0
#endif

#define  DOS_PROMPT     "mawk> "
	/* change to "mawk \01 "  on a good day */
#endif





#endif   /* MACHINE_H  */
