#!/bin/sh
showttfx $1 >ttf.txt
grep '^ Glyph' ttf.txt | sed 's/^.*> //' | grep U+[1F]
grep '<-' ttf.txt | cut -d'(' -f2 | cut -d')' -f1
grep '\.[0-9]' ttf.txt | grep Base | cut -d'(' -f2 | cut -d')' -f1
