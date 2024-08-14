include ../../../oracc-am-rules.txt
include ../qx-rules.txt

bin_PROGRAMS=qx

qx_SOURCES = all.proto se.c se_run.c se_any.c binops.c binops24.c	\
	evaluate.c se_pcre.c setoks.c tokenize.c xkeys.c		\
	cgi_decode_url.c qx.c qx_options.c se_results.c qx_error.c	\
	se_debug.c px.c cgi_options.c cgiopts.c xml.c

# px.c 

qx_LDADD = $(qxLDADD) -lpcre ../../px/px_mesg.o
qx_DEPENDENCIES = $(qxLDADD)
