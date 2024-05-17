#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use Getopt::Long;

GetOptions(
    );

my $x = shift @ARGV;
my $ok = 0;
my @block = ();
open(G,'gsort.log') || die;
while (<G>) {
    if (/^\#\#\#/) {
	$ok = 0;
	push @block, $_;
    } elsif (/^>>/) {
	push @block, $_;
	print @block if $ok;
	@block = ();
    } else {
	$ok = 1 if /$x/;
	push @block, $_;
    }	
}

1;
