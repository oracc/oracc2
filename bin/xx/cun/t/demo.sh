#!/bin/sh
./demo.plx >demo.html
demodir=${ORACC}/www/osl/cuneify/demo
$ORACC_sudo mkdir -p $demodir
d=`pwd`
$ORACC_sudo cp $d/demo.html $d/demo.css $d/[0-9]*.html ${ORACC}/www/osl/cuneify/demo/
$ORACC_sudo chmod -R o+r $demodir
