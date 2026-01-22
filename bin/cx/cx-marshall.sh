#!/bin/sh
#
# Marshall inputs from 00cat in 01tmp/00cat/local-[pqx].tsv.  This
# script is designed so that all projects in a new Oracc installation
# can run cx-marshall.sh before any actual catalogue building is done.
#
# For 00cat/*.tsv cp to 01tmp/00cat/l/*.tsv.
#
# For 00cat/*.xml convert 00cat/*.xml to 01tmp/00cat/l/*.tsv.
#
# Then rotate all .tsv to ensure ID is at start and split into
# separate files.
#
#set -x
set -a
echo $0 $*
projtype=`oraccopt . type`

bin="$ORACC/bin"
bdir=01bld/cat
cdir=01tmp/00cat
ldir=01tmp/00cat/l

# clean out old cat data
rm -fr $$bdir $cdir $ldir ; mkdir -p $bdir $cdir $ldir
if [ ! -d $bdir ]; then
    echo $0: failed to make $bdir. Stop.
    exit 1
fi
if [ ! -d $cdir ]; then
    echo $0: failed to make cdir. Stop.
    exit 1
fi
if [ ! -d $ldir ]; then
    echo $0: failed to make $ldir. Stop.
    exit 1
fi

# copy .tsv data because we might reorder fields
set 00cat/*.tsv
if [ "$1" != "00cat/*.tsv" ]; then
    for t in $* ; do
	cp $t $ldir
    done
fi

# convert XML to TSV
set 00cat/*.xml
if [ "$1" != "00cat/*.xml" ]; then
    for x in $* ; do
	t=`basename $x .xml`.tsv 
	${bin}/xml2tsv $t >$ldir/$t
    done
fi

# rotate id_text/id_composite; OOD data is not touched by this script.
set $ldir/*.tsv
if [ "$1" != "$ldir/*.tsv" ]; then
    for t in $* ; do
	id=`head -1 $t | grep ^id_`
	if [ "${id}" = "" ]; then
	    id=`head -1 $t | tr '\t' '\n' | grep ^id_ | tr -d '\n'`
	    ${bin}/rocox -z $id $t >$t.z
	    mv $t.z $t
	fi
    done
fi

# ensure basic requirements are met
${bin}/cx-sanity.sh
if [ $? -ne 0 ]; then
    echo $0: failed catalague sanity checks. Stop.
    exit 1
fi

# set up 01bld/cat/local-[pqx].tsv
${bin}/cx-local.sh

# process 00lib/cat.d
${bin}/cx-extra.sh

# join local and cat.d
${bin}/cx-all.sh

# create 01bld/00cat.lst
${bin}/cx-00cat-lst.sh

# must do this after creating 00cat.lst
${bin}/cx-xcat.sh

# Now 01bld/cat contains local-[pqx].tsv based on catalogue data
# actually in the project
