
# remove C comments
#

BEGIN {
 RS = "/\*([^*]|\*[^/])*\*/"
 ORS = " "
 getline hold
}

{ # hold one record because we don't want ORS on the last
  # record

  print hold
  hold = $0
}

END { printf "%s", hold }
