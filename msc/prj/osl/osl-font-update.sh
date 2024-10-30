#!/bin/sh
echo $0
sudo ./osl-prepare-update.sh
sudo su -l ogsl /home/ogsl/osl/00bin/osl-font-update.sh
sudo ./osl-sx-slix.sh
