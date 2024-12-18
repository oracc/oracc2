include ../../../oracc-am-rules.txt

noinst_SCRIPTS: *.sh

dir=$(shell pwd)

default: all

all:
	@echo make in ${dir} only works with \'make osl\'

pcsl:
	@(cd /home/stinney/orc/pcsl ; cp -u fonts/PC-all.ttf 00res/fonts)
	@(cd /home/stinney/orc/pcsl/00res ; sudo cp -uRv * /home/oracc/pcsl/00res/)
	@(cd /home/oracc/pcsl/00res ; sudo cp -uRv * /home/oracc/www/pcsl/)
	@./pcsl-update-index.sh
	@./pcsl-site-update.sh
