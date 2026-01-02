	cx -- Oracc II catalogue processor

Phases
======

	0 -- Marshall cx-marshall.sh
	     - set up 01tmp/00cat with copy of 00cat/*.tsv
	     - convert 00cat/*.xml to .tsv
	     - force rotation of id_text/id_composite to col 0 unless ^id_
	1 -- build list of IDs that need cat entry and source of base cat data
	     - catalog-build-list => list entries
	     - -or- catalog-build-policy custom|local => cat entries
	     - -or- catalog-build-policy auto|virtual => build cat from ATF
	     - -or- catalog-build-policy mixed => use local/custom supplemented by dynacat
	     Some of this work was previously done by lx-inputs.sh/lx-lists.sh and xmd-ids.plx.
	2 -- identify additional cat data for each ID, including img data, cat.d,
	     designation sort orders, and imported fields from keydata.xml
	3 -- generate single 01bld/cat.tsv with call necessary cat entry data
	4 -- load 01bld/cat.tsv
	5 -- save sortinfo data
