include ../../../oracc-am-rules.txt
include ../qx-rules.txt

bin_PROGRAMS=qcbdx

qcbdx_SOURCES = secbdx.c sn_alias.c statnames.c

qcbdx_LDADD = $(qxLDADD) \
	../txt/grapheme.o ../txt/node.o ../txt/parallel.o \
	../txt/indexed.o ../txt/intercha.o ../txt/gdlix.o \
	../txt/option.o ../txt/property.o ../txt/branch.o

qcbdx_DEPENDENCIES = $(qxLDADD)
