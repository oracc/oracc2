#!/bin/dash
(cd /home/stinney/orc/pcsl ;
 sudo cp -a 00lib/pcsl.asl /home/oracc/pcsl/00lib ;
 sudo cp -a 00etc/im-cdli-gh.tab /home/oracc/pcsl/00etc)
(cd /home/oracc/pcsl ; sudo /home/oracc/bin/orc /home/oracc/bin/sx-update.sh)
