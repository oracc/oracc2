include ../../oracc-am-rules.txt

bin_PROGRAMS=lex-collapse-refs

bin_SCRIPTS = lex-sign-data.sh lex-sign-xtf.sh lex-word-data.sh		\
	lex-word-get-data.sh lex-word-xtf.sh lex-word-base.sh		\
	lex-word-qakk.sh lex-word-phra.sh lex-word-equi.sh		\
	lex-master.sh lex-data-tis.sh lex-phra-base.sh lex-tis.sh	\
	lex-word-tail.sh lex-provides.sh

scriptdata_DATA = lex-sign-data.xsl lex-sign-group.xsl			\
	lex-sign-lookup.xsl lex-sign-HTML.xsl lex-word-data.xsl		\
	lex-data-line.xsl lex-word-base-group.xsl			\
	lex-word-base-HTML.xsl lex-word-phra-pull.xsl			\
	lex-word-phra-group.xsl lex-word-phra-sort.xsl			\
	lex-word-phra-HTML.xsl lex-refs-group.xsl lex-sort.xsl		\
	lex-data-xis-ids.xsl lex-data-xis-remap.xsl lex-data-xis.xsl	\
	lex-pull-base.xsl lex-phra-base-group.xsl lex-sign-tis.xsl	\
	lex-phra-base-HTML.xsl lex-provides.xsl lex-split-HTML.xsl	\
	lex-pull-tail.xsl lex-phra-tail-group.xsl lex-merge-phra.xsl	\
	lex-word-equi-HTML.xsl lex-sign-ref-codes.xsl			\
	lex-sign-ref-sort.xsl lex-sign-sortkeys.xsl lex-provides-xi.xsl	\
	lex-act-isid.xsl lex-group@refs.xsl

lex_collapse_refs_SOURCES=lex-collapse-refs.c
lex_collapse_refs_LDADD = $(xLDADD)
lex_collapse_refs_DEPENDENCIES = $(xLDADD)
