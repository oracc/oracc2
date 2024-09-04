#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use Getopt::Long;

GetOptions(
    );

my %cfgw = (); load_epsd();

while (<>) {
    chomp;
    my($n,$ngm) = split(/\t/,$_);
    my @ngm = split(/\]\s+/,$ngm);
    my $bad = 0;
    foreach my $one (@ngm) {
	$one .= ']' unless $one =~ /\]$/;
	unless ($cfgw{$one}) {
	    warn "$.: $one unknown (n=$n)\n" if $n > 0;
	    ++$bad;
	}
    }
    print "$n\t$ngm\n" unless $bad;
}

sub load_epsd {
    open(E,'/home/oracc/epsd2/00src/sux.glo')
	|| open(E,'/Users/stinney/orc/epsd2/00src/sux.glo')
	|| die;
    while (<E>) {
	if (/^\@entry\S*\s+(.*?)\s+\[(.*?)\]/) {
	    my $cfgw = "$1\[$2\]";
	    ++$cfgw{$cfgw};
	}
    }
    close(E);
}

1;
