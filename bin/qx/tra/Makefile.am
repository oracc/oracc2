include ../../../oracc-am-rules.txt
include ../qx-rules.txt

bin_PROGRAMS=qtrax

qtrax_SOURCES = setrax.c

qtrax_LDADD = \
	$(qxLDADD) \
	../txt/grapheme.o ../txt/node.o ../txt/parallel.o \
	../txt/indexed.o ../txt/intercha.o \
	../txt/option.o ../txt/property.o ../txt/branch.o

qtrax_DEPENDENCIES = $(qxLDADD)
