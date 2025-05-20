include ../../../oracc-am-rules.txt

bin_SCRIPTS=neo-proxies.sh neo-rebuild.sh neo-sigs.sh neo-oids.sh neo-oids-sub.sh \
	neo-prepare.sh

dir=$(shell pwd)

default: all

# If there are any .sh.in scripts or the like add them as targets of all:
all:
	@echo To build NEO say \'make neo\'

neo:
	(cd ${ORACC}/neo ; neo-rebuild.sh)
