#!/bin/sh
sudo find /home/oracc/www/p4.d/htm -name '*.cun' -or -name '*.cfy' -exec rm '{}' ';'
