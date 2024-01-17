.SUFFIXES:

MK=${ORACC_BUILDS}/lib/data/mcu-lemm.mk

SUPER=01tmp/superglo-for-lemmdata.glo

check:
	@for a in 00lib/*.glo ; \
	  do $(MAKE) -s -f $(MK) 00lib/.c-`basename $$a` ; \
	done

super:
	@for a in 00src/*.glo ; \
	  do $(MAKE) -s -f $(MK) 01tmp/`basename $$a` ; \
	done

lemmgloss: 00lib/*.glo
	l2p1-project-sig.plx
	l2p1-simple-from-project.sh
	l2p1-lemm-data.plx -g -f
	touch lemmgloss

# maybe integrate o2-super-prep.sh actions into this target instead of
# or as well as l2p1-*
lemmsuper: ${SUPER}
	l2p1-project-sig.plx
	l2p1-simple-from-project.sh
	l2p1-lemm-data.plx -g ${SUPER}
	touch lemmsuper

00lib/.c-%.glo: 00lib/%.glo
	@echo ${MK} checking $<
	cbdpp.plx -sig -nopsus $< || touch .badgloss
	touch 00lib/.c-$*.glo
	rm -f lemmgloss

01tmp/%.glo: 00src/%.glo
	@echo ${MK} creating ${SUPER} from $<
	cbdpp.plx -nosigs $< || touch .badgloss
	cp $< ${SUPER}
	rm -f lemmsuper
