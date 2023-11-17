include ../../oracc-am-rules.txt

bin_SCRIPTS = oscx-server-env.sh oraxx oscx-ls.sh

bin_PROGRAMS=osh

oraxx: oraxx.in
	$(ORACCIFY) $<

# oraxx-exec.plx oscx-build.sh oscx-server-environment.sh \
#	oscx-check.sh oscx-o2.sh oscx-check.plx oscx-run.sh oraxx-exec.sh \
#	oscx-create.sh oscx-unpack.sh oscx-serve.sh oscx-resources.sh \
#	oraxx oscx-pack.sh oscx-proj-from-conf.sh \
#	oscx-fake-server.sh
