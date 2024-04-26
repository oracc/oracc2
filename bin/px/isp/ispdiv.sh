#!/bin/dash
# $1=project $2=input $3=output $4=cemd
if [ "$4" = "ccat" ]; then
    cat $2 | /home/oracc/bin/cex -p $1 -icat |
	xsltproc -stringparam project $1 /home/oracc/lib/scripts/isp-ce-HTML.xsl - \
		 >$3	
else
    /home/oracc/bin/wmx -p $1 -l -i $2 |
	/home/oracc/bin/cextfx -4 -p $1 -l | \
	    xsltproc -stringparam project $1 /home/oracc/lib/scripts/isp-ce-HTML.xsl - \
		     >$3
fi
# remove the output if it's empty
if [ ! -s $3 ]; then
    rm -f $3
    exit 1
fi
