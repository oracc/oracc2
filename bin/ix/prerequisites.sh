#!/bin/sh
pkg=$1
if [ "$pkg" = "" ]; then
    echo $0: must give package name on commandline. Stop.
    exit 1
fi

while read pkg <$1
do
    sudo apt upgrade $pkg
done

# Perl is being phased out of Oracc but for now we install some packages that Oracc I uses

while read pkg <pkg/perl.txt
do
    sudo apt upgrade $pkg
done

while read pkg <pkg/perl-packages.txt
do
    cpan install $pkg
done
