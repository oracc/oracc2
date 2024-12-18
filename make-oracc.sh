#!/bin/sh
u=$ORACC_USER
if [ "$u" == "yes" ]; then
    sudo="sudo"
fi
(cd lib ; make ; $sudo make install)
(cd bin ; make ; $sudo make install)
(cd msc ; make ; $sudo make install)
