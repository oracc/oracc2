#!/bin/sh
echo $0
(cd /home/stinney/orc/pcsl ; pwd ; 00bin/update-pages.sh ; oracc build)
