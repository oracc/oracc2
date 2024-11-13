#!/bin/sh
oidx -c oid.tab
if [ $? -eq 0 ]; then
    rocox -x oids -R '<oid id="%1" dom="%2" key="%3"/>' <oid.tab >oid.xml
else
    echo $0: errors checking oid.tab--oid.xml not updated.
fi
