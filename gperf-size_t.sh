#!/bin/sh
gperf -G -W "${1}_wl" -N $1 -tT $2 | \
    sed 's/register unsigned int len/register size_t len/g' >$3
