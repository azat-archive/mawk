

/*   rand0.c    */

unsigned long  mrand48() ;
unsigned long  jrand48(short [3]) ;

double  drand48()
{ return  (double) mrand48() / 4294967296.0 ; }

double  erand48(short x[3])
{ return  (double) jrand48(x) /  4294967296.0 ; }
