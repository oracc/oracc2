include ../../../oracc-am-rules.txt

noinst_SCRIPTS: *.sh

dir=$(shell pwd)

default: all

all: osl-prereqs.sh osl-prepare-update.sh osl-sx-slix.sh osl-user-page.sh osl-user-site.sh
	@echo To build OSL say \'make osl\'

osl:
	@./osl-prereqs.sh
	(cd ../../../lib/ofp ; make)
	(cd ../../../lib ; make)
	(cd ../../../bin/xx/fonts ; make)
	(cd ../../../bin/xx/fonts ; $(ORACC_sudo) make install)
	(cd ../../../msc/fonts ; make)
	(cd ../../../msc/fonts ; $(ORACC_sudo_E) make install)
	@./osl-page-update.sh
	@./osl-site-update.sh
