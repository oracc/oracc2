This directory contains scripts to retrieve CDLI data using the
cdli-framework API and process it for use by Oracc.

To rebuild the CDLI metadata:

* retrieve the md from CDLI using ./cat.sh -- this is normally done on
  dev-oracc right now because the lack of other use of that machine
  doesn't interrupt the download

* convert the .csv to .tsv using ./csv2tsv.sh

* copy cdli.tsv to build-oracc:/home/oracc/cdli/00src/cdli.tsv

* run 'make cdli' in this directory
