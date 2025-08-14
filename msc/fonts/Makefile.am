include ../../oracc-am-rules.txt

include Makefile.fnt

css_DATA=fonts.css *.cur

DEF=	Oracc-Noto-SansCuneiform.def Oracc-gudea.def Oracc-NA-Outline.def \
	Oracc-OBF-Regular.def CuneiformClassic-final.def Oracc-LAK.def \
	Oracc-RSP.def PCSL.def

OFP=	Oracc-Noto-SansCuneiform.ofp Oracc-gudea.ofp Oracc-NA-Outline.ofp \
	Oracc-OBF-Regular.ofp CuneiformClassic-final.ofp Oracc-LAK.ofp \
	Oracc-RSP.ofp PCSL.ofp

OIN=	Oracc-Noto-SansCuneiform.oin Oracc-gudea.oin Oracc-NA-Outline.oin \
	Oracc-OBF-Regular.oin CuneiformClassic-final.oin Oracc-LAK.oin \
	Oracc-RSP.oin PCSL.oin

.PRECIOUS: ${OIN}

data_DATA=${OFP} ${DEF} \
	im-ofs-cc.tab im-ofs-gudea.tab im-ofs-noto.tab im-ofs-oobf.tab \
	im-ofs-lak.tab im-ofs-nao.tab im-ofs-rsp.tab im-ofs-pc.tab

font_DATA=Oracc-Noto-SansCuneiform.ttf Oracc-gudea.ttf Oracc-NA-Outline.ttf \
	Oracc-OBF-Regular.ttf CuneiformClassic-final.ttf Oracc-LAK.ttf \
	Oracc-RSP.ttf Oracc-UET2.ttf PCSL.ttf

default: sl/lists.tsv ${OFP} ${DEF}

#install-data-local:
#	chmod o+r *.lig
#	cp -a *.lig $(datadir)
