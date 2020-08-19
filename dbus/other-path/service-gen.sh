#!/usr/bin/bash

DIR=$(pwd)
TARGET="/usr/bin"

if [ -n "$1" ]
  then
    TARGET=${1%/}
fi

FILE='[D-BUS Service]\nName=org.dharkael.kylin-screenshot\nExec=##REPLACE##/kylin-screenshot'

FILE=${FILE//##REPLACE##/$TARGET}
echo -e $FILE > $DIR/org.dharkael.kylin-screenshot.service
