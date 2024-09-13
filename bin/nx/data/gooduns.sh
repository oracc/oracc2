#!/bin/sh
#
# remove numbers that don't start with digits or which contain '+' items
grep '	[0-9]' | grep -v -F '+n'
