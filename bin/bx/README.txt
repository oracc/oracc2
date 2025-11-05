BX--Oracc Bibliography Processing

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


3) Refs Mode (REF)
==================

Arg -r, read .xml files, use CIT to get citations; augment for bib needs; gen output

Arg -R, read .bib files.  Default is to validate; with output, sort as well

Output to XML with -x or HTML with -d (div only) or -h (full html file).

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


TODO
====

In the XML only b:cite is defined; a @type attr needs to be added with
values foot|paren|no to correspond to \footcite, \parencite, and
\nocite

bx needs an option to set b:cite type attribute so it's not hardwired
into documents
