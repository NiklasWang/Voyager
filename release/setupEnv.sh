#!/bin/sh

echo "Instruction:"
echo "source \`this_file_name\` to load settings"

PWD=`pwd`

LOCALDIR=$PWD
for dir in `ls .`; do
  if [ -d $dir ]; then
    _dir=$PWD/$dir
    LOCALDIR=$LOCALDIR:$_dir
    echo "Export local path $_dir ..."
  fi
done

export LD_LIBRARY_PATH=$LOCALDIR:$LD_LIBRARY_PATH
