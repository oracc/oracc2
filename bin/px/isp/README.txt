ISP (Instance Set Pager) ARCHITECTURE
=====================================

Introduction
------------

The ISP is a successor to elements of P3 and the XIS resolver and
generalizes these previous components as well as completing the
URL-based API of the XIS resolver.

Background
----------

Oracc's token architecture consists of three parts:

 	1. Token extraction and expansion (tok)
	2. Token Instance Set (tis) compilation
	3. The Instance Set Pager subsystem (isp)

ISP works both with tis outputs and with any other list of Oracc list
of glossary-article, text, line, word, or grapheme IDs--collectively
we call this suite of IDs content IDs, or CIDs.

Lists of glossary-articles and text IDs are also called Record Lists (RL),
whereas line, word and grapheme ID lists are called Instance Sets
(IS).  Record Lists are a simpler subset of Instance Sets, and not all
of the parameters and processes are needed when paging RLs.

In the following, we say 'LIST' when we mean a name that identifies a
list of CIDs--this list could be a temporary list identified by a
tmpdir such as p3.fzCvhE, an XIS name such as akk.r00001, a TIS name
such as t123456, or the name of a project-build or user-specified list
such as 'sansatf' or 'emar'.  Collectively we call this suite of IDs
list IDs, or LSIDs.

ISP Parameters
--------------

ISP operates with the following context parameters:

 PROJECT -- the project owning the LSIDS

 LIST -- an LSID providing a set of CIDs

 META -- a specification of metadata fields for sorting and section
         headers; these can be set separately for two pager display
         modes: default and special.  The default fields are
         time/place/genre; maximum number of fields is 3. The config
         file sets the fields for zero or more of the default/special
         modes.
 
 MODE -- the current default/special pager mode

 PERM -- the permutation of metadata fields set by the outline
      	 selector for the current mode. The fields for MODE are
      	 obtained from the config and numbered 1..3 in the order given
      	 in the config option.  A PERM is a 1 to 3 digit code with
      	 each digit in the range 1..3 and no digit occurring more than
      	 once.  Thus, in the default the PERM would be 123; after
      	 selectiong genre/place/time the PERM would be 321, etc.

 ZOOM -- an integer which is the line number of LIST that contains a
      	 slice of the LIST selected by sorting and grouping according
      	 to META, MODE and PERM

 ZMAX -- the maximum allowable value of ZOOM

 PAGE -- a page of the current ZOOM slice that is sent for display

 PSIZ -- the number of instances per page

 PMAX -- the maximum allowable value of PAGE at the current PSIZ

 ITEM -- the item from the current zoomed list that is displayed in the content pane

 PACK -- the packaging method; ASIS or ESP2

 XHMD -- the text mode; OXML (Oracc XML) or HTML

 UIMD -- the UI mode for the display; MAXI is the traditional P3
      	 display; MINI is the reduced version utilized in instance
      	 popups

 CEMD -- the context engine mode for rendering instances -- CCAT, CCBD; implying CXTF: UNIT, LINE, KWIC
 
 LANG -- the language for translations accompanying instances

 AAPI -- the A tag API: REST=construct URLs for server resolution;
      	 FILE=construct PATHS for client-based static file reading

 HOST -- the host page for the ISP container, used with PACK

For RLs from glossaries
***********************

The following parameter equivalents obtain:
 
 GLOS = LIST -- the glossary is considered the total list of available articles

 LETR = ZOOM -- a letter in the glossary is a zoomable subset of the total list

 ARTE = ITEM -- a glossary article is the unit of item display in the content pane

For RLs from catalogues
***********************

The following parameter equivalents obtain:

 _all = LIST -- the special token '_all' has historically been used by P3
      	     	to mean all catalogue entries

 RECO = ITEM -- catalogue lists have defaulted to linking to text ITEMs;
      	     	should the new P3 modify this? Maybe a project config item?

ISP Sequence of Actions
-----------------------

ISP does its work in the following sequence of steps:

 0. CC--Cache Check
 1. LA--List acquisition
 2. LS--List sorting and grouping
 3. ZS--Zoom slicing
 4. PE--Page extraction
 5. WM--Word mapping
 6. CE--Context extraction
 7. OF--Output formatting
 8. OP--Output packaging

ISP Toolchain
-------------

ISP is implemented in a collection of library routines with a related
suite of front-end programs meaning that processing can be done as a
typical filter or by building the libraries in to client programs.

 0. ispcc -- determines the path to the cached output
 1. ispla -- determines list location based on LSID; loads CIDs from
    	     stdin or from list
 2. ispls -- sorts/groups CIDs
 3. ispzs -- excerpts zoom slice from ispls result
 4. isppe -- excerpts page slice from ispzs result
 5. ispwm -- translates word ids into context ranges
 6. ispce -- extracts context units from XML
 7. ispof -- translates output to HTML if necessary
 8. ispop -- packages output if necessary

ISP Options
-----------

ISP options can be used to control which actions are performed and to set parameters.

Uppercase options set actions:

 0. -E CC--Cache Check
 1. -L LA--List acquisition
 2. -S LS--List sorting and grouping
 3. -Z ZS--Zoom slicing
 4. -P PE--Page extraction
 5. -W WM--Word mapping
 6. -C CE--Context extraction
 7. -F OF--Output formatting
 8. -O OP--Output packaging

Lowercase options set parameters

 -j project
 -l list
 -z zoom number
 -p page number
 -s page size
 -k pack
 -h host page
 -m oxml/html mode
 -u ui mode
 -c context mode
 -n lang
 -a aapi
