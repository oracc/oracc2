#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use Getopt::Long;

GetOptions(
    );

my @cfy = grep /^[01]/, (<*.html>);
demo_header();
foreach (@cfy) {
    demo_div($_);
}
demo_trailer();

1;

################################################################################

sub demo_div {
    my $cfy = shift;
    my $base = $cfy;
    $base =~ s/\.html$//;
    my $atf = `cat $base.atf`;
    my $txt = `cat $base.txt`;
    print <<EOF;
<div class="cfy-demo">
  <h1>Test $base</h1>
  <p>$txt</p>
  <h2>ATF</h2>
  <pre>
$atf
</pre>
  <h2>Cuneified</h2>
  <iframe src="$cfy" width="300px"/>
</div>
EOF
}

sub demo_header {
    print <<EOF;
<html><head><title>Cuneify Demo Page</title></head><body>
EOF
}

sub demo_trailer {
    print "</body></html>";
}
