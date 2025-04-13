#!/bin/sh
./nx -x -d ed3bg.dat
xsltproc ns-sl.xsl ns.xml >ns-sl.xml
xsltproc ns-html.xsl  ns-sl.xml >ns.html
sudo cp ns.html /home/oracc/www/ns-test.html
