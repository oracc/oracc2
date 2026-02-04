#!/bin/sh
bin=${ORACC}/bin
cut -d@ -f1 02pub/atf-data.tab | ${bin}/pqxpand -r tok >02pub/tokfiles.lst
mkdir -p 02pub/tokl
cat 02pub/tokfiles.lst | ${bin}/tok2tisl.sh >02pub/tokl.kis

##at present we process graphemes for corpus sign list; in future we
##may do other stuff with grapheme toks

##cat 02pub/tokfiles.lst | ${bin}/tok2tisg.sh >02pub/tokg.tpc
