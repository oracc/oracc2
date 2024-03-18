include ../../../oracc-am-rules.txt
include ../qx-rules.txt

bin_PROGRAMS=qx

qx_SOURCES = se.c binops.c binops24.c evaluate.c se_pcre.c \
	setoks.c tokenize.c xkeys.c xml.c
qx_LDADD = $(qxLDADD) -lpcre
qx_DEPENDENCIES = $(qxLDADD)
