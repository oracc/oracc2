include ../../oracc-am-rules.txt

bin_PROGRAMS=ax atfdatax atfinstall

bin_SCRIPTS=atfmake.sh atftoks.sh

data_DATA=Makefile.atf

ax_SOURCES=ax.c ax_check.c ax_parse.c ax_atf.c ax_jox.c axjoxfnc.c
ax_LDADD = $(xLDADD)
ax_DEPENDENCIES = $(ax_LDADD)

atfdatax_SOURCES=atfdata.c atfdata.lex.c
atfdatax_LDADD = $(xLDADD)
atfdatax_DEPENDENCIES = $(atfdatax_xLDADD)

atfdata.lex.c: atfdata.l

atfinstall_SOURCES=atfinstall.c
atfinstall_LDADD = $(xLDADD)
atfinstall_DEPENDENCIES = $(atfinstall_xLDADD)

