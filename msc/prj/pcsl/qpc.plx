#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use Getopt::Long;

GetOptions(
    );
my $P = '';
my %qpc = ();

while (<>) {
    if (/^\&(P\d+)/) {
	if (/^\&P00[89]/
	    || /Ghazir|MDP|PETF|Porada 0001|Ozbaki 01|Sb 15222| TSF/
	    || /composite\s*$/
	    || /CDLI Seal/
	    || /UET 2/) {
	    $P = undef;
	} else {
	    $P = $1;
	}
    } elsif (/^\#atf:\s+lang\s+qpc/) {
	++$qpc{$P} if $P;
	$P = undef;
    }
}

print join("\n", sort keys %qpc), "\n";

1;

################################################################################

