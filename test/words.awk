
# words0.awk

# find real words
# i.e contigous letters

BEGIN { FS = "[^A-Za-z]+" } # split fields on not letters

{
# $1 and $NF  might be  empty

  if ( NF > 0 )
  {
    cnt += NF

    if ( $NF == "" )  cnt--
    if ( NF > 1 && $1 == "" ) cnt--
  }
}

END { print cnt}


