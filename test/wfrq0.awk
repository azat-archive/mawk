
#  this program is the same as wfrq.awk except it runs
#  about twice as fast because it uses RS as a regular 
#  expression
#

function down_heap(i,  k) 
{
  while ( 1 )
  {
      if ( count[2*i] <= count[2*i+1] )  k = 2*i
      else  k = 2*i + 1 

      if ( count[i] <= count[k] )  return

      hold = word[k] ; word[k] = word[i] ; word[i] = hold
      hold = count[k] ; count[k] = count[i] ; count[i] = hold
      i = k
   }
}


BEGIN { RS = "[^a-zA-Z]+" ;  BIG = 999999 }

{ cnt[$0]++ }

END { delete  cnt[ "" ]

# load twenty values
j = 1
for( i in cnt )
{
  word[j] = i  ; count[j] = cnt[i] 
  delete cnt[i] ;
  if ( ++j == 21 )  break ;
}

# make some sentinals
for( i = j ; i < 43 ; i++ )  count[i] = BIG

h_empty = j  # save the first empty slot
# make a heap with the smallest in slot 1
for( i = h_empty - 1 ; i > 0 ; i-- )  down_heap(i) 

# examine the rest of the values
for ( i in cnt )
  if ( (j = cnt[i]) > count[1] )
  { # its bigger
    # take the smallest out of the heap and readjust
    word[1] = i ; count[1] = j
    down_heap(1)
  }

h_empty-- ;

# what's left are the twenty largest
# smallest at the top
#

i = 20
while ( h_empty > 1 )
{
  buffer[i--] = sprintf ("%3d %s"  , count[1], word[1])
  count[1] = count[h_empty] ; word[1] = word[h_empty]
  count[h_empty] = BIG
  down_heap(1)
  h_empty--
}
  buffer[i--] = sprintf ("%3d %s"  , count[1], word[1])

  for(j = 1 ; j <= 20 ; j++ )  print buffer[j]
}
