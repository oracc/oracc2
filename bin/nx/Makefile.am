include ../../oracc-am-rules.txt

bin_PROGRAMS=nx
bin_SCRIPTS=
scriptdata_DATA=

nx_SOURCES=ns.tab.c ns.lex.c nx.c

nx_LDADD = $(xLDADD)
nx_DEPENDENCIES = $(nx_LDADD)

ns.tab.c ns.tab.h: ns.y ns.l

ns.lex.c: ns.l ns.tab.h
