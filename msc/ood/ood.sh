#!/bin/sh
if [ -d ood ]; then
    cd ood ; for o in * ; do (cd $o ; ood-one.sh ) ; done
fi
