#!/bin/sh
perl -p -e "s/\'.*\$//" $1-tok2.lst | perl -p -e 's#//.*?\]#]#' | sort -u >$1-uniq.lst
