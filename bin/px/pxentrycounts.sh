#!/bin/dash
#
# Invoke with glossary directory as argument
#
# Create list of basenames and line counts
#

if [ "$1" = "" ] || [ ! -d $1 ]; then
    echo $0: must give glossary dir on command line. Stop.
    exit 1
else
    (cd $1 ; wc -l [eL]*.lst | grep -v total | tr -s ' ' '\t' \
	 | rocox -C32 | sed 's/.lst//' | sort >entry-counts.tab
     chmod o+r entry-counts.tab)
fi
