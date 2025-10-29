BX--Oracc Bibliography Processing

Quick notes ...

bx -k generates key list from .bib files

.bib can be on command line, or sent in to bx -k via stdin

bx -k generates system ORACC/lib/data/bib-res.ltx, a file of
\addresource{...bib} lines and ORACC/lib/data/bib-key.txt, a file of
legal keys, one per line

bx -p PROJECT -k generates PROJECT/02pub/bib-res.ltx and
PROJECT/02pub/bib-key.txt

bx.sh uses bx to get <b:cite> from XML files and writes a file
containing \nocite{KEY} for each key.

It then calls latex on a wrapper (see bx.sh notes) to create the .bcf
file.

It then calls biber on the .bcf to create the .bbl file   

===Formatting inline cites

Formatting inline cites will probably be added to bx which could add
an attribute to b:cite or add a child with the formatted inline
citation.

===Formatting the reference list

Formatting the bibliography will use tex4ht.

Given a bibliography in mybib.tex

 latex mybib
 biber mybib
 xhlatex mybib

produces XHTML mybib.html and mybib.css

===

In the XML only b:cite is defined; a @type attr needs to be added with
values foot|paren|no to correspond to \footcite, \parencite, and
\nocite

bx needs an option to set b:cite type attribute so it's not hardwired
into documents

