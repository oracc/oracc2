#!/bin/dash
(cd /home/stinney/orc/pcsl ; 00bin/pre-build-update.sh)
(cd /home/stinney/orc/pcsl ; sudo cp -a 00lib/pcsl.asl /home/oracc/pcsl/00lib)
(cd /home/oracc/pcsl ; sudo /home/oracc/bin/orc /home/oracc/bin/sx-update.sh)
