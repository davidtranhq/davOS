#!/bin/sh
set -e
. ./config.sh

PROJECTS="kernel"

# clean recurisvely
for PROJECT in $PROJECTS; do
  (cd $PROJECT && ./clean-rec.sh)
done

rm -rf sysroot
rm -rf isodir
rm -rf davos.iso
