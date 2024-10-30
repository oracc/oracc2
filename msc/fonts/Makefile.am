include ../../oracc-am-rules.txt

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

*.ofp: ${bindir}/ttf2ofp.sh ${bindir}/ofp2im.sh ${bindir}/ofpx \
	${prefix}/osl/02pub/lists.tsv ${prefix}/osl/02pub/unicode.tsv
