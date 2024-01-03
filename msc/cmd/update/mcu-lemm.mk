MK=$(lastword $(MAKEFILE_LIST))

default: check lemm

check:
	@for a in 00lib/*.glo ; \
	  do $(MAKE) -s -f $(MK) 00lib/.c-`basename $$a` ; \
	done

lemm: 02pub/lemm-*.sig
	@make -s -f $(MK) lemmstamp

lemmstamp: 00lib/*.glo
	l2p1-lemm-data.plx
	touch lemmstamp

00lib/.c-%.glo: 00lib/%.glo
	echo checking $<
	cbdpp.plx -sig -nopsus $<
	touch 00lib/.c-$*.glo

02pub/lemm-%.sig: 00lib/%.glo
	cbdpp.plx -sig -nopsus $<
	rm -f lemmstamp
