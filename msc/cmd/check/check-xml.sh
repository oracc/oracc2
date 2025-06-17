#!/bin/sh
exec 2>check-xml.log
for a in 00atf/*.otf ; do
    >2 echo Checking $a
    oxx $a | xl - >/dev/null
done
