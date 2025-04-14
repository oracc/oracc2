#!/bin/sh
if [ "$ORACC_MODE" = "single" ]; then
    cd $ORACC/bin; ln -sf odo oracc
fi
