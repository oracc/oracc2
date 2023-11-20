include ../../../oracc-am-rules.txt

scriptdata_DATA=*.xsl

bin_SCRIPTS=odo odo-cvs-checkout.sh odo-cvs-refresh.sh \
	odo-dir-links.sh odo-environment.sh

odo: odo.in
	$(ORACCIFY) $<
