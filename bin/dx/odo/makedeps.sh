#!/bin/sh
set 00atf/*.atf
if [ "$1" != "00atf/*.atf" ]; then
    ATFDEPS=$*
fi
set 00cat/*.tsv
if [ "$1" != "00cat/*.tsv" ]; then
    CATDEPS=$*
fi
set 00lib/*.glo
if [ "$1" != "00lib/*.glo" ]; then
    GLODEPS=$*
fi

cat >.deps <<EOF
ATFS=${ATFDEPS}
CATS=${CATDEPS}
GLOS=${GLODEPS}
DEPS=${ATFDEPS} ${CATDEPS} ${GLODEPS}
EOF
