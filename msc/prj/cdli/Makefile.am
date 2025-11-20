include ../../../oracc-am-rules.txt

bin_PROGRAMS=cdliperiodx cdlixtfx imgp4

bin_SCRIPTS = cdli-field-tab.sh cdli-period+.sh cdli-period-lists.sh \
	cdli-csv2tsv.sh cdli-00src-00cat.sh cdli-img2oracc.sh

noinst_SCRIPTS = cdli-cat.sh cdli-img.sh

cdliperiodx_SOURCES=cdliperiodx.c periods.c
cdliperiodx_LDADD = $(xLDADD)
cdliperiodx_DEPENDENCIES = $(xLDADD)

cdlixtfx_SOURCES=cdlixtfx.c cdli2xtf.lex.c xmlify.c
cdlixtfx_LDADD = $(xLDADD)
cdlixtfx_DEPENDENCIES = $(xLDADD)

imgp4_SOURCES=imgp4.c 
imgp4_LDADD = $(xLDADD)
imgp4_DEPENDENCIES = $(xLDADD)

cdli=${ORACC}/cdli

${cdli}/00src/cdli.tsv: ${cdli}/00src/cdli.csv
	(cd ${cdli} ; cdli-csv2tsv.sh)

${cdli}/00cat/cdli_catalogue.tsv: ${cdli}/00src/cdli.tsv
	(cd ${cdli} ; cdli-00src-00cat.sh)

cdli: ${cdli}/00cat/cdli_catalogue.tsv
	rm -f nohup.out ; nohup time ./cdli-build.sh &
