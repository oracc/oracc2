#!/bin/sh
# Identify signs that are in 'need' (usually a corpus) that are not in
# the given font (usually the font used to cuneify a corpus)
#
fntsigns.sh $1 >have
if [ -s have ]; then
    cut -f1 need | sort -u | nofakes.sh >need.1
    cut -f1 have | sort -u | nofakes.sh >have.1
    comm -23 need.1 have.1
fi
