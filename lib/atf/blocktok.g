%{
#include <string.h>
#include "block.h"
#include "blocktok.h"
%}
struct block_token;
%%
composite,  e_composite, NULL, B_TEXT, NULL, NULL, NULL
score,  e_score, NULL, B_TEXT, NULL, NULL, NULL
transliteration, e_transliteration, NULL, B_TEXT, NULL, NULL, NULL
tablet,  	e_object, "tablet", B_OBJECT, NULL, NULL, NULL
bulla,		e_object, "bulla", B_OBJECT, "Bulla", "Bulla", "bulla"
prism,		e_object, "prism", B_OBJECT, "Prism", "Prism", "prism"
envelope,	e_object, "envelope", B_OBJECT, "Envelope", "Env.", "env"
object,	    e_object, "", B_OBJECT, NULL, NULL, NULL
obverse, 	e_surface, "obverse", B_SURFACE, "Obverse", "Obv.", "o"
reverse,	e_surface, "reverse", B_SURFACE, "Reverse", "Rev.", "r"
side,	e_surface, "side", B_SURFACE, "Side", "Side", "s."
left,		e_surface, "left", B_SURFACE, "Left edge", "Left", "l.e."
right,		e_surface, "right", B_SURFACE, "Right edge", "Right", "r.e."
top,		e_surface, "top", B_SURFACE, "Top edge", "Top", "t.e."
bottom,	    e_surface, "bottom", B_SURFACE, "Bottom edge", "Bottom", "b.e."
edge,		e_surface, "edge", B_SURFACE, "Edge", "Edge", "e."
face,		e_surface, "face", B_SURFACE, "Face", "face", NULL
surface,	e_surface, "", B_SURFACE, NULL, NULL, NULL
seal,		e_surface, "seal", B_SURFACE, "Seal", "seal", NULL
docket,		e_surface, "docket", B_SURFACE, "Docket", "docket", NULL
column, 	e_column, NULL, B_COLUMN, NULL, NULL, NULL
m,			e_m, "", B_MILESTONE, NULL, NULL, NULL
fragment,	e_m, "fragment", B_MILESTONE, "Fragment", "Frag.", "frg"
cfragment,	e_m, "cfragment", B_MILESTONE, "Fragment", "Frag.", "frg"
body,		e_m, "body", B_MILESTONE, NULL, NULL, NULL
colophon,	e_m, "colophon", B_MILESTONE, NULL, NULL, NULL
catchline,	e_m, "catchline", B_MILESTONE, NULL, NULL, NULL
date,		e_m, "date", B_MILESTONE, NULL, NULL, NULL
sealings,	e_m, "sealers", B_MILESTONE, NULL, NULL, NULL
linecount,	e_m, "linecount", B_MILESTONE, NULL, NULL, NULL
summary,	e_m, "summary", B_MILESTONE, NULL, NULL, NULL
signature,  e_m, "signature", B_MILESTONE, NULL, NULL, NULL
witnesses,  e_m, "witnesses", B_MILESTONE, NULL, NULL, NULL
include,    e_include,"include",B_MILESTONE, NULL, NULL, NULL
referto,    e_referto,"referto",B_MILESTONE, NULL, NULL, NULL
div,        e_div, "div", B_DIVISION, NULL, NULL, NULL
end,        e_div, "end", B_DIVISION, NULL, NULL, NULL
translation, e_xtr_translation, NULL, B_TR_TRANS, NULL, NULL, NULL
transtype,  e_xtr_translation, NULL, B_TR_SWITCH, NULL, NULL, NULL
label,	    e_enum_top, NULL, B_TR_LABEL, NULL, NULL, NULL
unit,	    e_enum_top, NULL, B_TR_UNIT,  NULL, NULL, NULL
span,	    e_xh_span,  NULL, B_TR_SPAN,  NULL, NULL, NULL
note,	    e_xh_div,   NULL, B_TR_NOTE,  NULL, NULL, NULL
h1x,        e_h,        NULL, B_H1,       NULL, NULL, NULL
h2x,        e_h,        NULL, B_H2,       NULL, NULL, NULL
h3x,        e_h,        NULL, B_H3,       NULL, NULL, NULL
h1,         e_xh_h1,    NULL, B_TR_H1,    NULL, NULL, NULL
h2,         e_xh_h2,    NULL, B_TR_H2,    NULL, NULL, NULL
h3,         e_xh_h3,    NULL, B_TR_H3,    NULL, NULL, NULL
variants,	e_variants, "variants", B_DIVISION, NULL, NULL, NULL
variant,    e_variant,  "variant", B_DIVISION, NULL, NULL, NULL
endvariants,e_variants, "endvariants", B_DIVISION, NULL, NULL, NULL
