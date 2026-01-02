#!/bin/sh
#
# Marshall inputs for .tsv in 01tmp/00cat
#
# For 00cat/*.tsv link to 01tmp/00cat/*.tsv
#
# For 00cat/*.xml convert 00cat/*.xml to 01tmp/00cat/*.tsv
#
echo $0 $*
bin="$ORACC/bin"
policy=`oraccopt . catalog-build-policy`

function auto {
    
}

function custom {
}

function mixed {
}

function virtual {
}

function unknown {
    echo $0: unknown catalog-build-policy '$policy'. Stop.
    exit 1
}

case $policy in
    auto)	auto ;;
    custom)	custom ;;
    local)	local ;;
    mixed)	mixed ;;
    virtual)	virtual ;;
    *)	unknown;;
esac

