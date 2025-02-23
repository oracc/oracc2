#!/bin/sh
echo $0
(cd /home/stinney/orc/pcsl/pc25/w ; make)
(cd /home/stinney/orc/pcsl ; pwd ; 00bin/update-pages.sh ; oracc build)
