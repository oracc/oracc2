#!/bin/dash
. ${ORACC}/bin/odo-func.sh
odo_odo $0 $*

##
## 1) NON-XML SOURCES
##

# create 01bld/LANG/xtf.tpc for each lang in 02pub/ltok.tpc
siglangx -lst >.tlangs

# create 01bld/LANG/glo.sig for each lang in 00lib/*.glo
glosigx -gl >.glangs

# create a union list of langs in corpus and glossaries
echo qpn >>.tlangs
sort -u .tlangs .glangs >.langs

# create 01bld/LANG/LANG.glo for each lang in tpc including qpn-x-*
langs=`cat .langs`
for l in $langs; do
    d=01bld/$l
    if [ -r $d/xtf.tpc ]; then
	base=$d/xtf.tpc
    elif [ -r $d/glo.sig ]; then
	base=$d/glo.sig
    else
	echo $0: skipping language $l because no data.
    fi
    if [ "$base" != "" ]; then
	odov $0: sigglox -p $odo_proj -o 01bld/$l/$l.glo $base
	sigglox -p $odo_proj -o 01bld/$l/$l.glo $base
    fi
done

##
## 2) XML GLOSSARY
##

# create 01bld/LANG/LANG.cbd for each lang in 00lib/*.glo excluding
# qpn-x-*
langs=`grep -v ^qpn-x .langs`
glo_ok=1
for l in $langs; do
    if [ -r 00lib/$l.glo ]; then
	#	odov $0: gx -o 01bld/$l/$l.cbd $g
	#	gx -o 01bld/$l/$l.cbd 00lib/$l.glo
	if [ $(gx -v 00lib/$l.glo) ]; then
	    glo_ok=0
	fi
    fi
done

if [ "$glo_ok" = "0" ]; then
    echo $0: errors validating 00lib/*.glo. Stop.
    exit 1
fi

#
#if we created qpn.cbd then generate qpn-x-*.cbd from it
#

# Now we have marshalled all the pieces we have to take account of the
# config options that control how they are used--see the loop below
# for cbd-mode meanings:
cbd_mode=`oraccopt . cbd-mode`
if [ "$cbd_mode" = "" ]; then
    cbd_mode=mixed
fi

# If cbd-with-all = yes the output glossary includes articles with no
# attestations. The default is to suppress unattested entries.
cbd_with=`oraccopt . cbd-with-all`
if [ "$cbd_with"="yes" ]; then
    with=-w
fi

# create 01bld/LANG/articles.xml for each lang in corpus/glossaries,
# excluding qpn-x-*
for l in $langs; do
    lib=00lib
    bld=01bld/$l
    # If we have keys (01bld/LANG/*.glo) and a 00lib/*.glo
    # (01bld/LANG/*.cbd), the priorities depend on cbd-mode > $cbd_mode:
    #
    # For mode=dynamic, ignore .cbd and only use .glo
    # For mode=static, use .cbd and get stats from keys
    # For mode=mixed, use .cbd and merge .glo into it, then add keys to merger

    if [ "$cbd_mode" = "dynamic" ]; then
	base=$bld/$l.glo
    elif [ "$cbd_mode" = "static" ]; then
	if [ -r $lib/$l.glo ]; then
	    base=$lib/$l.glo
	else
	    base=
	fi
    elif [ "$cbd_mode" = "mixed" ]; then
	if [ -r $lib/$l.glo ]; then
	    base=$lib/$l.cbd
	    merge=$$bld/$l.glo
	else
	    base=$bld/$l.glo
	    merge=
	fi
    else
	echo $0: unknown option for cbd-mode. Stop.
	exit 1
    fi
    if [ "$base" = "" ]; then
	echo $0: skipping articles.xml for $l: mode=static but no 00lib/$l.glo
    else
	odov $0: gx -x -o $d/articles.xml $merge $keys $with $base
	gx -x -o $d/articles.xml $merge $keys $with $base
    fi
done
