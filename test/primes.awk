
# find all primes 
#  <= ARGV[1]
#
BEGIN {  stop = ARGV[1]
   prime[ p_cnt = 1 ] =  3

# keep track of integer part of square root by adding
# odd integers 
   odd = test = 5
   root = 2
   squares = 9

   
while ( test <= stop )
{
   if ( test >= squares )
   { root++
     odd += 2
     squares += odd 
   }

   flag = 1
   for ( i = 1 ; prime[i] <= root ; i++ )
   	if ( test % prime[i] == 0 )  #  not prime
	{ flag = 0 ; break }

   if ( flag )  prime[ ++p_cnt ] = test

   test += 2
}

prime[0] = 2

for(i = 0 ; i <= p_cnt ; i++ )  print prime[i]

}


     
