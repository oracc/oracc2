#!/bin/sh
echo $0 checking esp2 files in 00web and subdirectories ...
exec 2>check-esp.log
for a in `find 00web -name '*.xml'` ; do
    >&2 echo Checking $a
    xl $a >/dev/null
    if [ $? -ne 0 ]; then
	echo XML errors detected in $a
    fi
done
cat check-esp.log
echo "$0 check complete; log written to check-esp.log"
