#!/bin/sh
cd /root
rm -fr restoreset
mkdir -p restoreset/tar
mkdir -p restoreset/doc/{admin,install}
mkdir -p restoreset/etc
mkdir -p restoreset/usr/share/fonts
cp -a /etc/{passwd,shadow,group,gshadow} restoreset/etc
cp -a /etc/httpd/conf restoreset/etc
cp -a /etc/sysconfig/network-scripts/ifcfg-eth0 restoreset/
cp -a /etc/cron.{allow,deny} restoreset/etc
cp -a /etc/resolv.conf restoreset/etc
cp -a /etc/mime.types restoreset/etc
crontab -l -u root >restoreset/root.cron
crontab -l -u cdli >restoreset/cdli.cron
cp -a /usr/share/fonts/{sac,ungkam} restoreset/usr/share/fonts
cp -a ${ORACC}/src/doc/sysadmin/*.txt restoreset/doc/admin
cp -a ${ORACC}/src/doc/install restoreset/doc/install
tar zcf restoreset/tar/oracc.tar.gz ${ORACC}/{00,00any,bin,etc,lib,src}
# cp /home/cdli/00any/{cdliatf.atf.gz,cdlicat.csv.gz,image.data.gz} restoreset/tar
tar zcf oracc-server-restore.tar.gz restoreset
