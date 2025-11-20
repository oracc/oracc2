This directory contains scripts to retrieve CDLI data using the
cdli-framework API and process it for use by Oracc.

To rebuild the CDLI metadata:

* retrieve the md from CDLI using ./cdli-cat.sh which saves to cdli.csv
  -- this is normally done on dev-oracc right now because the lack of
  other use of that machine doesn't interrupt the download

* mv cdli.csv to ${ORACC}/cdli/00src ; 'make cdli' works from here on
  but the separate steps are:

  * convert the .csv to .tsv using ./cdli-csv2tsv.sh

    make ${ORACC}/cdli/00src/cdli.tsv

  * massage the .tsv for use as the Oracc CDLI catalogue

    make ${ORACC}/cdli/00cat/cdli_catalogue.tsv

  * update CDLI catalogue installation

    make cdli
