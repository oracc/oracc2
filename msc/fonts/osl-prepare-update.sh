#!/bin/sh
(cd /home/oracc/osl
 chown -R root:oracc 00lib/osl.asl 02pub
 chmod -R g+w 00lib/osl.asl 02pub
)
