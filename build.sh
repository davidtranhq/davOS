#!/bin/sh
set -e
. ./config.sh

PROJECTS="kernel"

# root directory of the new OS
mkdir -p "$SYSROOT"

# recurisvely build the sub-projects in each project
for PROJECT in $PROJECTS; do
  (cd $PROJECT && DESTDIR="$SYSROOT" ./build-rec.sh)
done
