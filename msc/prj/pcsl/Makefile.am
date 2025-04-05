include ../../../oracc-am-rules.txt

bin_SCRIPTS=pcsl-csl-hook.sh

noinst_SCRIPTS=*.sh
scriptdata_DATA=pcsl-csldata.xsl

dir=$(shell pwd)

default: all

# If there are any .sh.in scripts or the like add them as targets of all:
all:
	@echo To build PCSL say \'make pcsl\'

pcsl:
	@(cd /home/stinney/orc/pcsl/00res ; sudo cp -uRv * /home/oracc/pcsl/00res/)
	@(cd /home/oracc/pcsl/00res ; sudo cp -uRv * /home/oracc/www/pcsl/)
	@./pcsl-update-index.sh
	@./pcsl-site-update.sh
