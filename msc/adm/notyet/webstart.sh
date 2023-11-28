#!/bin/sh
echo Killing current webserver processes ...
killall httpd
echo Napping for 3 seconds
sleep 3
echo Starting new webserver processes ...
httpd=`which httpd`
if [ "$httpd" = "" ]; then
    httpd=`which apache2`
    if [ "$httpd" = "" ]; then
	echo "$0: httpd/apache2 not found or installed. Stop"
	exit 1
    else
	$httpd -f /etc/apache2/apache2.conf
    fi
else
    $httpd -f /etc/httpd/conf/httpd.conf
fi
echo done.

#echo Starting OpenOffice service ...
#soffice -headless -accept="socket,host=127.0.0.1,port=8100;urp;" -nofirststartwizard &
