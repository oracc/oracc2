#!/bin/sh
xl -xpath './/*[@url]/@url' ../01tmp/source-tree-40.xml | sed 's/^ url=//' | tr -d '"=' | sed 's/$/index.html/'
