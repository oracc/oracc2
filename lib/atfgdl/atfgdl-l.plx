#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use Getopt::Long;

GetOptions(
    );

my @defs = ();
my @starts = ();
my @pat = ();

my $saw_MTS = 0;
my $saw_EXX = 0;

my $afh;
my $gfh;

load_pro('../atf/atf.l');
load_pro('../gdl/gdl.l');

print <<EOF;
%option prefix="ag"
%option never-interactive noyywrap yylineno nounput

%{
EOF
print `cat l-prologue.c`;
print "%}\n";

print @defs;
print "\n";
print @starts;

print "\n%%\n";

while (<$afh>) { last if /ATFGDL STOPS HERE/; my $f = fix($_); print $f if $f }
while (<$gfh>) { last if /^%%/; print fix($_); }

print "\n%%\n";

print `cat l-funcs.c`;
fix_and_print('../atf/atf-l-funcs.c');
fix_and_print('../gdl/gdl-l-funcs.c');

1;

################################################################################

sub fix {
    my $t = shift;
    $t =~ s/(?:atf|yy)_delete/ag_delete/g;
    $t =~ s/(?:atf|gdl)flextrace/agflextrace/g;
    $t =~ s/(?:atf|gdl)_l_lex_init/ag_l_lex_init/g;
    $t =~ s/(?:atf|gdl)lloc/aglloc/g;
    $t =~ s/(?:atf|gdl)lineno/aglineno/g;
    $t =~ s/(?:atf|gdl)lval/aglval/g;
    $t =~ s/(?:atf|gdl)_scan_buffer/ag_scan_buffer/g;
    $t =~ s/(?:atf|gdl)set_lineno/agset_lineno/g;
    $t =~ s/(?:atf|gdl)text/agtext/g;
    if (/return (?:BIL|EXX|MTS|VAR)/) {
	if (/MTS/) {
	    if (!$saw_MTS++) {
		$t =~ s/\\\./\\.\//;
		$t =~ s/t\]/t\\n\\r]/;
	    } else {
		$t = undef;
	    }
	} elsif (/EXX/) {
	    if (!$saw_EXX++) {
		$t =~ s/:\[/:\/\[/;
		$t =~ s/\]\+(\s)/\\n\\r]+$1/;
	    } else {
		$t = undef;
	    }
	}
	$t =~ s/BEGIN s_T; longinit/BEGIN ag_line_save(agtext)/
			       if $t;

    }
    $t;
}

sub fix_and_print {
    open(F,$_[0]) || die;
    while (<F>) {
	my $f = fix($_);
	print $f if $f;
    }
    close(F);    
}

sub load_pro {
    my $f = shift;
    my $fh;
    if ($f =~ /gdl/) {
	open($gfh, $f) || die;
	$fh = $gfh
    } else {
	open($afh, $f) || die;
	$fh = $afh;
    }
    while (<$fh>) {
	last if /^%\}/;
    }
    while (<$fh>) {
	last if /^%%/;
	next if /^\s*$/;
	if (/^\%[sx]/) {
	    push @starts, $_;
	} else {
	    push @defs, $_;
	}
    }
}
