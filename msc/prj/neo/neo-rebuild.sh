#!/bin/sh
set -x
neo-dir-links.sh
neo-proxies.sh
neo-sigs.sh
neo-oids.sh
neo-prepare.sh

l2p1-simple-from-project.sh
l2p2.sh
o2-web.sh
l2p3.sh 01bld/www

#run all the indexing for neo
qindex.sh neo

for a in 02pub/cbd/* ; do
    lang=`basename $a`
    if [ -r 01bld/$lang/letter_ids.tab ]; then
	cp 01bld/$lang/letter_ids.tab $a
	cp 01bld/$lang/L*.lst $a
    fi
done
o2-weblive.sh
sudo o2-prm.sh
neo-cbd-json.sh
sudo chown -R root:oracc /home/oracc/www/neo/downloads
sudo chmod g+r /home/oracc/www/neo/downloads
mkdir -p 02www/lists
cp 01bld/lists/outlined.lst >02www/lists/outlined.lst
chmod -R o+r 02www/lists

pxp3p4.sh neo
pxreset.sh neo

docker restart oracc-ingest
