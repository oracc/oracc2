#!/bin/dash
#
# This script needs to find all the .tis files in a project and
# compile them into the tis db.
#
echo $0
cat 01tmp/g.tis | cut -f2,5 | dbix -s -d 02pub -n tis
chmod o+r 02pub/tis.*
