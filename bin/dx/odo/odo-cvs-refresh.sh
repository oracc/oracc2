#!/bin/dash
#
# Update the local version of a repo.
#
# This script can be modified so Oracc can work with CVS other than
# git.  The first argument is the "user" -- i.e., the Oracc project;
# osh passes additional arguments to this script so it can be
# configured in various ways.
#

if [ -r .git ]; then
    echo $0: git pull    
    git pull
else
    echo $0: `pwd` is not a git repository "(no .git)."
fi
