TOK -- new Oracc data collection subsystem
******************************************

Oracc2 uses token lists to gather instances.  Token lists are
tab-separated files in which each line consists of a type column
followed by one or more columns of data.  Types are single characters:
uppercase characters are location types, lowercase characters are data
types.


TOKEN DEFINITION
================

A token is an indexable element of data which is specified using a
version of Oracc's 'signatures'.  A signature contains all of the
information associated with a word-instance; token-signatures use a
superset of word-signatures with prepended type and OID elements.

PRIMARY AND SECONDARY TOKENS
----------------------------

Primary tokens are major data items: signs, forms, values; words,
senses; catalog values that are covered by control lists such as
period, id_text.

Secondary tokens are additional items in word-signatures--base, cont,
morph1, morph2, norm; free text tokens in catalogue fields.

PRIMARY TOKEN DATA TYPES
========================

s sign
f form
v value
w word
m sense (meaning)

For sg, fr, and vl types, the language/script '%020' is the default if
omitted from the signature.

SIGN TOKENS
===========

A sign-token is always typed--the type is always the first
element of the token-signature and is always a single lowercase
letter. A period, '.', separates the token type from the following
elements.

A sign-token always has an OID-signature. If the OID-signature
begins with 'o' or 'x', the OID has been obtained from the the Oracc
ID management system.  If the OID begins with 'z', it has been
assigned ad hoc during data processing because the token is not known
in the Oracc OID system.

An OID-signature is a sequence of one or more OIDs: a sign-token
deriving from a form has a doubleton OID consisting of the parent sign
OID and the form OID.

A value sign-token ends with the value, separated from the rest of the
sign-token with a colon, ':'.

Sign-token for sign 'A':

  s.o0000087

Sign-token for value 'a' of sign A:

  v.o0000087.:a

Sign-token for form ZA of sign A:

  f.o0000087.o0000603

Sign-token for value a written "ZA", a(LAK797):

  v.o0000087.o0270203:a

SIGN SIGNATURES
===============

Sign signatures are analogous to word-instance signatures.

A sign signature has the form:

 @PROJECT%LANGUAGE:SIGN-FORM-VALUE=ROLE

The SIGN-FORM-VALUE is always a triple, with empty components as
necessary. The SIGN-FORM-VALUE for an A-sign is A--.

The signature up to the '=' is the same as a word-instance signature.
The component after the '=' is the sign role:

 IDE ideogram
 SYL syllable
 LOG logogram
 SEM semantic determinative
 PRO pronunciation determinative
 SUR surrogate
 PUN punctuation

For SF_LOG, the logogram language follows SF_LOG separated by a right
apostrophe, "'"--this mimics the POS'EPOS syntax of word-instance
signatures.

 LUGAL ->

 @dcclt%akk:LUGAL--=LOG'sux

 UTU{+ši} ->

 @dcclt%akk:UD--UTU=LOG'sux
 @dcclt%akk:UD--ši=PRO

Location Types:
===============

Type	Data
--------------------------------------
F	filename
T	project text-id text-name
L	line-number line-id line-label
W	word-id

SECONDARY TOKENS:
=================

Secondary tokens are expanded from the primary token list in a
separate phase of processing and have the form:

 PRIMARYTYPE.SECONDARYTYPE SECONDARYOID.KEY XDATA

The primary ID and primary KEY are inherited from the last primary token.

For secondary tokens combined using '-' the data for the secondary
rolls up into the data for the primary.

For secondary tokens combined using '.' the data for the secondary is
a subset of the data for the primary.

For grapheme tokens which have a tripartite signature, S.F.V,
word-related secondary tokens from the GSIG are generated for S, S.F,
and S.F.V.  Sign-related secondary tokens--compound constituents--are
generated for either S or F but not both: if F is present, secondary
token generation is determined only by F's status as a compound.

For a compound sign |A.A| with OID o0000656 there is a token list entry:

 g  	o0000656..	[GSIG]

This has secondary tokens

 g.c	o0000087-o0000656	A	[GSIG]
 g.c	o0000087-o0000656	A	[GSIG]

The SECONDARYOID is itself compound to provide the context for the secondary token.

LEMMA TOKENS:
=============

Lemmata signatures have a WORD ID as the primary token

 l	o0012345	LSIG

The SENSE is a secondary token:

 l.s	o0012345.o002342	CGSPE_SIG (CF[GW/SENSE]POS'EPOS)

Secondary tokens from the signature are secondary to the SENSE token not to the WORD:

 l.s.m	o0012345.o002342#m=~;a

The secondary OID and KEY are combined and uniqueness is ensured by
embedding the type into the string suffix of the OID.

