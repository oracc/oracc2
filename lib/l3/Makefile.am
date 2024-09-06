
SUBDIRS=bigrams ilem link ngram prop psus \
	sigs word xcl2 dsa psa xli

noinst_LTLIBRARIES=libl3.la

libl3_la_DEPENDENCIES=sigs/libsigs.la word/libwords.la xcl2/libxcl2.la \
	prop/libprops.la ilem/libilem.la \
	link/liblinks.la psus/libpsus.la bigrams/libbigrams.la \
	ngram/libngram.la xli/libxli.la \
	dsa/libdsa.la psa/libpsa.la psus/libpsus.la

libl3_la_LIBADD=sigs/libsigs.la word/libwords.la xcl2/libxcl2.la \
	prop/libprops.la ilem/libilem.la \
	link/liblinks.la psus/libpsus.la bigrams/libbigrams.la \
	ngram/libngram.la xli/libxli.la \
	dsa/libdsa.la psa/libpsa.la psus/libpsus.la
