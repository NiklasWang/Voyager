#!/bin/bash

# $1 - Keyword to search
# $* - Which directory and it's sub directory to perform search

DIRS=$*
DIRS=${DIRS/$1/}

FILES=$(find $DIRS -name "$1" -type d)
for FILE in $FILES; do
  echo $FILE
done
