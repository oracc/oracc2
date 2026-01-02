	cx -- Oracc II catalogue processor

Phases
======

	0 -- set up 01tmp/00cat with links to 00cat/*.tsv; convert 00cat/*.xml to .tsv
	1 -- build list of IDs that need cat entry
	2 -- identify source of cat data for each ID, including img data, cat.d,
	     designation sort orders, and imported fields from keydata.xml
	3 -- generate single 01bld/cat.tsv with call necessary cat entry data
	4 -- load 01bld/cat.tsv
	5 -- save sortinfo data

