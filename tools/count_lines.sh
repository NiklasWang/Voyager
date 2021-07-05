#!/bin/bash

ROOT=`pwd`/..

legacydir=$ROOT/legacy/Pandora_v1.4.1
legacyfiles=`find $legacydir -path $legacydir/external -prune -o -regex ".*\.h\|.*\.hpp\|.*\.c\|.*\.cpp\|.*Makefile*\|.*\.rule\|.*\.xml\|.*\.xsd" -print | wc -l`
legacylines=`find $legacydir -path $legacydir/external -prune -o -regex ".*\.h\|.*\.hpp\|.*\.c\|.*\.cpp\|.*Makefile*\|.*\.rule\|.*\.xml\|.*\.xsd" -print | xargs wc -l | tail -1`
echo "Pandora 2.x:"
echo "Summary All : $legacyfiles files, $legacylines lines"
echo ""

currentdir=$ROOT
codefiles=`find $currentdir \( -path $currentdir/external -o -path $currentdir/legacy -o -path $currentdir/pipeline/tester -o -path $currentdir/pipeline/testerxml -o -path $currentdir/pipeline/simulator -o -path $currentdir/pipeline/simulatorxml \) -prune -o -regex ".*\.h\|.*\.hpp\|.*\.c\|.*\.cpp\|.*Makefile*\|.*\.rule\|.*\.xml\|.*\.xsd" -print | wc -l`
codelines=`find $currentdir \( -path $currentdir/external -o -path $currentdir/legacy -o -path $currentdir/pipeline/tester -o -path $currentdir/pipeline/testerxml -o -path $currentdir/pipeline/simulator -o -path $currentdir/pipeline/simulatorxml \) -prune -o -regex ".*\.h\|.*\.hpp\|.*\.c\|.*\.cpp\|.*Makefile*\|.*\.rule\|.*\.xml\|.*\.xsd" -print | xargs wc -l | tail -1`
testerfiles=`find $currentdir/pipeline/tester $currentdir/pipeline/simulator -regex ".*\.h\|.*\.hpp\|.*\.c\|.*\.cpp\|.*Makefile*\|.*\.rule\|.*\.xml\|.*\.xsd" -print | wc -l`
testerlines=`find $currentdir/pipeline/tester $currentdir/pipeline/simulator -regex ".*\.h\|.*\.hpp\|.*\.c\|.*\.cpp\|.*Makefile*\|.*\.rule\|.*\.xml\|.*\.xsd" -print | xargs wc -l | tail -1`
allfiles=`find $currentdir \( -path $currentdir/external -o -path $currentdir/legacy -o -path $currentdir/pipeline/testerxml -o -path $currentdir/pipeline/simulatorxml \) -prune -o -regex ".*\.h\|.*\.hpp\|.*\.c\|.*\.cpp\|.*Makefile*\|.*\.rule\|.*\.xml\|.*\.xsd" -print | wc -l`
alllines=`find $currentdir \( -path $currentdir/external -o -path $currentdir/legacy -o -path $currentdir/pipeline/testerxml -o -path $currentdir/pipeline/simulatorxml \) -prune -o -regex ".*\.h\|.*\.hpp\|.*\.c\|.*\.cpp\|.*Makefile*\|.*\.rule\|.*\.xml\|.*\.xsd" -print | xargs wc -l | tail -1`
echo "Pandora 3.0:"
echo "Code        : $codefiles files, $codelines lines"
echo "Unit Tester : $testerfiles files, $testerlines lines"
echo -e "\033[42;30mSummary All : $allfiles files, $alllines lines\033[0m"
