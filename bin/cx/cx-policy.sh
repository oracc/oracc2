#!/bin/sh
#
# Handle the various allowed values for catalog-build-policy. This is
# the script that triggers catalogue building.
#
# Use project lists to figure out which cat data should come from
# project and which can be drawn from other projects.
#

echo $0 $*
bin="$ORACC/bin"
lxd=01bld/lists
policy=`oraccopt . catalog-build-policy`
approved=$lxd/approved.lst
incat=$lxd/cat-ids.lst
nocat=$lxd/no-cat-data.lst

# set up 01bld/cat/fields.[pqx]
cx-fields.sh

# no-cat-data.lst is a list of approved IDs that are not in project 00cat
${bin}/lx -p `oraccopt` -q $approved - $incat >$nocat

case $policy in
    auto|virtual)
	${bin}/cx-outer.sh $approved
	;;
    custom)
	# in custom mode all data must be maintained by the project
	if [ -s $nocat ]; then
	    echo "$0: custom catalog should contain all items. Missing data for:"
	    cat $nocat
	    ${bin}/cx-outer.sh $nocat
	fi
	;;
    local)
	if [ -s $nocat ]; then
	    echo "$0: local catalog items to be supplied from fallback cat:"
	    cat $nocat
	    ${bin}/cx-outer.sh $nocat
	fi
    ;;
    mixed)
	if [ -s $nocat ]; then
	    ${bin}/cx-outer.sh $nocat
	fi
    ;;
    *)
	echo $0: unknown catalog-build-policy '$policy'. Stop.
	exit 1
	;;
esac
