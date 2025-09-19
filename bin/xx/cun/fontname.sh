#!/bin/sh
cat >fontname.g <<EOF
%{
#include <stdlib.h>
#include "cfy.h"
%}
struct fontname;
%%
EOF
grep css $* | tr : '\t' | cut -f1,3 | sed 's#../../../msc/fonts/##' | sed 's/.def//' | sed 's/ofs-//' | rocox -n -R '%2,"%1"' >>fontname.g
