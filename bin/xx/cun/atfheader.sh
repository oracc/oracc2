#!/bin/sh
atf=$1
if [[ "$atf" == "" ]]; then
    echo $0: must give ATF file. Stop.
    exit 1
fi
lang=$2
unicode=$3
char=`head -c 1 $atf`
if [[ $char != "&" ]]; then
    if [[ "$lang" == "sux" ]]; then
	proj="epsd2"
    else
	proj="cams"
    fi
    if [[ "$unicode" != "" ]]; then
	unicode=`atfuni $atf`
    fi
    cat <<EOF
&X010101=Cuneify Data
#project: $proj
#atf: use math
$unicode
EOF
    atfnum check <$atf
    if [ $? != 0 ]; then
	atfnum <$atf
    else
	cat $atf
    fi
else
    cat $atf
fi
