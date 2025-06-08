#!/bin/sh
if [ "$ORACC_MODE" = "single" ]; then
    cd $ORACC/bin; ln -sf odo oracc
else
    rm -f $ORACC/bin/oracc
    cp -v oracc $ORACC/bin
fi
