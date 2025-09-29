# Makefile for cfy test suite

T=t01-a.html t02-ne.html t03-sajgal2.html \
	t04-f-default.html t05-f-spread.html t06-f-penult.html \
	t07-f-sa4.html t08-f-return.html t09-f-sa4r.html t10-d+en.html \
	t11-f-cspread.html t12-f-cpenult.html \
	t13-r-ruled.html t14-r-ruled.html t15-r-boxed.html t16-r-column.html

.PRECIOUS: %.xtf

default: t

t: ${T}
	./demo.sh

%.xtf: %.atf
	oxx $< >$@

%.html: %.xtf
	cfy -Hdemo.css $< >$@

clean:
	rm -f *.xtf *.html
