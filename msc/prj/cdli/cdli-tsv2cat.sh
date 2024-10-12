#!/bin/sh
tunquotex <00src/cdli.tsv | grep -v '^	' | tidtextx >00cat/cdli_catalogue.tsv
