# toksigs

The toksigs utility reads .tok files and writes the signature data in
tabbed format.  Each signature row begins with file, line, and
instance information, so the signatures can be tracked back to their
occurrence in the source ATF file.  The PROJECT and WORDID are also
given which means that URLs to the highlighted word can also be
generated.

The field order and numbers for argument to the 'cut' utility are:

     1	FILE
     2	LINE
     3	INST
     4	PROJ
     5	WORDID
     6	FORM
     7	LANG
     8	CF
     9	GW
    10	SENSE
    11	POS
    12	EPOS
    13	NORM
    14	BASE
    15	CONT
    16	STEM
    17	MORPH
    18	MORPH2

## Sumerian Sentence Boundaries

Sumerian sentence boundaries can normally be identified as finite
verbal forms with no subordination marking.  To insert them into the
source ATF files using the ATF '+.' notation, the FILE/LINE/INST
information can be used.

Thus, a pipeline that is identifying and inserting sentence boundaries
might begin:

      toksigs [DOT_TOK_FILE] | cut -f 1-3,12,17 | [IDENTIFY_SENTENCE_BOUNDARIES]

The program that identifies sentence boundaries could then write outputs of the form:

    FILE	LINE	INST

The LINE is the main transliteration line of an ATF block; the program
that inserts the '+.' would first navigate to LINE, then to the next
'#lem:' line, then search for the INST and add ' +.' before the next
';' or EOL.
