include ../../oracc-am-rules.txt

bin_PROGRAMS=showttfx

bin_SCRIPTS=ttf2ofp.sh ttf2oin.sh

css_DATA=fonts.css

data_DATA=Oracc-Noto-SansCuneiform.ofp Oracc-gudea.ofp	\
	Oracc-OBF-Regular.ofp CuneiformClassic-final.ofp \
	Oracc-Noto-SansCuneiform.ofpx Oracc-gudea.ofpx	\
	Oracc-OBF-Regular.ofpx CuneiformClassic-final.ofpx \
	Oracc-Noto-SansCuneiform.def Oracc-gudea.def	\
	Oracc-OBF-Regular.def CuneiformClassic-final.def \
	im-ofs-cc.tab im-ofs-gudea.tab im-ofs-noto.tab im-ofs-oobf.tab

font_DATA=Oracc-Noto-SansCuneiform.ttf Oracc-gudea.ttf	\
	Oracc-OBF-Regular.ttf CuneiformClassic-final.ttf

showttfx_SOURCES = showttf.c
showttfx_LDADD = $(xLDADD)
showttfx_DEPENDENCIES = $(xLDADD)

noinst_LTLIBRARIES=libofp.la

libofp_la_SOURCES=ofp.c ofp_ingest.c ofp_marshall.c \
	ofp_dump.c ofp_xml.c ofp_debug.c osl_unicode.c
