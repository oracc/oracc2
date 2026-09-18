#!/bin/sh
cut -f3,4,6-8 ~/orc/osl/00etc/cdli-sign-osl.tsv | rocox -C43521 >catf-map.tsv
