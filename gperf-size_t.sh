#!/bin/sh
gperf -G -W "${1}_wl" -N $1 -tT $2 | \
    grep -v 'register const char \*str' | \
    grep -v 'register unsigned int len' | \
    sed 's/^hash (str, len)/hash (register const char *str, size_t len)/g' | \
    sed "s/^$1 (str, len)/$1 (register const char *str, size_t len)/g" >$3

#gperf -G -W "${1}_wl" -N $1 -tT $2 | \
#    sed 's/register unsigned int len/register size_t len/g' >$3
