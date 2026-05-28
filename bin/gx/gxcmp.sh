#!/bin/sh
#
# Compare @entry/@sense in two glossary to see where +/- is needed.
#
#set -x

ga=$1
gb=$2

if [ "$ga" = "" ] || [ "$gb" = "" ]; then
    echo $0: must give two glossaries to compare on command line. Stop.
    exit 1
fi

mkdir -p 01tmp
pat='^\(@proj\|@lang\|@name\|[-+]*@entry\|[-+]*@sense\|@why\|@end[ ]\+entry\|@end[ ]\+senses\)'
grep "$pat" $ga >01tmp/xa.glo
grep "$pat" $gb >01tmp/xb.glo

gx -C 01tmp/xa.glo 01tmp/xb.glo
