
# test looping speed
#

BEGIN {

for(i=1; i<=100 ; i++)
{ 
  j = i ;
  while ( j >= 0 ) 
  {
    k = 0 
    do  
    { sum += k + j + i
      k++
    }
    while ( k <= j )
    j--
  }
}

print sum
}
