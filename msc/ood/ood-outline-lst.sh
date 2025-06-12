#!/bin/sh
xmllint -xpath '*/*/*[1]/text()' 00lib/data.xml |sed 's/^/o/'>00lib/outlined.lst
