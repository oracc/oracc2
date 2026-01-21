include ../../oracc-am-rules.txt

bin_PROGRAMS=ax atfdatax linknormx

ax_SOURCES=ax.c ax_check.c ax_parse.c
ax_LDADD = $(xLDADD)
ax_DEPENDENCIES = $(ax_LDADD)

atfdatax_SOURCES=atfdata.c atfdata.lex.c atflink.lex.c
atfdatax_LDADD = $(xLDADD)
atfdatax_DEPENDENCIES = $(atfdatax_xLDADD)

atfdata.lex.c: atfdata.l

atflink.lex.c: atflink.l

linknormx_SOURCES=linknorm.c
linknormx_LDADD = $(xLDADD)
linknormx_DEPENDENCIES = $(linknormx_LDADD)
