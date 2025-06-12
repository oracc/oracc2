#!/bin/sh
xmllint -xpath '*/*/*[1]/text()' 00lib/data.xml >00lib/outlined.lst
