#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use Getopt::Long;

GetOptions(
    );

my %tab = (); my @t = `cat sp2`; chomp @t; foreach (@t) { my($from,$to) = split(/\t/, $_); $tab{$from} = $to; }

open(G,'gur.ngm');
while (<G>) {
    chomp;
    my($n,$ngm) = split(/\t/,$_);
    my @ngm = split(/\]\s+/, $ngm);
    my @new = ();
    foreach my $one (@ngm) {
	$one .= ']' unless $one =~ /\]$/;
	if ($tab{$one}) {
	    push @new, $tab{$one};
	} else {
	    push @new, $one;
	}
    }
    print "$n\t@new\n";
}
close(G);

1;
