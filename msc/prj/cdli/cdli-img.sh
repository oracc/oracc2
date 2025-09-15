#!/bin/sh
#
# Note: the cdli python api needs to be installed for this to work
#
# only functional on dev-oracc atm
#
cdli export -f csv -o imgs.csv -e artifact-assets
