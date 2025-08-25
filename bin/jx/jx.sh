#!/bin/sh
jx <$1 | xsltproc $ORACC_BUILDS/lib/scripts/jx-uptag.xsl - \
    | xsltproc $ORACC_BUILDS/lib/scripts/jx-un-a.xsl -
