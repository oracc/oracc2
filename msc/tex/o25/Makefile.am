include ../../../oracc-am-rules.txt

bin_SCRIPTS = mk-oracctex.sh oracctex texmanager.plx
texdata_DATA = cdlrandom.tex chartest.tex cuneify.tex facinglines.tex gdl.tex \
	newmcol.tex pagecont.tex rinap.tex tet-mac.tex xtf.tex fonts25.tex \
	cdf/*.tex cdf/*.ini cdf/graphics/*.tex
scriptdata_DATA = csl-TeX.xsl cuneify-TeX.xsl glo-TeX.xsl TeX-util.xsl
data_DATA = chartest.xml

oracctex: oracctex.in
	$(ORACCIFY) oracctex.in

mk-oracctex.sh: mk-oracctex.sh.in
	$(ORACCIFY) mk-oracctex.sh.in
