

# use non letters as RS
#
#

BEGIN { RS = "[^A-Za-z][^A-Za-z]*" 
    getline
    if ( $0 == "" )  NR = 0
}

END { print NR }
