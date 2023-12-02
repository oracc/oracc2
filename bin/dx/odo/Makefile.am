include ../../../oracc-am-rules.txt

bin_SCRIPTS=odo odo-cvs-checkout.sh odo-cvs-refresh.sh \
	odo-build.sh odo-dir-links.sh odo-list.sh \
	odo-update.sh odo-config.sh odo-environment.sh \
	odo-status.sh odo-signlist.sh

odo: odo.in
	$(ORACCIFY) $<
