
####################################
# This is a makefile for mawk,
# an implementation of AWK (1988).
####################################
#
#
# This builds a regular expression library
# Remove the -DMAWK and the library has general use.
# (Even if left in, the diff is very small)
#

CFLAGS = -O  -DMAWK

C=rexp.c rexp0.c rexp1.c rexp2.c rexp3.c rexpdb.c

regexp.a : $(C)
	rm -f *.o
	cc -c $(CFLAGS) $?
	ar r regexp.a *.o
	rm -f *.o



    

