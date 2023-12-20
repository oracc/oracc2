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

TOKEN-SIGNATURES
================

The data associated with a signature is concatenated into a string
with a fixed sequence of initial elements and delimiters.

A token-signature is always typed--the type is always the first
element of the token-signature and is always a single lowercase
letter. A period, '.', separates the token type from the following
elements.

A token-signature always has an OID-signature. If the OID-signature
begins with 'o' or 'x', the OID has been obtained from the the Oracc
ID management system.  If the OID begins with 'z', it has been
assigned ad hoc during data processing because the token is not known
in the Oracc OID system.

An OID-signature is a sequence of one or more OIDs.  For sg and wd
types, the OID is a singleton.  For fr and sn types, the OID is a
doubleton where the first item is the parent sg or wd, and the second
item is the child fr or sn.  For vl types, the OID is a doubleton
ending with a period in which the second item may be empty.  When the
second item is empty (indicating that the value belongs to a sign not
a form), the vl OID-signature ends with a double period.


Signature for sign 'A':

  s.o0000087

Signature for value 'a' of sign A:

  v.o0000087..:a

Signature for form ZA of sign A:

  f.o0000087.o0000603

Signature for value a written "ZA:, a(ZA):

  v.o0000087.o0000603.:a

A token-signature may include a project, but this is optional.  A
project begins with a '@', as with word-signatures.

A token-signature may include a language or script element.  A
language/script begins with a percent sign, e.g., %sux, %020.

A token-signature may include the token, which begins with a colon; if
the token is empty, the value is by definition the value of its
OID-signature.  If the token-signature ends with a word-signature, the
token is the FORM, and the remaining signature items are separated
from the FORM with an equals-sign, '='.


Location Types:
===============

Type	Data
--------------------------------------
F	filename
T	project text-id text-name
L	line-number line-id line-label
W	word-id
