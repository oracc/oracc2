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
