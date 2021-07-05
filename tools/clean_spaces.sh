#!/bin/bash

files=`find . -name "*.h" -o -name "*.c" -o -name "*.cpp" -o -name "*.hpp"`

for file in $files
do
  echo "Processing $file..."
  sed -i "s/
  sed -i "s/[ \t]*$//g" $file
done