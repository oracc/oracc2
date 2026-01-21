#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use Getopt::Long;

GetOptions(
    );

my %g = ();
while (<>) {
    if (/^\@entry\s+(.*?)\s*$/) {
	my $cgp = $1;
	dump_g();
	$cgp =~ s/\s+\[/\[/;
	$cgp =~ s/\]\s+/\]/;
	my($c,$x) = ($cgp =~ /^(.*?)(\[.*)$/);
	$c =~ tr/ /_/;
	$g{'cgp'} = "$c$x";
	$g{'name'} = $c;
    } elsif (/^\@pl_id\s+(\S+)\s*$/) {
	$g{'id'} = $1;
    } elsif (/^\@pl_coord\s+(\[.*?\])\s*$/) {
	$g{'co'} = $1;
    } elsif (/\@alias\s+(.*?)\s*$/) {
	push @{$g{'a'}}, $1;
    }	
}

dump_g();

1;

################################################################################

sub dump_g {
    $g{'id'} = '' unless $g{'id'};
    $g{'co'} = '' unless $g{'co'};
    print "$g{'cgp'}\t$g{'name'}", "\t$g{'id'}\t$g{'co'}\t",join(",", @{$g{'a'}}),"\n"
	if $g{'cgp'};
    %g = ();
    @{$g{'a'}} = ();
}
