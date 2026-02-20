%{
#include <string.h>
#include "block.h"
#include "blocktok.h"
#include "atf.tab.h"
%}
struct block_token;
%%
composite,	Y_COMPOSITE,	e_composite, NULL, B_TEXT, NULL, NULL, NULL
score,	Y_SCORE,	e_score, NULL, B_TEXT, NULL, NULL, NULL
transliteration,	Y_TRANSLITERATION,	e_transliteration, NULL, B_TEXT, NULL, NULL, NULL
tablet,	Y_TABLET,	e_object, "tablet", B_OBJECT, NULL, NULL, NULL
bulla,	Y_BULLA,	e_object, "bulla", B_OBJECT, "Bulla", "Bulla", "bulla"
prism,	Y_PRISM,	e_object, "prism", B_OBJECT, "Prism", "Prism", "prism"
envelope,	Y_ENVELOPE,	e_object, "envelope", B_OBJECT, "Envelope", "Env.", "env"
object,	Y_OBJECT,	e_object, "", B_OBJECT, NULL, NULL, NULL
obverse,	Y_OBVERSE,	e_surface, "obverse", B_SURFACE, "Obverse", "Obv.", "o"
reverse,	Y_REVERSE,	e_surface, "reverse", B_SURFACE, "Reverse", "Rev.", "r"
side,	Y_SIDE,	e_surface, "side", B_SURFACE, "Side", "Side", "s."
left,	Y_LEFT,	e_surface, "left", B_SURFACE, "Left edge", "Left", "l.e."
right,	Y_RIGHT,	e_surface, "right", B_SURFACE, "Right edge", "Right", "r.e."
top,	Y_TOP,	e_surface, "top", B_SURFACE, "Top edge", "Top", "t.e."
bottom,	Y_BOTTOM,	e_surface, "bottom", B_SURFACE, "Bottom edge", "Bottom", "b.e."
edge,	Y_EDGE,	e_surface, "edge", B_SURFACE, "Edge", "Edge", "e."
face,	Y_FACE,	e_surface, "face", B_SURFACE, "Face", "face", NULL
surface,	Y_SURFACE,	e_surface, "", B_SURFACE, NULL, NULL, NULL
seal,	Y_SEAL,	e_surface, "seal", B_SURFACE, "Seal", "seal", NULL
docket,	Y_DOCKET,	e_surface, "docket", B_SURFACE, "Docket", "docket", NULL
column,	Y_COLUMN,	e_column, NULL, B_COLUMN, NULL, NULL, NULL
m,	Y_M,	e_m, "", B_MILESTONE, NULL, NULL, NULL
fragment,	Y_FRAGMENT,	e_m, "fragment", B_MILESTONE, "Fragment", "Frag.", "frg"
cfragment,	Y_CFRAGMENT,	e_m, "cfragment", B_MILESTONE, "Fragment", "Frag.", "frg"
body,	Y_BODY,	e_m, "body", B_MILESTONE, NULL, NULL, NULL
colophon,	Y_COLOPHON,	e_m, "colophon", B_MILESTONE, NULL, NULL, NULL
catchline,	Y_CATCHLINE,	e_m, "catchline", B_MILESTONE, NULL, NULL, NULL
date,	Y_DATE,	e_m, "date", B_MILESTONE, NULL, NULL, NULL
sealings,	Y_SEALINGS,	e_m, "sealers", B_MILESTONE, NULL, NULL, NULL
linecount,	Y_LINECOUNT,	e_m, "linecount", B_MILESTONE, NULL, NULL, NULL
summary,	Y_SUMMARY,	e_m, "summary", B_MILESTONE, NULL, NULL, NULL
signature,	Y_SIGNATURE,	e_m, "signature", B_MILESTONE, NULL, NULL, NULL
witnesses,	Y_WITNESSES,	e_m, "witnesses", B_MILESTONE, NULL, NULL, NULL
include,	Y_INCLUDE,	e_include,"include",B_MILESTONE, NULL, NULL, NULL
referto,	Y_REFERTO,	e_referto,"referto",B_MILESTONE, NULL, NULL, NULL
div,	Y_DIV,	e_div, "div", B_DIVISION, NULL, NULL, NULL
end,	Y_END,	e_div, "end", B_DIVISION, NULL, NULL, NULL
translation,	Y_TRANSLATION,	e_xtr_translation, NULL, B_TR_TRANS, NULL, NULL, NULL
transtype,	Y_TRANSTYPE,	e_xtr_translation, NULL, B_TR_SWITCH, NULL, NULL, NULL
label,	Y_LABEL,	e_enum_top, NULL, B_TR_LABEL, NULL, NULL, NULL
unit,	Y_UNIT,	e_enum_top, NULL, B_TR_UNIT,  NULL, NULL, NULL
span,	Y_SPAN,	e_xh_span,  NULL, B_TR_SPAN,  NULL, NULL, NULL
note,	Y_NOTE,	e_xh_div,   NULL, B_TR_NOTE,  NULL, NULL, NULL
h1x,	Y_H1X,	e_h,        NULL, B_H1,       NULL, NULL, NULL
h2x,	Y_H2X,	e_h,        NULL, B_H2,       NULL, NULL, NULL
h3x,	Y_H3X,	e_h,        NULL, B_H3,       NULL, NULL, NULL
h1,	Y_H1,	e_xh_h1,    NULL, B_TR_H1,    NULL, NULL, NULL
h2,	Y_H2,	e_xh_h2,    NULL, B_TR_H2,    NULL, NULL, NULL
h3,	Y_H3,	e_xh_h3,    NULL, B_TR_H3,    NULL, NULL, NULL
variants,	Y_VARIANTS,	e_variants, "variants", B_DIVISION, NULL, NULL, NULL
variant,	Y_VARIANT,	e_variant,  "variant", B_DIVISION, NULL, NULL, NULL
endvariants,	Y_ENDVARIANTS, e_variants,	"endvariants", B_DIVISION, NULL, NULL, NULL
