include ../../oracc-am-rules.txt

SUBDIRS=skel

sbin_SCRIPTS = oadm oadmsudo oadmwrapper

oadm: oadm.in
	$(ORACCIFY) oadm.in

oadmsudo: oadmsudo.in
	$(ORACCIFY) oadmsudo.in

oadmwrapper: oadmwrapper.in
	$(ORACCIFY) oadmwrapper.in
