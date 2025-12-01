BIB and BX--Oracc Bibliography Processing

Bib (the short name for Oracc bibliography) is mostly managed by bx.

bx operates in several different modes:


1) Cites Mode (CIT)
===================

Default mode. Scans XML inputs for <cite> elements and uses the found
ref-keys to generate a .bib file. XML inputs are on command line or in
a file named with -c arg, which may be '-' to read XML inputs from stdin.

.bib files are not used in CIT mode.

TODO: would be nice for XML inputs to follow xincludes; or be given as
dir

TODO: give list of directories with -B to look for .bib files in.

TODO: project-level bib using config.xml to give list of .bib
      files or dirs


2) Keys Mode (KEY)
==================

Arg -k, scans .bib files checking for duplicate keys and outputs it to
a file named with -o, or in a conventional location
ORACC/lib/data/bib-key.txt or ORACC/pub/PROJECT/bib-key.txt if the -p
arg is given.

Reports duplicates and the location of first occurences.

.bib files as Cites mode but with -b arg for bib files list.

.xml files are not used in KEY mode.

KEY mode can be used as a subsidiary of ICF mode with the -K arg. In
this case, bib files must be given in a list file and the keys are
first harvested from the files in the list.


3) Refs Mode (REF)
==================

Arg -r, read .xml files, use CIT to get citations; augment for bib needs; gen output

Arg -R, read .bib files.  Default is to validate; with -o write .bib;
    with -s sorted .bib
    with -S sort fields within entries (only used in -R mode)
    *note* -s and -S are separate options; -sS for sorted .bib with sorted fields

Output to XML with -x or HTML with -d (div only) or -h (full html file).

bx refuses to write to an existing file unless -O (overwrite) is given before -[dhx].

Uses popen to pipe through xsltproc bib-HTML.xsl).

Arg -p [PROJECT] is passed to xsltproc; arg -d is implemented as arg
param to xsltproc.

.bib files as Keys mode.


4) Inline Cite Formatting Mode (ICF)
====================================

ICF depends on CIT mode, so it calls CIT and REF then runs itself.

Arg -f, or -F to have no formatted bibliography output.

Reads the same XML inputs as were specified for the CIT
modeand adds a formatted citation as child of <cite>. Any <cite>
children already in the input are discarded and replaced with newly
produced ones.

Outputs to arg -t [TMP] directory or in-place with arg -i.  


5) Bib-ID mode (BID)
====================

Arg -i: BID scans $ORACC/bib/00etc/*.bib to find last used B-ID and
then reads and writes .bib files, adding B-IDs to any entries that
lack them. See Oracc Bibliography IDs for further information.

Arg -I: BID Identity sub-mode: reads and writes .bib files but does no
B-ID handling.  With no -s/-S the output should diff -w clean against
the original; use with -s/-S to sort entries/fields.  Default output
to stdout; use -n for no output.


Oracc Bibliography IDs
======================

Oracc uses Bibliography IDs of the form B123456 to ensure unique and
stable addressing of bibliography entries.  These IDs are stored in
the BiBTeX ids field:

 ids = { B000123 },

To ensure persistency, bibliography items that are determined to be
identical after they have been assigned B-IDs are merged by adding the
B-ID to the ids field of the retained item:

 ids = { B000123 , B001432 },

B-IDs are programmatically assigned in bx's BID mode by running bx -i
[FILE].  Only one file is processed at a time, default output to STDOUT
or the destination named with the -o argument; use -n for no output.

In BID mode bx reads all of the Oracc system .bib files
(/home/oracc/bib/00etc/*.bib) to determine the last used B-ID, then it
reads the input file to assign B-IDs to any entries that lack one.


Bib and OOD (Oracc Open Data)
=============================

Oracc supports searching and listing bibliography entries using the
OOD infrastructure which allows data to be treated as a special form
of Oracc catalogue.

The bib ood uses the bx XML output to create its searchable format and
the bx HTML output for the display data.

The searchable data supports field prefixes id for B-ID, au for
author, and ti for title.  These and all other fields are also merged
into a generic 'keys' field.

The displayed version consists of a formatted version of the entry
along with the lead reference; a button allows the lead reference to
be copied to the clipboard.


TODO
====

In the XML only b:cite is defined; a @type attr needs to be added with
values foot|paren|no to correspond to \footcite, \parencite, and
\nocite

bx needs an option to set b:cite type attribute so it's not hardwired
into documents
