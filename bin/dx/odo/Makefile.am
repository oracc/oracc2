include ../../../oracc-am-rules.txt

bin_SCRIPTS=odo odo-cvs-checkout.sh odo-cvs-refresh.sh \
	odo-dir-links.sh odo-list.sh \
	odo-update.sh odo-config.sh odo-environment.sh \
	odo-status.sh odo-check.sh odo-check-glo.sh odo-check-atf.sh \
	odo-check-asl.sh odo-lexdata.sh odo-portal.sh \
	odo-build.sh odo-build-default.sh \
	odo-catalog.sh odo-corpus.sh odo-signlist.sh \
	odo-gloss.sh odo-gloss-xml.sh odo-gloss-htm.sh odo-gloss-web.sh \
	odo-gloss-htm-wrapup.sh odo-gloss-web-wrapup.sh \
	odo-chown.sh odo-chmod.sh odo-sudo.sh

noinst_SCRIPTS=oracc-single.sh

odo: odo.in
	$(ORACCIFY) $<

install-exec-local:
	. ./oracc-single.sh
