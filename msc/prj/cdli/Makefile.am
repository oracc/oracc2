include ../../../oracc-am-rules.txt

bin_PROGRAMS=cdliperiodx cdlixtfx imgp4

bin_SCRIPTS =cdli-tsv2cat.sh cdli-field-tab.sh cdli-period+.sh cdli-period-lists.sh \
	cdli-imgs-export.sh

cdliperiodx_SOURCES=cdliperiodx.c periods.c
cdliperiodx_LDADD = $(xLDADD)
cdliperiodx_DEPENDENCIES = $(xLDADD)

cdlixtfx_SOURCES=cdlixtfx.c cdli2xtf.lex.c xmlify.c
cdlixtfx_LDADD = $(xLDADD)
cdlixtfx_DEPENDENCIES = $(xLDADD)

imgp4_SOURCES=imgp4.c 
imgp4_LDADD = $(xLDADD)
imgp4_DEPENDENCIES = $(xLDADD)

cdli:
	rm -f nohup.out ; nohup time ./cdli-build.sh &
