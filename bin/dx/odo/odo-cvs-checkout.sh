#!/bin/dash
#
# Perform initial check out of a CVS project.
#
# This script can be modified so Oracc can work with CVS other than
# git.  The first argument is the "repo" -- i.e., the Oracc
# project-user; osh passes additional arguments to this script so it
# can be configured in various ways.
#
# If there is a second argument the present script assumes it is the
# repo owner, default oracc.
#
repo=$1
owner=$2
if [ "$owner" = "" ]; then
    owner=oracc
fi
me=`who am i`
echo $0: running as $me
echo $0: checking out $owner/$repo in `pwd`
git clone git@github.com:$owner/$repo.git
