include ../../../oracc-am-rules.txt

noinst_SCRIPTS: *.sh

all: pcsl

pcsl:
	@(cd /home/stinney/orc/pcsl ; cp -u fonts/PC-all.ttf 00res/fonts)
	@(cd /home/stinney/orc/pcsl/00res ; sudo cp -uRv * /home/oracc/www/pcsl/)
	@./pcsl-update-index.sh
	@./pcsl-site-update.sh
