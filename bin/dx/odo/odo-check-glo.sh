#!/bin/dash

echo $0 $*

mcu-lemm.sh

if [ -r .badgloss ]; then
    exit 1
else
    echo $0: all glossaries check OK.
    exit 0
fi
