
/*  rand48.h  */
/*  see Unix drand(3) for expansive discussion */

double  drand48(void) ;
double  erand48(unsigned short[3]) ;

long   mrand48(void) ;
long   jrand48(unsigned short[3]) ;
/* uniform on [-2^31, 2^31)  or
   [0,2^32)  depending on how you interpret the sign bit */

long  lrand48(void) ;
long  nrand48(unsigned short[3]) ;
/* uniform on [0,2^31)
   lrand48() == mrand48()>>1   */

unsigned  urand48(void) ;
unsigned  vrand48(unsigned short[3]) ;
/*  for 16bit machines uniform on [0,2^16)  */

/* SEEDING  */
void  srand48(long seedval) ;
unsigned short *seed48(unsigned short seedv[3]) ;
void  lcong(unsigned short[7] ) ;


void  srand48() ;
long  mrand48(), jrand48(), lrand48(), nrand48() ;
unsigned urand48(), vrand48() ;
double  drand48(), erand48() ;
