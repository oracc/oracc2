#!/bin/sh
./demo.plx >index.html
demodir=${ORACC}/www/osl/OraccCuneiformFonts/Cuneify/demo/
$ORACC_sudo mkdir -p $demodir
d=`pwd`
$ORACC_sudo cp $d/index.html $d/demo.css $d/t*.html $demodir
$ORACC_sudo chmod -R o+r $demodir
