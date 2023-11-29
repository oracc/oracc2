#!/bin/dash
#
# mcu-lemm checks the glossary files and updates the 02pub/*-lemm.sig
# lemmatization data.

echo $0 $*

mcu-lemm.sh
if [ -r 01log/glo.err ]; then
    for a in `cat 01log/glo.err` ; do
	cat $a
    done
fi
