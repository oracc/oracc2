include ../../../oracc-am-rules.txt

noinst_SCRIPTS: *.sh

dir=$(shell pwd)

default: all

all: osl-prepare-update.sh osl-sx-slix.sh
	@echo To build OSL say \'make osl\'

osl:
	@./osl-font-update.sh
	(cd ../../../lib/ofp ; make)
	(cd ../../../lib ; make)
	(cd ../../../bin/xx/fonts ; make)
	(cd ../../../bin/xx/fonts ; sudo make install)
	(cd ../../../msc/fonts ; make)
	(cd ../../../msc/fonts ; sudo make install)
	@./osl-page-update.sh
	@./osl-site-update.sh
