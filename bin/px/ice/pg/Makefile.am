include ../../../../oracc-am-rules.txt

AM_LDFLAGS=${LD_GDBM}

bin_PROGRAMS=pgcsix pgux showcsix ptoqtestx pdx pgx xisperiodsx

pgcsix_SOURCES=pgcsi.c sortinfo.c
pgcsix_LDADD = $(xLDADD)
pgcsix_DEPENDENCIES = $(xLDADD)

pgx_SOURCES=pg2.c sortinfo.c sort.c redblack.c sicache.c \
	debug.c page.c outline.c ptoq.c p2.c
pgx_LDADD = $(xLDADD) -lgdbm
pgx_DEPENDENCIES = $(xLDADD)

pdx_SOURCES=pd.c sortinfo.c sort.c redblack.c sicache.c \
	debug.c
pdx_LDADD = $(xLDADD)
pdx_DEPENDENCIES = $(xLDADD)

pgux_SOURCES=pgu.c
pgux_LDADD = $(xLDADD)
pgux_DEPENDENCIES = $(xLDADD)

ptoqtestx_SOURCES=ptoqtest.c ptoq.c
ptoqtestx_LDADD = $(xLDADD) -lgdbm
ptoqtestx_DEPENDENCIES = $(xLDADD)

showcsix_SOURCES=showcsi.c sortinfo.c
showcsix_LDADD = $(xLDADD)
showcsix_DEPENDENCIES = $(xLDADD)

xisperiodsx_SOURCES=xisperiods.c sortinfo.c sort.c redblack.c sicache.c \
	debug.c periodmap.c
xisperiodsx_LDADD = $(xLDADD)
xisperiodsx_DEPENDENCIES = $(xLDADD)
