%{
#include "string.h"
#include "inl.h"
%}
struct inltok;
%%
b,	 NS_INL , INL_NO , INL_YES , 0 , 0 , 0 , 0
em,  NS_INL , INL_NO , INL_YES , 0 , 0 , 0 , 0
i, 	 NS_INL , INL_NO , INL_YES , 0 , 0 , 0 , 0
r, 	 NS_INL , INL_NO , INL_YES , 0 , 0 , 0 , 0
sub, NS_INL , INL_NO , INL_YES , 0 , 0 , 0 , 0
sup, NS_INL , INL_NO , INL_YES , 0 , 0 , 0 , 0
u, 	 NS_INL , INL_NO , INL_YES , 0 , 0 , 0 , 0
gdl, NS_INL , INL_MAY, INL_YES , 0 , 0 , 0 , inl_gdl_h
akk, NS_INL , INL_NO , INL_YES , 0 , "gdl", "%akk" , 0
cfy, NS_INL , INL_NO , INL_YES , 0 , "gdl", "cfy"  , 0
sux, NS_INL , INL_NO , INL_YES , 0 , "gdl", "%sux" , 0
cite,NS_BIB , INL_NO , INL_YES , 0 , 0 , 0 , inl_bib_h
br,  NS_INL , INL_NO , INL_NO  , 0 , 0 , 0 , 0
"-", NS_INL , INL_NO , INL_TOK , 0 , 0 , 0 , 0
"?", NS_INL , INL_NO , INL_YES , "?@" , 0 , 0 , 0
"\"",NS_INL , INL_NO , INL_YES , "\"@", 0 , 0 , 0
