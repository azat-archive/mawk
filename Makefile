
# ###################################################
# This is a makefile for mawk,
# an implementation of The AWK Programmin Language, 1988.
# 
# 

SHELL=/bin/sh

####################################
# CFLAGS needs to match a define in machine.h
# unless machine.h uses a built-in compiler flag
#

CFLAGS = -O -DULTRIX
#CFLAGS =  -O -DBSD43
YACC=yacc -dv
#YACC=bison -dvy

#######################################

O=parse.o scan.o memory.o main.o hash.o execute.o code.o\
  da.o error.o init.o bi_vars.o cast.o print.o bi_funct.o\
  kw.o jmp.o array.o field.o  split.o re_cmpl.o zmalloc.o\
  fin.o files.o  scancode.o matherr.o  fcall.o

REXP_C=rexp/rexp.c rexp/rexp0.c rexp/rexp1.c rexp/rexp2.c\
    rexp/rexp3.c rexp/rexpdb.c



mawk : $(O)  rexp/regexp.a
	cc $(CFLAGS) -o mawk $(O) -lm rexp/regexp.a

rexp/regexp.a :  $(REXP_C)
	cd  rexp ; make


parse.c  : parse.y
	@echo  expect 3 shift/reduce conflicts
	$(YACC)  parse.y
	mv y.tab.c parse.c
	-if cmp -s y.tab.h parse.h ;\
	   then rm y.tab.h ;\
	   else mv y.tab.h parse.h ; fi

scancode.c :  makescan.c  scan.h
	cc -o makescan.exe  makescan.c
	makescan.exe > scancode.c
	rm makescan.exe


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
