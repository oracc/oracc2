#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use Getopt::Long;

GetOptions(
    );

my %ngm = ();
while (<>) {
    chomp;
    my($n,$ngm) = split(/\t/,$_);
    if ($ngm{$ngm}) {
	$ngm{$ngm} += $n;
	warn "merging $ngm\n";
    } else {
	$ngm{$ngm} = $n;
    }
}

foreach my $n (sort { $ngm{$b} <=> $ngm{$a} } keys %ngm) {
    print "$ngm{$n}\t$n\n";
}

1;
