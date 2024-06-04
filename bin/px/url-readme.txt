P4 URLs
=======

TEXTS
*****

Base: PROJECT/TEXTID

Query:

	part=page [full plus tlit meta xlat]
	form=html [xml tei json pdf cuneify proof]
	what=text [score sources]
	lang=LANG

part: page = pager; full = meta+tlit+xlat; plus=tlit+xlat; tlit/xlat/meta=just one of those

Default is pager display,

The new /PROJECT/TEXTID is equivalent of part=page what=text form=html

The old /PROJECT/TEXTID is now /PROJECT/TEXTID?full, i.e., all content, but no pager context

The old /PROJECT/TEXTID/html is now /PROJECT/TEXTID?html -- implies part=plus; ?xml implies part=tlit

The old /PROJECT/TEXTID/html/LANG is now /PROJECT/TEXTID?html&lang=LANG

lang defaults to UA HTTP_ACCEPT_LANGUAGES followed by Oracc default
     sequence en de fr hu ar fa ckb vel sim

How to handle score blocks? Have a line= param and use what=block or ...?


GLOSSARIES
**********

part=page (or EMPTY)

Base PROJECT/LANGUAGE

Entry PROJECT/LANGUAGE/OXID

Auto-glossary PROJECT/OXID

?html , ?xml [?json] give just glossary or glossary article, no pager scaffolding


LISTS
*****

Base PROJECT?list=LISTNAME


KEYS
****

/KEYS is deprecated effective immediately

?KEYS will continue to work with the QUERY_STRING option names as stop-words which will require the use of:

      ?q=KEYS

For queries.

## RECHECK THESE FROM DOCO ##


PORTING:
========

 PROJECT/TEXTID/html or xml => PROJECT/TEXTID?html or ?xml

 PROJECT/TEXTID/html/hun => PROJECT/TEXTID?html&lang=hun

