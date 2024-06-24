include oracc-am-rules.txt

SUBDIRS=lib bin msc

bin_SCRIPTS=oraccenv.sh
noinst_SCRIPTS=*.sh oracc.sed

oracc.sed: oraccsed.sh
	./oraccsed.sh
