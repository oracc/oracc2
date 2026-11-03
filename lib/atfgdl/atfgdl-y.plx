#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use Getopt::Long;

GetOptions(
    );

my @gt = `grep '^%\\(token\\|nterm\\)' ../gdl/gdl.y`;

open(Y, 'atfgdl.yy') || die;

while (<Y>) {
    last if /GDLTOK/;
    print;
}

print @gt;

while (<Y>) {
    last if /GDL\.Y/;
    print;
}

open(G, '../gdl/gdl.y') || die;
while (<G>) {
    last if /^%%$/;
}
while (<G>) {
    last if /^%%$/;
    s/gdllval/aglval/g;
    print;
}
close(G);

while (<Y>) {
    print;
}

print `cat y-funcs.c`;

close(Y);

1;

################################################################################

