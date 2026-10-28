#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use Getopt::Long;

GetOptions(
    );

my $rem_TEXT = 0;
my @a = `cat ../atf/atf.y`; my($a_t,$a_l) = toklab(@a);
$rem_TEXT = 1;
my @g = `cat ../gdl/gdl.y`; my($g_t,$g_l) = toklab(@g);

my %token = ();
my %nterm = ();

foreach my $p (@$a_t,@$g_t) {
    my @p = @$p;
    if ($p[0] =~ s/^(\S+)\s+(\S+)\s+//) {
	my($t1,$t2) = ($1,$2);
	if ($t1 eq '%token') {
	    push @{$token{$t2}}, \@p;
	} elsif ($t1 eq '%nterm') {
	    push @{$nterm{$t2}}, \@p;
	}
    }
}

atfgdl_header();

foreach my $t (keys %token) {
    print "%token\t$t\n";
    foreach my $p (@{$token{$t}}) {
	print newtab(@$p);
    }
    print "\n";
}

foreach my $t (keys %nterm) {
    print "%nterm\t$t";
    foreach my $p (@{$nterm{$t}}) {
	print newtab(@$p);
    }
    print "\n";
}

atfgdl_middle();

foreach my $p (@$a_l,@$g_l) {
    print @$p;
    print "\n";
}

atfgdl_trailer();

1;

################################################################################

sub atfgdl_header {
    print <<EOF;
%define api.prefix {ag}
%file-prefix"ag"
%locations
%define parse.error verbose

%{
#include <oraccsys.h>
#include <mesg.h>
#include <gdl.h>
#include <gdlstate.h>
#include <atf.h>
#include "ag.h"
extern const char *agtext, *curragfile;
extern int aglineno, agtrace;
extern int gdl_unicode, gdl_lexical;

static Tree *ytp;
static Node *ynp, *yrem, *ycp, *mnp;
extern Node *lgp;

#define AGLTYPE_IS_DECLARED 1
#define AGLTYPE Mloc
#define yylineno aglineno
AGLTYPE aglloc;

extern int aglex(void);
extern void yyerror(const char *);

extern Node *gdl_post_det_gp_attach;

#define alo(xxx) ag_line_o_##xxx
#define alc(xxx) ag_line_c_##xxx
%}

%union { char *text; int i; struct node *node; Blocktok *b; }

EOF
}

sub atfgdl_middle {
    print <<EOF;
%start atfgdl

%%

atfgdl: atfs | gdl.line

EOF
}

sub atfgdl_trailer {
    print <<EOF;
%%
EOF
    print `cat y-funcs.c`;
}

sub get_par {
    my($i,@x) = @_;
    my @p = ();
    while ($i <= $#x) {
	$x[$i] =~ s/\sTEXT\s/ / if $rem_TEXT;
	$x[$i] =~ s/(?:atf|gdl)lval/aglval/g;
	if ($x[$i] =~ /(MTS|NTS|LGS|GUS|BIL|EXX|VAR) longtext/) {
	    my $base = "$1";
	    my $lbase = "\L$base";
	    $lbase = 'var' if $lbase eq 'exx';
	    $x[$i] =~ s/\slongtext\s+/ {alo($lbase)(@1,agtext);} gdl.line /;
	    $x[$i] =~ s/, \(ucp\)longtext_get\(\)//;
	    $x[$i] =~ s/line_${base}/alc($lbase)/;
	    $x[$i] =~ s/; line_${lbase}/; alc($lbase)/;
	}
	push @p, $x[$i] unless $x[$i] =~ /^\s+END\s*$/;
	last if !defined($x[$i+1]) || $x[$i+1] =~ /^\s*$/;
	++$i;
    }
    return (\@p,$i)
}

sub newtab {
    my @n = ();
    foreach my $l (@_) {
	$l =~ s/^\s*/\t\t/;
	push @n, $l;
    }
    @n;
}

sub toklab {
    my @x = @_;
    my @t = ();
    my @l = ();
    for (my $i = 0; $i < $#x; ++$i) {
	if ($x[$i] =~ /^\s*$/) {
	    ++$i;
	    if ($x[$i] =~ /^\%(?:token|nterm)/) {
		my($p,$i) = get_par($i,@x);
		push @t, $p;
	    } elsif ($x[$i] =~ /^[^ \t]+:\s/) {
		my($p,$i) = get_par($i,@x);
		push @l, $p;
	    }
	}
    }
    (\@t, \@l);
}
