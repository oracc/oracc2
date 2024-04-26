#!/bin/dash
# $1=project $2=input $3=output
/home/oracc/bin/wmx -p $1 -l -i $2 |
    /home/oracc/bin/cextfx -4 -p $1 -l | \
	xsltproc -stringparam project $1 /home/oracc/lib/scripts/isp-ce-HTML.xsl - \
		 >$3
