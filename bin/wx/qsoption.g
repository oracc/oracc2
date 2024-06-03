%{
#include <string.h>
#include "resolver.h"
/*
 * This gives the names of options that can occur in the QUERY_STRING and a key for the allowed values.
 */
%}
struct qsoptiontab;
%%
what, "text|score|sources|block"
part, "page|full|tlit|meta|xlat"
form, "html|xml|tei|json"
lang, "#LANG"
line, "#LREFS"
page, "#NUM"
zoom, "#NUM|#LET|entry_ids"
refs, "#REFS"
sort, "#PERM"
q, 	  "#STRING"
