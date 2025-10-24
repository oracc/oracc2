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
my $ndiv = 0;
demo_header();
foreach (@cfy) {
    demo_div($_);
    ++$ndiv;
}
demo_trailer();

1;

################################################################################

sub demo_div {
    my $cfy = shift;
    my $base = $cfy;
    my $tclass = sprintf("t%02d",$ndiv+1);
    $base =~ s/\.html$//;
    my $atf = xmlify(join('',`grep '^[0-9@=	]' $base.atf`));
    my $ccf = xmlify(join('',`cat $base.ccf`));
    my $txt = xmlify(join("\n",`cat $base.txt`));
    my @txt = split(/\n/, $txt); $txt = join("<br/>", grep (!/^$/, @txt));
    print <<EOF;
<div class="test">
  <h1 class="test">Test $base</h1>
  <p>$txt</p>
  <h2>ATF</h2>
  <pre>
$atf</pre>
  <h2>Configuration</h2>
  <pre>
$ccf</pre>
  <h2 class="cuneified">Cuneified</h2>
  <iframe src="$cfy" class="$tclass"></iframe>
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
<body class="demo"><h1>Cuneify Test/Demo Page</h1>
<p>In the examples below, the Cuneified display uses <code>iframe</code> to include
entire cuneified documents into the Test/Demo page. The HTML/CSS implementation
can be explored using the browser inspector facility.</p>
<div class="grid">
EOF
}

sub demo_trailer {
    print "</div></body></html>";
}
