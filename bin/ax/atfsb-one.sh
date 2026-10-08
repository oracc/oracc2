#!/bin/sh
ax -sl $1 | tokx -Mls | toksigs -sS | lemsb >x/$1
