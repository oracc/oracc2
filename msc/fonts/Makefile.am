include ../../oracc-am-rules.txt

include Makefile.fnt

css_DATA=fonts.css *.cur

FONTS=Oracc-Noto-SansCuneiform.ttf Oracc-gudea.ttf Oracc-NA-Outline.ttf \
	Oracc-OBF-Regular.ttf CuneiformClassic-final.ttf Oracc-LAK.ttf \
	Oracc-RSP.ttf Oracc-UET2.ttf PCSL.ttf Nabuninuaihsus.ttf Oracc-EMC.ttf

DEF=	Oracc-Noto-SansCuneiform.def Oracc-gudea.def Oracc-NA-Outline.def \
	Oracc-OBF-Regular.def CuneiformClassic-final.def Oracc-LAK.def \
	Oracc-RSP.def PCSL.def Nabuninuaihsus.def Oracc-EMC.def

LIG=	Oracc-Noto-SansCuneiform.lig Oracc-gudea.lig Oracc-NA-Outline.lig \
	Oracc-OBF-Regular.lig CuneiformClassic-final.lig Oracc-LAK.lig \
	Oracc-RSP.lig PCSL.lig Nabuninuaihsus.lig Oracc-EMC.lig

OFP=	Oracc-Noto-SansCuneiform.ofp Oracc-gudea.ofp Oracc-NA-Outline.ofp \
	Oracc-OBF-Regular.ofp CuneiformClassic-final.ofp Oracc-LAK.ofp \
	Oracc-RSP.ofp PCSL.ofp Nabuninuaihsus.ofp Oracc-EMC.ofp

OIN=	Oracc-Noto-SansCuneiform.oin Oracc-gudea.oin Oracc-NA-Outline.oin \
	Oracc-OBF-Regular.oin CuneiformClassic-final.oin Oracc-LAK.oin \
	Oracc-RSP.oin PCSL.oin Nabuninuaihsus.oin Oracc-EMC.oin

UNI=	Oracc-Noto-SansCuneiform.uni Oracc-gudea.uni Oracc-NA-Outline.uni \
	Oracc-OBF-Regular.uni CuneiformClassic-final.uni Oracc-LAK.uni \
	Oracc-RSP.uni PCSL.uni Nabuninuaihsus.uni Oracc-EMC.uni

.PRECIOUS: ${OIN}

data_DATA=${OFP} ${DEF} ${LIG} ${UNI} \
	im-ofs-cc.tab im-ofs-gudea.tab im-ofs-noto.tab im-ofs-oobf.tab \
	im-ofs-lak.tab im-ofs-nao.tab im-ofs-rsp.tab im-ofs-pc.tab

font_DATA=${FONTS}

default: sl/lists.tsv ${FONTS} ${OFP} ${DEF} ${OIN} ${LIG} ${UNI}
	true

Nabuninuaihsus:
	./nni.sh

nni-pull: Nabuninuaihsus
	(cd $< ; git pull)

Nabuninuaihsus/Nabuninuaihsus.ttf: nni-pull

Nabuninuaihsus.ttf: Nabuninuaihsus/Nabuninuaihsus.ttf
	cp Nabuninuaihsus/Nabuninuaihsus.ttf $@

Oracc-EMC.ttf: ~/santak/fonts/emc/Oracc-EMC.ttf
	cp $< $@

#install-data-local:
#	chmod o+r *.lig
#	cp -a *.lig $(datadir)
