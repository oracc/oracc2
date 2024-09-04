#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use Getopt::Long;

GetOptions(
    );

my %gur = ();

open(L,'gur.lem') || die;
while (<L>) {
    chomp;
    s/^#lem:\s+//;
    my @t = split(/;\s+/, $_);
    # find gur
    my $gur = 0;
    while ($gur <= $#t && $t[$gur] !~ /^gur\[/) {
	++$gur;	
    }
    $t[$gur] = 'gur[unit]';

    #continue for gur modifiers
    while ($gur < $#t) {
	if ($t[$gur+1] =~ /saŋ\[head|ŋal\[be|mah\[|lugal\[|Agade\[/) {
	    ++$gur;
	} else {
	    last;
	}
    }
    
    # find preceding 'n'
    my $n = $gur;
    while ($n && $t[$n-1] ne 'n' && $t[$n-1] ne 'u') {
	--$n;
    }
    while ($t[$n] =~ /^(ban|giŋ|gun|sila)\[/) {
	++$n;
    }
    if ($n != $gur) {
	my $k = "@t[$n..$gur]";
	$k =~ s/\]\S+/\]/g;
	if ($k =~ /[a-z]/) {
	    ++$gur{$k};
	}
    }
}
close(L);

open(N,'>gur.ngm') || die; select N;
foreach  (sort { $gur{$b} <=> $gur{$a} } keys %gur) {
    print "$gur{$_}\t$_\n";
}
close(N);

1;
