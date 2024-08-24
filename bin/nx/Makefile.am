include ../../oracc-am-rules.txt

bin_PROGRAMS=nx
bin_SCRIPTS=
scriptdata_DATA=

nx_SOURCES=nx.c ns.tab.c ns.lex.c ns_build.c nxt.c nx_parse.c nxp_numbers.c \
	nxr_print.c nx_data.c

nx_LDADD = $(xLDADD)
nx_DEPENDENCIES = $(nx_LDADD)

ns.tab.c ns.tab.h: ns.y ns.l

ns.lex.c: ns.l ns.tab.h
