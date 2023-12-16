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

g	OID	grapheme
l	OID	lemma
