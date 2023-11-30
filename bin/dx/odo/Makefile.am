include ../../../oracc-am-rules.txt

bin_SCRIPTS=odo odo-build.sh odo-cvs-checkout.sh odo-dir-links.sh odo-list.sh \
	odo-update.sh odo-config.sh odo-cvs-refresh.sh odo-environment.sh \
	odo-status.sh

odo: odo.in
	$(ORACCIFY) $<
