#!/bin/sh
bin=${ORACC}/bin
lst=01bld/lists/atfinstall.lst
tok=02pub/tok.lst

# Update the .tok files for the project's installed ATF
cat $lst | tokx -p `oraccopt` -lgt

# Make a list of all ATF including proxies
cut -f1 02pub/atf-data.tab | cut -d@ -f1 | pqxpand tok >$tok

# These token lists contain all the project data including proxied
# data
mkdir -p 02pub/tokl
cat $tok | ${bin}/tok2tisl.sh >02pub/tokl.tpc

mkdir -p 02pub/tokg
cat $tok | ${bin}/tok2tisg.sh >02pub/tokg.tpc
