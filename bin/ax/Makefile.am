include ../../oracc-am-rules.txt

bin_PROGRAMS=ax atfdatax atfinstall

bin_SCRIPTS=atfmake.sh atftoks.sh

data_DATA=Makefile.atf

ax_SOURCES=ax.c ax_atf.c ax_run.c ax_prj.c ax_xcl.c ax_lem.c ax_jox.c axjoxfnc.c
ax_LDADD = $(xLDADD)
ax_DEPENDENCIES = $(ax_LDADD)

atfdatax_SOURCES=atfdata.c atfdata.lex.c atfidentity.lex.c atf_cdli.c
atfdatax_LDADD = $(xLDADD)
atfdatax_DEPENDENCIES = $(atfdatax_xLDADD)

atfdata.lex.c: atfdata.l

atfidentity.lex.c: atfidentity.l

atfinstall_SOURCES=atfinstall.c
atfinstall_LDADD = $(xLDADD)
atfinstall_DEPENDENCIES = $(atfinstall_xLDADD)

