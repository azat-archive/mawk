

# compute square root by newton's method
#
function SQRT(x)
{ new = x/2
  do  
  { old = new
    new = (old*old+x)/(2*old)
  }
  while ( abs(new-old) > 1e-6 )

  return  (new+old)/2
}

function abs(x)
{ return x>=0?x:-x }

  
BEGIN {
  for(i = 1 ; i <= 1000 ; i++) print i, SQRT(i)
}
