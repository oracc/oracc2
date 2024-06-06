include ../../../oracc-am-rules.txt
include ../qx-rules.txt

# This is no longer used in Oracc but we build it here pending
# checking on possible dependencies that qlemx might have on qtxtx
# modules
noinst_PROGRAMS=qtxtx

qtxtx_SOURCES = setxtx.c gdlix.c indexed.c node.c parallel.c \
	branch.c grapheme.c intercha.c option.c property.c

qtxtx_LDADD = $(qxLDADD)
qtxtx_DEPENDENCIES = $(qxLDADD)
