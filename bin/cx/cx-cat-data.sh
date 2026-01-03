#!/bin/sh
#
# Use project lists to figure out which cat data should come from
# project and which can be drawn from other projects.
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

