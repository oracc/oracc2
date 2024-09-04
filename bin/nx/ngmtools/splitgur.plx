#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use Getopt::Long;

GetOptions(
    );

open(N,'>gur-no-gur.tsv') || die;
open(G,'>gur-expr.tsv') || die;
while (<>) {
    chomp;
    my($n,$ngm) = split(/\t/,$_);
    if ($ngm =~ /^gur\[unit\]/) {
	print G "$n\t$ngm\n";    
    } else {
	my($c,$g) = ($ngm =~ /^(.*?) (gur\[unit\].*)$/);
	print N "$n\t$c\n";
	print G "$n\t$g\n";
    }
}
close(N);
close(G);

1;
