%{
#include <oraccsys.h>
#include <tok.h>
%}
struct tokflags;
%%
"g:queried", "?"
"g:remarked", "!"
"g:collated", "*"
erased, "r"
excised, "e"
implied, "i"
maybe, "m"
ok, "k",
some, "v"
supplied, "s"
damaged, "."
missing, "-"
present, "+"
