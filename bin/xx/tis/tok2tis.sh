#!/bin/sh
./tokex <dcclt.tok | ./tokix -d . >col3.tis
./tokpct <col3.tis >col4.tis 2>tokpct.log

