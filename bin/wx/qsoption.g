%{
#include <string.h>
#include "p4url.h"
/*
 * This gives the names of options that can occur in the QUERY_STRING and a key for the allowed values.
 */
%}
struct qsoptionstab;
%%
what, "text|score|sources|block|sig|pager|corpus|statistics|searchbar|adhoc|file"
part, "page|full|tlit|meta|xlat"
form, "html|xml|tei|json"
lang, "#LANG"
line, "#LREFS"
page, "#NUM"
zoom, "#NUM|#LET|entry_ids"
bkmk, "#NUM|#REFS"
refs, "#REFS"
sort, "#PERM"
q, 	  "#STRING"
srch, "#LIST"
cemd, "kwic|line|unit"
frag, "#NCNAME"
%%
int qs_total_keywords(void)
{
    return TOTAL_KEYWORDS + 11; /* 11 is a magic number that == the number of 'what' types */
}
