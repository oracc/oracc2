#!/bin/sh
# Identify signs that are in 'need' (usually a corpus) that are not in
# the given signlist (usually the csl for the corpus)
#
# To compare against a master sign list give that project as the arg
# to aslsigns.sh, i.e., aslsigns.sh osl >have
aslsigns.sh >have
cut -f1 need | sort -u >need.1
cut -f1 have | sort -u >have.1
comm -23 need.1 have.1
