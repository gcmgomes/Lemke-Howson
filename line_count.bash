#!/bin/bash/

ROOTDIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

t=" "
for i in $(find ${ROOTDIR}/* | grep -e "\.cc$" -e "\.h$"); do
  t=$i" "$t
done
wc -l $t
