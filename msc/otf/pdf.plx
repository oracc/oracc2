#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;
use Data::Dumper;

my %args = @ARGV;

#system '/home/oracc/bin/atf2pdf.sh', "$args{'project'}\:$args{'pqx'}";

print STDERR "pdf.plx calling webpdf.sh $args{'project'} $args{'pqx'}\n";

system '/home/oracc/bin/webpdf.sh', $args{'project'}, $args{'pqx'};

#if (-r $args{'pdf'}) {
#    print "Content-type: application/pdf\nContent-disposition: attachment; filename=$args{'pqx'}.pdf\n\n";
#    exec 'cat', $args{'pdf'};
#} else {
#    exit 1;
#}

1;
