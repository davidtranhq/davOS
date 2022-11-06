#!/bin/sh
set -e

PROJECTS="lib/libc"

for PROJECT in $PROJECTS; do
  (cd $PROJECT && ./clean-rec.sh)
done

$MAKE clean