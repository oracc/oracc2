#!/bin/sh
java -jar ../../utl/trang.jar $1 $2
for a in *.rng ; do
    mkdir -p x
    rm -f x/*
    sed 's#<include#<xi:include xmlns:xi="http://www.w3.org/2001/XInclude" #g' <$a >x/$a
    mv x/* .
    rmdir x
done
