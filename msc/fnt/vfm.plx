#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use Getopt::Long;

my $ascender = 900;
my $descender = -300;

GetOptions(
    a=>\$ascender,
    d=>\$descender,
    );

# Read a Fontlab .vfm file and report on characters with ascender >
# $ascender or descender < $descender
my $u = undef;
my $a = undef;
my $d = undef;
while (<>) {
    /\"(u[0-9A-F]{5})\"/ && ($u = $1)
	|| /\"ymax\": (\d+),/ && ($a = $1)
	|| /\"ymin\": (-?\d+)/ && ($d = $1);
    report() if /^\s*\},\s*$/;
}

sub report {
    if ($u) {
	if ($a > $ascender || $d < $descender) {
	    warn "$u\t$a\t$d\n";
	}
    }
}

1;
