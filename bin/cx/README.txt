	cx -- Oracc II catalogue processor

Phases
======

	0 -- Marshall cx-marshall.sh
	     - set up 01tmp/00cat with copy of 00cat/*.tsv
	     - convert 00cat/*.xml to .tsv
	     - force rotation of id_text/id_composite to col 0 unless ^id_
	0'-- Generate set of lists with odo-lists.sh
	1 -- build list of IDs that need cat entry and source of base cat data
	     - catalog-build-list => list entries
	     - -or- catalog-build-policy custom|local => cat entries
	     - -or- catalog-build-policy auto|virtual => build cat from ATF
	     - -or- catalog-build-policy mixed => use local/custom supplemented by dynacat
	     Some of this work was previously done by lx-inputs.sh/lx-lists.sh and xmd-ids.plx.
	2 -- extra -- marshall additional cat data for each ID, including cat.d,
	     and imported fields from keydata.xml; CDLI img data now in cdli/02pub/img.tsv
	3 -- generate single 01bld/cat.tsv with all necessary cat entry data
	4 -- load 01bld/cat.tsv
	5 -- save sortinfo data
