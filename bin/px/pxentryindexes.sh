#!/bin/dash
#
# Invoke with glossary directory as argument
#
# Create table of basename:wordid\tindex
#
if [ "$1" = "" ] || [ ! -d $1 ]; then
    echo $0: must give glossary dir on command line. Stop.
    exit 1
else
    (cd $1 ; 
     for a in entry_ids.lst L*.lst ; do \
	 cat -n $a | sed "s/ \+/$a	/g" ;
     done | sed 's/.lst//' >tmp.tab
     cat tmp.tab | rocox -R '%3:%1	%2' -n >entry-indexes.tab
     cat tmp.tab | rocox -R '%1:%2	%3' -n >>entry-indexes.tab
     rm -f tmp.tab
     dbix -d . -n entry-indexes entry-indexes.tab
     chmod o+r entry-indexes.db[hi]
     )
fi
