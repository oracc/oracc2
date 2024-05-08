#!/bin/dash
#
# Helper program for px search
#
proj=$1
srchtmp=$2
/home/oracc/bin/qx -8 -x $srchtmp -j $proj
