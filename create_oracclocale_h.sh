#!/bin/sh
mkdir -p lib/include
ORACC_LOCALE=`locale -a | grep ^en | grep -i us | grep -i utf | head -1`
cat >include/oracclocale.h <<EOF
#define ORACC_LOCALE "$ORACC_LOCALE"
EOF

