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
akk, NS_INL , INL_NO , INL_YES , 0 , "gdl", "%akk" , inl_gdl_h
cfy, NS_INL , INL_NO , INL_YES , 0 , "gdl", "cfy"  , inl_gdl_h
sux, NS_INL , INL_NO , INL_YES , 0 , "gdl", "%sux" , inl_gdl_h
cite,NS_BIB , INL_NO , INL_YES , 0 , 0 , 0 , inl_bib_h
nocite,NS_BIB , INL_NO , INL_YES , 0 , 0 , 0 , inl_bib_h
fullcite,NS_BIB , INL_NO , INL_YES , 0 , 0 , 0 , inl_bib_h
br,  NS_INL , INL_NO , INL_NO  , 0 , 0 , 0 , 0
"-", NS_INL , INL_NO , INL_NO , 0 , 0 , 0 , 0
"~", NS_INL , INL_NO , INL_NO , 0 , 0 , 0 , 0
"?", NS_INL , INL_NO , INL_YES , "?@" , 0 , 0 , 0
"\"", NS_INL , INL_NO , INL_YES , "\"@" , 0 , 0 , 0
"@*", NS_INL , INL_NO , INL_NO , 0 , 0 , 0 , 0
"@=", NS_INL , INL_NO , INL_NO , 0 , 0 , 0 , 0
