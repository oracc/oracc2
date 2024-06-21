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
text, "what"
score, "what"
sources, "what"
page, "part"
full, "part"
tlit, "part"
meta, "part"
xlat, "part"
html, "form"
xml, "form"
tei, "form"
json, "form"
cuneify, "form"
proof, "form"
pdf, "form"
pager, "what"
corpus, "what"
sig, "what"
searchbar, "what"
statistics, "what"
adhoc, "what"
