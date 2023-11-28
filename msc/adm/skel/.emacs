(setq load-path (append load-path 
			'(concat (getenv "ORACC") "/lib/emacs")))
(load-file "oracc-init.el")
