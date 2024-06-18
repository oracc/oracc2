#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use Getopt::Long;

GetOptions(
    );

while (<>) {
    s#href="javascript:distprof2\('\{\$basename\}','\{/\*/\@xml:lang\}','\{\@xis\}'\)"#href="javascript://" onclick="act_isid(event)" data-lang="{/*/\@xml:lang}" data-isid="{\@xis}"#;
    print;
}

1;
