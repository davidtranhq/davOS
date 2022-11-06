#!/bin/sh

PROJECTS=

# copy all necessary header files to the new OS root before building
$MAKE install-headers

# recurisvely build the sub-projects
for PROJECT in $PROJECTS; do
  (cd $PROJECT && DESTDIR="$SYSROOT" ./build-rec.sh)
done

# build this project
$MAKE install