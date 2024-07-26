#!/bin/sh
umask 026
ORACC=/home/oracc
ORACC_BUILDS=/home/oracc
ORACC_HOME=/home/oracc
ORACC_HOST=dev-oracc.museum.upenn.edu
ORACC_MODE=multi
ORACC_USER=yes
export ORACC ORACC_HOME ORACC_HOST ORACC_BUILDS ORACC_USER ORACC_MODE
TEXINPUTS=$TEXINPUTS:/home/oracc/lib/tex/:.
TEXFORMATS=/home/oracc/lib/tex/:.
TEXFONTS=/home/oracc/lib/tex:@@ORACC_BUILDS@@/lib/tex/fonts:.
export TEXINPUTS TEXFORMATS TEXFONTS
PATH=$ORACC/bin:$PATH:$HOME/00bin:/usr/local/texlive/2024/bin/x86_64-linux
export PATH
