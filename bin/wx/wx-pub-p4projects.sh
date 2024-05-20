#!/bin/sh
grep -A1 p4projects.g ../wx/p4projects.c \
    | grep '{"' | tr -d ' {}",' >/home/oracc/lib/data/p4projects.lst
