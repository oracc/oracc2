include ../../oracc-am-rules.txt

SUBDIRS=../../lib sxweb

bin_PROGRAMS=sx slix slx csldist

bin_SCRIPTS=\
	sxcmp.sh sxnorm.sh sxclean.sh \
	sxmissing.sh sxudata.sh sxportal.sh \
	sxportal-listdata.sh sxportal-udata.sh sxoid.sh \
	sximages.sh sx-add-oid.sh sxportal-abbrevs.sh sx-csl.sh \
	sxfreq.sh sxsortorder.sh sx-slix.sh sx-update.sh sx-tok.sh

# osl-update.sh pcsl-update.sh 

data_DATA=sx-list-*.xml sx-udata-*.xml sx-udata-codes.tsv Makefile.csl

scriptdata_DATA=*.xsl

slx_SOURCES=slx.c
slx_LDADD = $(xLDADD) 
slx_DEPENDENCIES = $(xLDADD)

sx_SOURCES=sx.c sx_marshall.c sx_homophones.c sx_compounds.c sx_digests.c \
	sx_inherited.c sx_listdefs.c sx_values.c sx_qualified.c sx_unicode.c \
	sx_walk.c sx_sll.c sx_w_asl.c sx_oid_list.c sx_config.c sx_script.c \
	sx_images.c sx_syss_dump.c sx_akas_dump.c sx_sortcodes.c sx_w_jox.c \
	sx_extras.c sx_idata.c sx_merge.c sx_numbers.c noset.c sx_forms.c
sx_LDADD = $(xLDADD) -lpcre2-8
sx_DEPENDENCIES = $(xLDADD)

slix_SOURCES=slix.c
slix_LDADD = $(xLDADD)
slix_DEPENDENCIES = $(xLDADD)

csldist_SOURCES=csldist.c
csldist_LDADD = $(xLDADD) 
csldist_DEPENDENCIES = $(xLDADD)
