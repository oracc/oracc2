%{
#include <string.h>
#include "wrapper.h"
%}
struct progtab;
%%
"xtf2cun", "p4-cuneify.xsl", NULL, "xtf", "cun"
"xtf2prf", "p4-proofing.xsl", NULL, "xtf", "prf"
"xtf2tei", "xtf2tei.xsl", NULL, "xtf", "tei"
"xtf2txh", "g2-xtf-HTML.xsl", NULL, "xtf", "txh"
"xmd2htm", "p4-htmlview.xsl", NULL, "xmd", "html"
