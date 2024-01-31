#!/bin/sh
#
# pass output of tokx via stdin and write 4-col output to stdout
#
# TOKENID TOKEN COUNT PERCENT
#
tokex | tokix -p `oraccopt` | tokpct
