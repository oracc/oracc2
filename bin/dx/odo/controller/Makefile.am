include ../../../../oracc-am-rules.txt

bin_SCRIPTS=odo odo-func.sh

noinst_SCRIPTS=oracc-odo.sh oracc

data_DATA=*.mk

odo: odo.in
	$(ORACCIFY) $<

install-exec-local:
	. ./oracc-odo.sh
