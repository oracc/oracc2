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
my $c = $x; $c =~ s/xml$/c/;
my $s = $x; $s =~ s/.xml//;
open(X,$x) || die "$0: can't open $x\n";
open(C,">$c") || die "$0: can't write $c\n"; select C;
print "const char *$s = \n";
while (<X>) {
    chomp;
    s/"/\\"/g;
    print "\"$_\\n\"\n";
}
print ";\n";

close(C);

1;
