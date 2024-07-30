#!/bin/sh
(cd $ORACC/www/p4.d ; for a in [a-z][a-z0-9][a-z0-9][a-z0-9]* ; do pxreset.sh $a ; done)
