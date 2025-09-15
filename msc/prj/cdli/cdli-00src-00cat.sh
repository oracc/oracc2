#!/bin/sh
head -1 00src/cdli.tsv >00lib/fieldnames.row
tail -n +1 00src/cdli.tsv | grep -v '^	' | grep -v '^$'| tidtextx >00cat/cdli_catalogue.tsv
# | tunquotex
