include ../../../oracc-am-rules.txt

noinst_SCRIPTS: *.sh

dir=$(shell pwd)

default: all

# If there are an .sh.in scripts or the like add them as targets of all:
all:
	@echo To build PCSL say \'make pcsl\'

pcsl:
	@(cd /home/stinney/orc/pcsl ; cp -u fonts/PC-all.ttf 00res/fonts)
	@(cd /home/stinney/orc/pcsl/00res ; sudo cp -uRv * /home/oracc/pcsl/00res/)
	@(cd /home/oracc/pcsl/00res ; sudo cp -uRv * /home/oracc/www/pcsl/)
	@./pcsl-update-index.sh
	@./pcsl-site-update.sh
