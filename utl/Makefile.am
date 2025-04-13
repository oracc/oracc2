include ../oracc-am-rules.txt

bin_SCRIPTS = oraccenv.sh trang xl
noinst_SCRIPTS = oraccsed.sh
java_DATA = trang.jar

all-local: ../oracc.sed

../oracc.sed: oraccsed.sh
	./oraccsed.sh
