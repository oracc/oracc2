#!/bin/dash
echo $0 $*
o2-lst.sh
lx-lists.sh
o2-cat.sh
if [ -r 01bld/cdlicat.xmd ]; then
    project=`oraccopt`
    mkdir -p 02pub/cat
    echo qindex qcatx $project 01bld/cdlicat.xmd
    qcatx -p $project -s <01bld/cdlicat.xmd
    sudo chmod -R go+r 02pub/cat
fi
