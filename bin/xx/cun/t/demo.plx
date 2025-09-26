#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use ORACC::XML;

use Getopt::Long;

GetOptions(
    );

my @cfy = (<t*.html>);
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
    my $atf = xmlify(join('',`cat $base.atf`));
    my $ccf = xmlify(join('',`cat $base.ccf`));
    my $txt = xmlify(join('',`cat $base.txt`));
    print <<EOF;
<div class="cfy-demo">
  <h1>Test $base</h1>
  <p>$txt</p>
  <h2>$base.atf</h2>
  <pre>
$atf</pre>
  <h2>$base.ccf</h2>
  <pre>
$ccf</pre>
  <h2>$base.html</h2>
  <iframe src="$cfy" width="300px"></iframe>
</div>
EOF
}

sub demo_header {
    print <<EOF;
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<title>Cuneify Demo Page</title>
<link rel="stylesheet" type="text/css" href="demo.css"/>
</head>
<body>
EOF
}

sub demo_trailer {
    print "</body></html>";
}
