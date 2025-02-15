#!/bin/sh
echo $0
(cd /home/stinney/orc/pcsl/00etc/pc25 ; make)
(cd /home/stinney/orc/pcsl ; pwd ; 00bin/update-pages.sh ; oracc build)
