#
# For all targets, remove .dep and pass to oracc2.mk
#
# ePSD2 projects should
#
#	ln -sf ${ORACC}/lib/data/oracc1.mk Makefile

# It seems like a lot to send all errors to /dev/null here but it's
# the only way I can see to turn off the error that .deps does not
# exist, which is on purpose because we rebuild .deps via include
# .deps in oracc2.mk
project check: epsd2lemm
	@rm -f .deps
	@$(MAKE) -f ${ORACC}/lib/data/oracc2.mk $@ 2>/dev/null

epsd2lemm:
	(cd ${ORACC}/epsd2 ; make lemm)
