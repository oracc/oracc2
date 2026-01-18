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

TODO
====

	designation-sort-codes and friends do no validation so keys not given will just sort weirdly

===========

01tmp/00cat should have local-p, dynamic-p and proxy-p which then get cat *-p | cx -

cat building needs to be split into two phases; local-[pqx] need to be built as part of oracc config vel sim, so that all projects can be configured before building.  This will allow mutual proxying between projects

local-[pqx].tsv get built into 01bld/00cat

NEW 3ff
    3 - generate cat-[pqx].tsv; load each into their own roco; create one sortinfo hash from all of them; export as single XML stream; this needs to track which ones came from proxies

OR:

	cx local-[pqx].tsv dynamic-[pqx].tsv proxy-[pqx].tsv

into one sortinfo but only outputs local and dynamic

==========

0) all projects get local data built into 01bld/00cat

For non-local data:

a1) set fields for P/X: if any P/X in 00cat, use head -1; else use head -1 cdli|pmaster
a2) set fields for Q: if any Q in 00cat, use head -1; else use head -1 qcat|qmaster

b) set source projects for any non-local XMD needed, either dynamic or proxied

c) extract non-local data into 01bld/00cat/{dynamic|proxied}-[pqx].tsv
-- this won't need field-header lines because it will use the same
fields and order as the local data
