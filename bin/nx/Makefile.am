include ../../oracc-am-rules.txt

bin_PROGRAMS=nx nxtokx
bin_SCRIPTS=
scriptdata_DATA=
data_DATA=ns.dat

nx_SOURCES=nx.c ns.tab.c ns.lex.c ns_data.c ns_build.c nxt.c nx_parse.c nxp_numbers.c \
	nxr_print.c nx_debug.c nxr_testdata.c nx_modern.c nx_math.c \
	nx_values.c nx_sys_aevs.c nx_input.c nx_exec.c gc.c gw.c na.c ne.c nc.c

nx_LDADD = $(xLDADD)
nx_DEPENDENCIES = $(nx_LDADD)

nxtokx_SOURCES=nxtokx.c nwmap.c
nxtokx_LDADD = $(xLDADD)
nxtokx_DEPENDENCIES = $(nx_LDADD)

ns.tab.c ns.tab.h: ns.y ns.l

ns.lex.c: ns.l ns.tab.h

nxt.g: nxt.base nxt.sh
	./nxt.sh

gc.g: gur-ngm.tsv gc.sh
	./gc.sh
