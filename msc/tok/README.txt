TOK -- new Oracc data collection subsystem
******************************************

Oracc2 uses token lists to gather instances.  Token lists are
tab-separated files in which each line consists of a type column
followed by one or more columns of data.  Types are single characters:
uppercase characters are location types, lowercase characters are data
types.

Location Types:
===============

Type	Data
--------------------------------------
F	filename
T	project text-id text-name
L	line-number line-id line-label
W	word-id


Data Types:
===========

TOP	TID	TNM	SUB	SID	SNM	DTYPE	DATUM

TOP is g(rapheme) or l(emma); future types will include,
e.g., p(hrase), m(easure), d(ate), n(ame), e(ntity).

TID is the Oracc OID of TOP; TNM is TOP name

SUB is the subtype of the token's parent--form, sense

SID is the OID of SUB; SNM is the SUB name

DTYPE is the type of the data item--this could be a TOP, like sign
or entry; a SUB, like form or sense; or a minor type like value, form,
or morph.

DATUM is the value of the token--a sign name, sign value, lemma, base,
sense, etc.

The initially generated versions of token files may have empty fields
or items in the wrong location; these are post-processed to align with
the schema.

GRAPHEME TOKS
=============

Grapheme TOKS, g-lines, are initially output with TID, DTYPE, and
DATUM. In cases like girim₃(|A.BU|), the TID is actually the variant
form |A.BU|. Postprocessing determines the parent sign for
girim₃(|A.BU|) and rewrites the line to have the sign-ID in TID, the
sign-name in TNM, the form-ID (which was in TID) is moved to SID and
the FORM name is added to SNM.

TOK2TIS
=======

Inverting token lists to create Text-Instance-Sets (.tis) files is
simple: the TOK data must be rewritten as:

TYPE DATUM OID WORD-ID

TYPE may have greate

The reason for including DATUM is that it allows multiple levels of
granularity: the OID provides an overview of all primary data--signs,
lemmata--the DATUM allows values, meaning, forms, 
