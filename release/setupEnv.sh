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

EXTERNALROOT=$PWD/../external
EXTERNALDIR=$EXTERNALROOT
for dir in `ls $EXTERNALROOT`; do
  if [ -d $EXTERNALROOT/$dir ]; then
    _dir=$EXTERNALROOT/$dir/release
    if [ -e $_dir -a -d $_dir ]; then
      EXTERNALDIR=$EXTERNALDIR:$_dir
      echo "Export external path $_dir ..."
    fi
    _dir=$EXTERNALROOT/$dir/release/lib
    if [ -e $_dir -a -d $_dir ]; then
      EXTERNALDIR=$EXTERNALDIR:$_dir
      echo "Export external path $_dir ..."
    fi
  fi
done

export LD_LIBRARY_PATH=$LOCALDIR:$EXTERNALDIR:$LD_LIBRARY_PATH
export LIBGL_ALWAYS_SOFTWARE=1
