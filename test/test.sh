#! /bin/sh

###############
#  shell script for timing mawk and other awks
#
#  reads input file of the form
#  dump_or_not  program_file  input_file  list of awks
#
#  usage:  test.sh < input_file
#


if [ $# != 0 ]
then
  name=`basename $0`
  echo "usage: $name < input_file"  1>&2
  exit 1
fi


while  read  direct program file  awk_list
do

echo 
echo

if [ $direct = dump ]
then
for i in $awk_list
do
echo "$i -f $program $file"
/bin/time $i -f $program $file >/dev/null
done

else
for i in $awk_list
do
echo "$i -f $program $file"
/bin/time $i -f $program $file 
done
fi

done   2>&1    # send the timing to stdout
