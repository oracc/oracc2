#!/bin/sh
echo $0
(cd /home/stinney/orc/pcsl ; pwd ; 00bin/nacn-concordance.sh ; oracc build)
