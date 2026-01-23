%{
#include <string.h>
#include "p4url.h"
/*
 * This list is the keywords that can occur on their own in a
 * QUERY_STRING together with the option they imply
 */
%}
struct urlkeytab;
%%
adhoc, "what"
block, "form"
Wblock, "block"
corpus, "what"
cuneify, "form"
Wcuneify, "text"
full, "part"
html, "form"
Whtml, "text"
json, "form"
Wjson, "text"
meta, "part"
page, "part"
pager, "what"
pdf, "form"
Wpdf, "text"
proofing, "form"
Wproofing, "text"
score, "what"
Wscore, "text"
searchbar, "what"
sig, "what"
sources, "form"
Wsources, "score"
statistics, "what"
tei, "form"
Wtei, "text"
text, "what"
tlit, "part"
xlat, "part"
xml, "form"
Wxml, "text"
