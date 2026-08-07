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
fsux, NS_INL , INL_NO , INL_YES , 0 , 0 , "sux" , inl_etcsl_h
gap, NS_INL , INL_NO , INL_YES , 0 , 0 , 0 , inl_etcsl_h
note, NS_INL , INL_NO , INL_YES , 0 , 0 , 0 , inl_etcsl_h
noteanchor, NS_INL , INL_NO , INL_YES , 0 , 0 , 0 , inl_etcsl_h
notegap, NS_INL , INL_NO , INL_YES , 0 , 0 , 0 , inl_etcsl_h
notep, NS_INL , INL_NO , INL_YES , 0 , 0 , 0 , inl_etcsl_h
noteref, NS_INL , INL_NO , INL_YES , 0 , 0 , 0 , inl_etcsl_h
q, NS_INL , INL_MAY , INL_YES , 0 , 0 , 0 , inl_etcsl_h
wDN, NS_INL , INL_NO , INL_YES , 0 , 0 , "DN" , inl_etcsl_h
wEN, NS_INL , INL_NO , INL_YES , 0 , 0 , "EN" , inl_etcsl_h
wGN, NS_INL , INL_NO , INL_YES , 0 , 0 , "GN" , inl_etcsl_h
wON, NS_INL , INL_NO , INL_YES , 0 , 0 , "ON" , inl_etcsl_h
wPN, NS_INL , INL_NO , INL_YES , 0 , 0 , "PN" , inl_etcsl_h
wRN, NS_INL , INL_NO , INL_YES , 0 , 0 , "RN" , inl_etcsl_h
wSN, NS_INL , INL_NO , INL_YES , 0 , 0 , "SN" , inl_etcsl_h
wTN, NS_INL , INL_NO , INL_YES , 0 , 0 , "TN" , inl_etcsl_h
wWN, NS_INL , INL_NO , INL_YES , 0 , 0 , "WN" , inl_etcsl_h
w, NS_INL , INL_NO , INL_YES , 0 , 0 ,  0, inl_etcsl_h
xref, NS_INL , INL_YES , INL_YES , 0 , 0 , 0 , inl_etcsl_h
"-", NS_INL , INL_NO , INL_NO , 0 , 0 , 0 , 0
"~", NS_INL , INL_NO , INL_NO , 0 , 0 , 0 , 0
"?", NS_INL , INL_NO , INL_YES , "?@" , 0 , "uncertain" , 0
"\"", NS_INL , INL_NO , INL_YES , "\"@" , 0 , "literal" , 0
"@*", NS_INL , INL_NO , INL_NO , 0 , 0 , "foreign",  0
"@=", NS_INL , INL_NO , INL_NO , 0 , 0 , 0 , 0
