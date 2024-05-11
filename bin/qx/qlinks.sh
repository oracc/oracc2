#!/bin/sh
for a in secatx secbdx segdfx selemx setrax setxtx ; do
    mv $a x$a
done
for a in cat cbd gdf lem tra txt; do
    ln -sf q${a}x se${a}x
done
#mv ce2 xce2
#mv ce_xtf xce_xtf
ln -sf cex ce2
ln -sf cextfx ce_xtf
if [ -r sedbg ]; then
    mv sedbg xsedbg
fi
ln -sf qdbgx sedbg
