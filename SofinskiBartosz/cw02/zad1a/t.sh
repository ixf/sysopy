#!/bin/bash

exe() { echo "\$ $@" ; "$@" ; }

if [ "$1" == "generate" ]; then
exe ./main generate kappa.txt 10 4
elif [ "$1" == "sort" ]; then
exe ./main sort kappa.txt 10 4 $2
elif [ "$1" == "copy" ]; then
exe ./main copy kappa.txt kappa_copy.txt 10 4 $2
else
echo "Wrong arg 1"
fi
