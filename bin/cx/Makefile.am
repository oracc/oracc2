include ../../oracc-am-rules.txt

bin_PROGRAMS=cx

bin_SCRIPTS=cx.sh cx-marshall.sh cx-xmd-ids.sh cx-sanity.sh cx-local.sh cx-xcat.sh \
	cx-extra.sh cx-all.sh cx-policy.sh cx-outer.sh cx-clean.sh cx-fields.sh

scriptdata_DATA=*.xsl

cx_SOURCES=cx.c cx_roco.c cx_keydata.c keydata.c cx_sortinfo.c \
	cx_remap.c cx_sortcodes.c cx_merper.c cx_replace.c

cx_LDADD = $(xLDADD)
cx_DEPENDENCIES = $(cx_LDADD)
