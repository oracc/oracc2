include ../../../oracc-am-rules.txt

bin_SCRIPTS=odo odo-cvs-checkout.sh odo-cvs-refresh.sh \
	odo-dir-links.sh odo-environment.sh \
	odo-list.sh odo-status.sh

odo: odo.in
	$(ORACCIFY) $<
