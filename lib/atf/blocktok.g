%{
#include <string.h>
#include "block.h"
#include "blocktok.h"
#include "atf.tab.h"
%}
struct block_token;
%%
composite,	Y_COMPOSITE, NULL, B_TEXT, NULL, NULL, NULL
score,	Y_SCORE, NULL, B_TEXT, NULL, NULL, NULL
transliteration,	Y_TRANSLITERATION, NULL, B_TEXT, NULL, NULL, NULL
tablet,	Y_TABLET, "tablet", B_OBJECT, NULL, NULL, NULL
bulla,	Y_BULLA, "bulla", B_OBJECT, "Bulla", "Bulla", "bulla"
prism,	Y_PRISM, "prism", B_OBJECT, "Prism", "Prism", "prism"
envelope,	Y_ENVELOPE, "envelope", B_OBJECT, "Envelope", "Env.", "env"
object,	Y_OBJECT, "", B_OBJECT, NULL, NULL, NULL
obverse,	Y_OBVERSE, "obverse", B_SURFACE, "Obverse", "Obv.", "o"
reverse,	Y_REVERSE, "reverse", B_SURFACE, "Reverse", "Rev.", "r"
side,	Y_SIDE, "side", B_SURFACE, "Side", "Side", "s."
left,	Y_LEFT, "left", B_SURFACE, "Left edge", "Left", "l.e."
right,	Y_RIGHT, "right", B_SURFACE, "Right edge", "Right", "r.e."
top,	Y_TOP, "top", B_SURFACE, "Top edge", "Top", "t.e."
bottom,	Y_BOTTOM, "bottom", B_SURFACE, "Bottom edge", "Bottom", "b.e."
edge,	Y_EDGE, "edge", B_SURFACE, "Edge", "Edge", "e."
face,	Y_FACE, "face", B_SURFACE, "Face", "face", NULL
surface,	Y_SURFACE, "", B_SURFACE, NULL, NULL, NULL
seal,	Y_SEAL, "seal", B_SURFACE, "Seal", "seal", NULL
docket,	Y_DOCKET, "docket", B_SURFACE, "Docket", "docket", NULL
column,	Y_COLUMN, NULL, B_COLUMN, NULL, NULL, NULL
m,	Y_M, "", B_MILESTONE, NULL, NULL, NULL
fragment,	Y_FRAGMENT, "fragment", B_MILESTONE, "Fragment", "Frag.", "frg"
cfragment,	Y_CFRAGMENT, "cfragment", B_MILESTONE, "Fragment", "Frag.", "frg"
body,	Y_BODY, "body", B_MILESTONE, NULL, NULL, NULL
colophon,	Y_COLOPHON, "colophon", B_MILESTONE, NULL, NULL, NULL
catchline,	Y_CATCHLINE, "catchline", B_MILESTONE, NULL, NULL, NULL
date,	Y_DATE, "date", B_MILESTONE, NULL, NULL, NULL
sealings,	Y_SEALINGS, "sealers", B_MILESTONE, NULL, NULL, NULL
linecount,	Y_LINECOUNT, "linecount", B_MILESTONE, NULL, NULL, NULL
summary,	Y_SUMMARY, "summary", B_MILESTONE, NULL, NULL, NULL
signature,	Y_SIGNATURE, "signature", B_MILESTONE, NULL, NULL, NULL
witnesses,	Y_WITNESSES, "witnesses", B_MILESTONE, NULL, NULL, NULL
include,	Y_INCLUDE,"include",B_MILESTONE, NULL, NULL, NULL
referto,	Y_REFERTO,"referto",B_MILESTONE, NULL, NULL, NULL
div,	Y_DIV, "div", B_DIVISION, NULL, NULL, NULL
end,	Y_END, "end", B_DIVISION, NULL, NULL, NULL
translation,	Y_TRANSLATION, NULL, B_TR_TRANS, NULL, NULL, NULL
transtype,	Y_TRANSTYPE, NULL, B_TR_SWITCH, NULL, NULL, NULL
label,	Y_LABEL, NULL, B_TR_LABEL, NULL, NULL, NULL
unit,	Y_UNIT, NULL, B_TR_UNIT,  NULL, NULL, NULL
span,	Y_SPAN,  NULL, B_TR_SPAN,  NULL, NULL, NULL
note,	Y_NOTE,   NULL, B_TR_NOTE,  NULL, NULL, NULL
h1x,	Y_H1X,        NULL, B_H1,       NULL, NULL, NULL
h2x,	Y_H2X,        NULL, B_H2,       NULL, NULL, NULL
h3x,	Y_H3X,        NULL, B_H3,       NULL, NULL, NULL
h1,	Y_H1,    NULL, B_TR_H1,    NULL, NULL, NULL
h2,	Y_H2,    NULL, B_TR_H2,    NULL, NULL, NULL
h3,	Y_H3,    NULL, B_TR_H3,    NULL, NULL, NULL
variants,	Y_VARIANTS, "variants", B_DIVISION, NULL, NULL, NULL
variant,	Y_VARIANT,  "variant", B_DIVISION, NULL, NULL, NULL
endvariants,	Y_ENDVARIANTS,	"endvariants", B_DIVISION, NULL, NULL, NULL
