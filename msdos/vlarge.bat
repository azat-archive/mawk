echo off
rem  batch file to make large model mawk (bmawk.exe "big mawk")
rem  with a  unix command line
rem  using TurboC++ (should work with any Borland C)
rem
rem   change LIB on the tlink command
rem   to the appropriate directory on your system
rem
rem   you have to provide  reargv.c

echo  copy  msdos\tcc_dos.h  config.h
copy  msdos\tcc_dos.h  config.h

tcc -c -ml -f -K -d -G -v- -O -DHAVE_SMALL_MEMORY=0 -DHAVE_REARGV=1 reargv.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -O -DHAVE_SMALL_MEMORY=0 -DHAVE_REARGV=1 parse.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -O -DHAVE_SMALL_MEMORY=0 -DHAVE_REARGV=1 array.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -O -DHAVE_SMALL_MEMORY=0 -DHAVE_REARGV=1 bi_funct.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -O -DHAVE_SMALL_MEMORY=0 -DHAVE_REARGV=1 bi_vars.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -O -DHAVE_SMALL_MEMORY=0 -DHAVE_REARGV=1 cast.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -O -DHAVE_SMALL_MEMORY=0 -DHAVE_REARGV=1 code.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -O -DHAVE_SMALL_MEMORY=0 -DHAVE_REARGV=1 da.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -O -DHAVE_SMALL_MEMORY=0 -DHAVE_REARGV=1 error.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -O -DHAVE_SMALL_MEMORY=0 -DHAVE_REARGV=1 execute.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -O -DHAVE_SMALL_MEMORY=0 -DHAVE_REARGV=1 fcall.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -O -DHAVE_SMALL_MEMORY=0 -DHAVE_REARGV=1 field.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -O -DHAVE_SMALL_MEMORY=0 -DHAVE_REARGV=1 files.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -O -DHAVE_SMALL_MEMORY=0 -DHAVE_REARGV=1 fin.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -O -DHAVE_SMALL_MEMORY=0 -DHAVE_REARGV=1 hash.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -O -DHAVE_SMALL_MEMORY=0 -DHAVE_REARGV=1 init.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -O -DHAVE_SMALL_MEMORY=0 -DHAVE_REARGV=1 jmp.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -O -DHAVE_SMALL_MEMORY=0 -DHAVE_REARGV=1 kw.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -O -DHAVE_SMALL_MEMORY=0 -DHAVE_REARGV=1 main.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -O -DHAVE_SMALL_MEMORY=0 -DHAVE_REARGV=1 matherr.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -O -DHAVE_SMALL_MEMORY=0 -DHAVE_REARGV=1 memory.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -O -DHAVE_SMALL_MEMORY=0 -DHAVE_REARGV=1 print.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -O -DHAVE_SMALL_MEMORY=0 -DHAVE_REARGV=1 re_cmpl.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -O -DHAVE_SMALL_MEMORY=0 -DHAVE_REARGV=1 scan.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -O -DHAVE_SMALL_MEMORY=0 -DHAVE_REARGV=1 scancode.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -O -DHAVE_SMALL_MEMORY=0 -DHAVE_REARGV=1 split.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -O -DHAVE_SMALL_MEMORY=0 -DHAVE_REARGV=1 zmalloc.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -O -DHAVE_SMALL_MEMORY=0 -DHAVE_REARGV=1 version.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -O -DMAWK -DLMDOS -Irexp rexp\rexp.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -O -DMAWK -DLMDOS -Irexp rexp\rexp0.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -O -DMAWK -DLMDOS -Irexp rexp\rexp1.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -O -DMAWK -DLMDOS -Irexp rexp\rexp2.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -O -DMAWK -DLMDOS -Irexp rexp\rexp3.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -O -DMAWK -DLMDOS -Irexp rexp\rexpdb.c
if errorlevel 1 goto ERR

tlink /c/x \LIB\c0l \LIB\wildargs reargv @msdos\OBJS_TCC.LNK,bmawk,,\LIB\emu \LIB\mathl \LIB\cl
if errorlevel 1 goto LINKERR
echo  If you want to test bmawk.exe, then
echo  copy all files in msdos\dostest to mawk\test
echo  copy bmawk.exe to mawk\test\mawk.exe
echo  and run mawk_tes.bat and fpe_test.bat
goto done

:ERR
echo  compile error
goto done

:LINKERR
echo  link  error

:done
