%{
#include <string.h>
#include "resolver.h"
/*
 * This list is the keywords that can occur on their own in a
 * QUERY_STRING together with the option they imply
 */
%}
struct urlkeytab;
%%
text, "what"
score, "what"
block, "what"
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
