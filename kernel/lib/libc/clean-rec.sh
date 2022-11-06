#!/bin/sh
set -e

PROJECTS=""

for PROJECT in $PROJECTS; do
  (cd $PROJECT && ./clean-rec.sh)
done

$MAKE clean