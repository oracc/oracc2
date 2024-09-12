include ../../../oracc-am-rules.txt

SUBDIRS=../../../lib

bin_PROGRAMS=bgmx ngmx lemx postntokx numtokx

bgmx_SOURCES=bgmx.c
bgmx_LDADD=$(xLDADD)

lemx_SOURCES=lemx.c
lemx_LDADD=$(xLDADD)

ngmx_SOURCES=ngmx.c
ngmx_LDADD=$(xLDADD)

postntokx_SOURCES=postntokx.c runexinputs.c
postntokx_LDADD=$(xLDADD)

numtokx_SOURCES=numtokx.c runexinputs.c numword.c
numtokx_LDADD=$(xLDADD)

