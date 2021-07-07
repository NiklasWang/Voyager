#!/bin/bash

ROOT=`pwd`/..

currentdir=$ROOT
files=`find $currentdir -regex ".*\.h\|.*\.hpp\|.*\.c\|.*\.cpp\|.*Makefile*\|.*\.rule" -print | wc -l`
lines=`find $currentdir -regex ".*\.h\|.*\.hpp\|.*\.c\|.*\.cpp\|.*Makefile*\|.*\.rule" -print | xargs wc -l | tail -1`
echo "Voyager 0.1:"
echo -e "\033[42;30mSummary : $files files, $lines lines\033[0m"
