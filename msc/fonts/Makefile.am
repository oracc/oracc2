include ../../oracc-am-rules.txt

fonts:
	(cd ../../lib/ofp ; make)
	(cd ../../lib ; make)
	(cd ../../bin/xx/fonts ; make)
	(cd ../../bin/xx/fonts ; sudo make install)
	make all

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
