include ../../oracc-am-rules.txt

noinst_SCRIPTS = dot_bash_profile

skeldata_data = .bash_profile .emacs

dot_bash_profile: .bash_profile.in
	$(ORACCIFY) .bash_profile.in
	touch -f .bash_profile dot_bash_profile

install-data-local:
	mkdir -p $(skeldatadir)
	rm -f $(skeldatadir)/.bash_profile
	cp -a $(skeldata_data) $(skeldatadir)
