echo off
rem  batch file to make large model mawk  (bmawk.exe "big mawk")
rem  with a dos (not unix) command line
rem  using TurboC++ (should work with any Borland C)
rem   
rem   change LIB on the tlink command
rem   to the appropriate directory on your system
rem

echo copy  config\tcc_dos.h  config.h
copy  config\tcc_dos.h  config.h

tcc -c -ml -f -K -d -G -v- -DHAVE_SMALL_MEMORY=0 -O parse.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -DHAVE_SMALL_MEMORY=0 -O array.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -DHAVE_SMALL_MEMORY=0 -O bi_funct.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -DHAVE_SMALL_MEMORY=0 -O bi_vars.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -DHAVE_SMALL_MEMORY=0 -O cast.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -DHAVE_SMALL_MEMORY=0 -O code.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -DHAVE_SMALL_MEMORY=0 -O da.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -DHAVE_SMALL_MEMORY=0 -O error.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -DHAVE_SMALL_MEMORY=0 -O execute.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -DHAVE_SMALL_MEMORY=0 -O fcall.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -DHAVE_SMALL_MEMORY=0 -O field.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -DHAVE_SMALL_MEMORY=0 -O files.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -DHAVE_SMALL_MEMORY=0 -O fin.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -DHAVE_SMALL_MEMORY=0 -O hash.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -DHAVE_SMALL_MEMORY=0 -O init.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -DHAVE_SMALL_MEMORY=0 -O jmp.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -DHAVE_SMALL_MEMORY=0 -O kw.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -DHAVE_SMALL_MEMORY=0 -O main.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -DHAVE_SMALL_MEMORY=0 -O matherr.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -DHAVE_SMALL_MEMORY=0 -O memory.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -DHAVE_SMALL_MEMORY=0 -O print.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -DHAVE_SMALL_MEMORY=0 -O re_cmpl.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -DHAVE_SMALL_MEMORY=0 -O scan.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -DHAVE_SMALL_MEMORY=0 -O scancode.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -DHAVE_SMALL_MEMORY=0 -O split.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -DHAVE_SMALL_MEMORY=0 -O zmalloc.c
if errorlevel 1 goto ERR

tcc -c -ml -f -K -d -G -v- -DHAVE_SMALL_MEMORY=0 -O version.c
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

tlink /c/x \LIB\c0l \LIB\wildargs @msdos\OBJS_TCC.LNK,bmawk,,\LIB\emu \LIB\mathl \LIB\cl
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
