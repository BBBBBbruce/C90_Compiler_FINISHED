#!/bin/bash

#!/bin/bash

# Try to find a portable way of getting rid of
# any stray carriage returns
if which dos2unix ; then
    DOS2UNIX="dos2unix"
elif which fromdos ; then
    DOS2UNIX="fromdos"
else
    # This works on a GNU version of sed. I think this
    # will work in OSX as well, but don't have a machine
    # on which to test that. From reading the OSX docs,
    # it looks compatible.
    # The code \x0D is the ASCII code of carriage-return,
    # so it the regex should delete any CRs at the end of
    # a line (or anywhere in a line)
    DOS2UNIX="sed -e s/\x0D//g -"
    # Tested for combinations of:
    # - Ubuntu
    # - Cygwin
    # and inputs formats:
    # - LF
    # - CRLF
fi


make clean

make all

mkdir -p running_data


external="test_deliverable/external_tests"
handin="test_deliverable/test_cases"
current="test_deliverable/current"
input_dir=$handin



if [[ "$1" != "" ]] ; then
    compiler="$1"
else
    compiler="bin/c_compiler"
fi



have_compiler=0
if [[ ! -f bin/c_compiler ]] ; then
    >&2 echo "Warning : cannot find compiler at path ${compiler}."
    have_compiler=1
fi

count=1
passnumber=0

echo ""
echo ""
echo ""
echo "================  tests start  ===================="

for i in ${input_dir}/*.c ; do
  if [[ ${i} != *"driver"* ]]; then
    base=$(echo $i | sed -E -e "s|${input_dir}/([^.]+)[.]c|\1|g");
    mkdir -p running_data/$base
    path="running_data/${base}"
    bin/c_compiler -S ${input_dir}/$base.c -o $path/$base.s &>>$path/${base}_log.txt
    #mips-linux-gnu-gcc -S ${input_dir}/$base.c -o $path/$base.s &>>$path/${base}_log.txt
    mips-linux-gnu-gcc -mfp32 -o $path/$base.o -c $path/$base.s &>>$path/${base}_log.txt
    mips-linux-gnu-gcc -mfp32 -static -o $path/$base $path/$base.o ${input_dir}/${base}_driver.c &>>$path/${base}_log.txt
    qemu-mips $path/$base &>>$path/${base}_log.txt
    if [ $? -ne 0 ]; then
      echo "test $count $base failed"
    else
      echo "test $count $base pass."
      let "passnumber ++"
    fi
    let "count ++"
    echo "++++++++++++++++++++++++++++++++++++++++"
    echo ""
  fi
done
let "count --"
echo "++++++++++++++++++++++++++++++"
echo "++ $passnumber out of $count tests passed ++"
echo "++++++++++++++++++++++++++++++"
