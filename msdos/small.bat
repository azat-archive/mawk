echo off
rem  batch file to make small model mawk
rem  with a dos (not unix) command line
rem  using TurboC++ (should work with any Borland C)
rem   
rem   change LIB on the tlink command
rem   to the appropriate directory on your system
rem

echo  copy  config\tcc_dos.h  config.h
copy  config\tcc_dos.h  config.h

tcc -c -ms -f -K -d -G -v- -O parse.c
if errorlevel 1 goto ERR

tcc -c -ms -f -K -d -G -v- -O array.c
if errorlevel 1 goto ERR

tcc -c -ms -f -K -d -G -v- -O bi_funct.c
if errorlevel 1 goto ERR

tcc -c -ms -f -K -d -G -v- -O bi_vars.c
if errorlevel 1 goto ERR

tcc -c -ms -f -K -d -G -v- -O cast.c
if errorlevel 1 goto ERR

tcc -c -ms -f -K -d -G -v- -O code.c
if errorlevel 1 goto ERR

tcc -c -ms -f -K -d -G -v- -O da.c
if errorlevel 1 goto ERR

tcc -c -ms -f -K -d -G -v- -O error.c
if errorlevel 1 goto ERR

tcc -c -ms -f -K -d -G -v- -O execute.c
if errorlevel 1 goto ERR

tcc -c -ms -f -K -d -G -v- -O fcall.c
if errorlevel 1 goto ERR

tcc -c -ms -f -K -d -G -v- -O field.c
if errorlevel 1 goto ERR

tcc -c -ms -f -K -d -G -v- -O files.c
if errorlevel 1 goto ERR

tcc -c -ms -f -K -d -G -v- -O fin.c
if errorlevel 1 goto ERR

tcc -c -ms -f -K -d -G -v- -O hash.c
if errorlevel 1 goto ERR

tcc -c -ms -f -K -d -G -v- -O init.c
if errorlevel 1 goto ERR

tcc -c -ms -f -K -d -G -v- -O jmp.c
if errorlevel 1 goto ERR

tcc -c -ms -f -K -d -G -v- -O kw.c
if errorlevel 1 goto ERR

tcc -c -ms -f -K -d -G -v- -O main.c
if errorlevel 1 goto ERR

tcc -c -ms -f -K -d -G -v- -O matherr.c
if errorlevel 1 goto ERR

tcc -c -ms -f -K -d -G -v- -O memory.c
if errorlevel 1 goto ERR

tcc -c -ms -f -K -d -G -v- -O print.c
if errorlevel 1 goto ERR

tcc -c -ms -f -K -d -G -v- -O re_cmpl.c
if errorlevel 1 goto ERR

tcc -c -ms -f -K -d -G -v- -O scan.c
if errorlevel 1 goto ERR

tcc -c -ms -f -K -d -G -v- -O scancode.c
if errorlevel 1 goto ERR

tcc -c -ms -f -K -d -G -v- -O split.c
if errorlevel 1 goto ERR

tcc -c -ms -f -K -d -G -v- -O zmalloc.c
if errorlevel 1 goto ERR

tcc -c -ms -f -K -d -G -v- -O version.c
if errorlevel 1 goto ERR

tcc -c -ms -f -K -d -G -v- -O -DMAWK -Irexp rexp\rexp.c
if errorlevel 1 goto ERR

tcc -c -ms -f -K -d -G -v- -O -DMAWK -Irexp rexp\rexp0.c
if errorlevel 1 goto ERR

tcc -c -ms -f -K -d -G -v- -O -DMAWK -Irexp rexp\rexp1.c
if errorlevel 1 goto ERR

tcc -c -ms -f -K -d -G -v- -O -DMAWK -Irexp rexp\rexp2.c
if errorlevel 1 goto ERR

tcc -c -ms -f -K -d -G -v- -O -DMAWK -Irexp rexp\rexp3.c
if errorlevel 1 goto ERR

tcc -c -ms -f -K -d -G -v- -O -DMAWK -Irexp rexp\rexpdb.c
if errorlevel 1 goto ERR

tlink /c/x \LIB\c0s \LIB\wildargs @msdos\OBJS_TCC.LNK,mawk,,\LIB\emu \LIB\maths \LIB\cs
if errorlevel 1 goto LINKERR
echo  If you want to test mawk.exe, then
echo  copy all files in msdos\dostest to mawk\test
echo  copy mawk.exe to mawk\test
echo  and run mawk_tes.bat and fpe_test.bat
goto done

:ERR
echo  compile error
goto done

:LINKERR
echo  link  error

:done
