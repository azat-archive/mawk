
#  this is a makefile for mawk 
#  using Borland's make v3.0
#  and CFLAGS for TurboC

.SWAP

CFLAGS = -c -ms -f -wnod -wpro -A
LINKFLAGS = /c/m

!if $d(DEBUG)
CFLAGS = $(CFLAGS) -DDEBUG
LINKFLAGS = $(LINKFLAGS)/v
!else
#  optimize
CFLAGS = $(CFLAGS) -v- -O -DHAVE_REARGV=1
!endif


####################
#  user tuned macros
####################

# location of your C library
CLIB=c:\lib

# if you have a 80x87 coprocessor
#FLOATLIB=fp87
# otherwise
FLOATLIB=emu

#take this out if you don't need it, i.e.,
# if you have a reargv()
WILDARGS=\lib\wildargs.obj  #expand filenames

#  You'll need a yacc like parser
YACC = bison -dyv

######################  end of tuneable macros


OBS = parse.obj \
array.obj \
bi_funct.obj \
bi_vars.obj \
cast.obj \
code.obj \
da.obj \
error.obj \
execute.obj \
fcall.obj \
field.obj \
files.obj \
fin.obj \
hash.obj \
init.obj \
jmp.obj \
kw.obj \
main.obj \
matherr.obj \
memory.obj \
print.obj \
re_cmpl.obj \
scan.obj \
scancode.obj \
split.obj \
zmalloc.obj

REXP_C = rexp\rexp.c \
rexp\rexp0.c \
rexp\rexp1.c \
rexp\rexp2.c  \
rexp\rexpdb.c


LIBS = msdos\msdos.lib \
rexp\rexp.lib \
$(CLIB)\$(FLOATLIB) \
$(CLIB)\maths \
$(CLIB)\cs

mawk.exe : $(OBS)  rexp\rexp.lib
	tlink $(LINKFLAGS) @&&!
	$(CLIB)\c0s $(WILDARGS) $(OBS)
	mawk,mawk
	$(LIBS)
!

rexp\rexp.lib : $(REXP_C)
	cd rexp
	del *.obj
	make

.c.obj :
	tcc $(CFLAGS) $*.c 

parse.c :  parse.y
	$(YACC)  parse.y
	rename y_tab.c parse.c
	rename  y_tab.h parse.h

scancode.c :  makescan.c  scan.h
	tcc makescan.c
	makescan.exe > scancode.c
	rm makescan.obj  makescan.exe



# the dependencies of the obj's on h's
# you can make this with mawk -f deps.awk *.c

array.o : bi_vars.h sizes.h zmalloc.h memory.h types.h machine.h mawk.h symtype.h
bi_funct.o : fin.h bi_vars.h sizes.h memory.h zmalloc.h regexp.h types.h machine.h field.h repl.h files.h bi_funct.h mawk.h symtype.h init.h
bi_vars.o : bi_vars.h sizes.h memory.h zmalloc.h types.h machine.h field.h mawk.h symtype.h init.h
cast.o : parse.h sizes.h memory.h zmalloc.h types.h machine.h field.h scan.h repl.h mawk.h symtype.h
code.o : sizes.h memory.h zmalloc.h types.h machine.h code.h mawk.h init.h
da.o : sizes.h memory.h zmalloc.h types.h machine.h field.h repl.h code.h bi_funct.h mawk.h symtype.h
error.o : parse.h bi_vars.h sizes.h types.h machine.h scan.h mawk.h symtype.h
execute.o : sizes.h memory.h zmalloc.h regexp.h types.h machine.h field.h code.h repl.h bi_funct.h mawk.h symtype.h
fcall.o : sizes.h memory.h zmalloc.h types.h machine.h code.h mawk.h symtype.h
field.o : parse.h bi_vars.h sizes.h memory.h zmalloc.h regexp.h types.h machine.h field.h scan.h repl.h mawk.h symtype.h init.h
files.o : fin.h sizes.h memory.h zmalloc.h types.h machine.h files.h mawk.h
fin.o : parse.h fin.h bi_vars.h sizes.h memory.h zmalloc.h types.h machine.h field.h scan.h mawk.h symtype.h
hash.o : sizes.h memory.h zmalloc.h types.h machine.h mawk.h symtype.h
init.o : bi_vars.h sizes.h memory.h zmalloc.h types.h machine.h field.h code.h mawk.h symtype.h init.h
jmp.o : sizes.h memory.h zmalloc.h types.h machine.h code.h jmp.h mawk.h init.h
kw.o : parse.h sizes.h types.h machine.h mawk.h symtype.h init.h
main.o : fin.h bi_vars.h sizes.h memory.h zmalloc.h types.h machine.h field.h code.h files.h mawk.h init.h
makescan.o : parse.h scan.h symtype.h
matherr.o : sizes.h types.h machine.h mawk.h
memory.o : sizes.h memory.h zmalloc.h types.h machine.h mawk.h
parse.o : bi_vars.h sizes.h memory.h zmalloc.h types.h machine.h field.h code.h files.h bi_funct.h mawk.h jmp.h symtype.h
print.o : bi_vars.h parse.h sizes.h memory.h zmalloc.h types.h machine.h field.h scan.h files.h bi_funct.h mawk.h symtype.h
re_cmpl.o : parse.h sizes.h memory.h zmalloc.h regexp.h types.h machine.h scan.h repl.h mawk.h symtype.h
scan.o : parse.h fin.h sizes.h memory.h zmalloc.h types.h machine.h field.h scan.h repl.h files.h mawk.h symtype.h init.h
split.o : bi_vars.h parse.h sizes.h memory.h zmalloc.h regexp.h types.h machine.h field.h scan.h bi_funct.h mawk.h symtype.h
zmalloc.o : sizes.h zmalloc.h types.h machine.h mawk.h
