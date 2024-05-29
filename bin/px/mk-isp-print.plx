#!/usr/bin/perl
use warnings; use strict; use open 'utf8'; use utf8; use feature 'unicode_strings';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8'; binmode STDERR, ':utf8';
binmode $DB::OUT, ':utf8' if $DB::OUT;

use Data::Dumper;

use lib "$ENV{'ORACC_BUILDS'}/lib";

use Getopt::Long;

GetOptions(
    );

my $subref = 0;

my %ignore = (); my @ignore = qw/curr_cfg curr_step outline si_cache steps xpd/;
@ignore{@ignore} = (); ++$ignore{'curr_cfg *'};

my @isp = `grep -v '^#' isp/isp.h | cpp | grep -v '^#' | tr -s '\n'`; chomp @isp;

open(C,">px_print_ip.c") || die;
select C;
print "#include <oraccsys.h>\n";
print "#include <xmlify.h>\n";
print "#include \"isp/isp.h\"\n";

foreach (@isp) {
    if (/\s+steps\[/ || /\*\*/ || /Unsigned32\s+\*/) {
	# ignore
    } elsif (/^struct\s+(\S+)\s*$/) {
	# warn "<struct $1\n";
	begin_struct($1);
	++$subref;
    } elsif (/^typedef struct (\S+)/) {
	# warn "<<typedef struct $1\n";
	begin_isp();
    } elsif (/^\s+(char \*)(\S+?);/) {
	my($t,$n) = ($1,$2);
	# warn ">#$1 =$2\n";
	member('char',$2);
    } elsif (/^\s+(const char \*)(\S+?);/) {
	my($t,$n) = ($1,$2);
	# warn ">#$1 =$2\n";
	member('char',$2);
    } elsif (/^\s+(unsigned char \*)(\S+?);/) {
	my($t,$n) = ($1,$2);
	# warn ">#$1 =$2\n";
	member('char',$2);
    } elsif (/^\s+(int|Signed32|Unsigned32)\s+(\S+?);/i) {
	# warn ">#$1 =$2\n";
	member('int',$2);
    } elsif (/^\s+(long\s+int|size_t|unsigned long)\s+(\S+?);/i) {
	# warn ">#$1 =$2\n";
	member('long',$2);
    } elsif (/^\s+struct\s+(\S+(?:\s*\*)?)\s*(\S+?);/) {
	# warn ">$1 =$2\n";
	my($t,$n) = ($1,$2);
	if ($t =~ /\*/) {
	    subref('structp', $t, $n);
	} else {
	    subref('struct', $t, $n);
	}
    } elsif (/^\};/) {
	# warn ">(end block)\n";
	end_struct();
	$subref = 0;
    } elsif (/^\}\s+(\S+?);/) {
	# warn ">>\n";
	end_struct();
	$subref = 0;
    } elsif (/^\{/) {
	# warn "(start block)";
	# no action
    } elsif (/^extern/) {
	# ignore;
    } else {
	#warn "not handled $_\n";
    }
}

sub begin_isp {
    print "void\npx_print_ip(FILE *fp, Isp *ip)\n{\n";
    print "  Isp *pr = ip;\n";
    print "  fputs(\"<h2>Internal Data Structure State (Isp *ip)</h2>\\n\", fp);\n";
}
sub end_isp {
    print "}\n\n";
}

sub begin_struct {
    my $n = shift @_;
    print "void\npr_$n(FILE *fp, struct $n *pr, const char *name)\n{\n";
    print "  fprintf(fp, \"<h3>\\tstruct $n ip->\%s</h3>\\n\", name);\n";
}
sub end_struct {
    print "}\n\n";
}

sub member {
    my($t,$n) = @_;
    my $tabs = "\\t".($subref ? "\\t" : '');
    unless (exists $ignore{$n}) {
	if ($t eq 'int') {
	    print "  fprintf(fp,\"$tabs$n=%d\\n\",(int)pr->$n);\n";
	} elsif ($t eq 'char') {
	    print "  fprintf(fp,\"$tabs$n=%s\\n\",(char*)xmlify((ucp)pr->$n));\n";
	} elsif ($t eq 'long') {
	    print "  fprintf(fp,\"$tabs$n=%lu\\n\",(unsigned long)pr->$n);\n";
	} else {
	    print "###$t###$n###\n";
	}
    }
}

sub subref {
    my($p,$t,$n) = @_;
    $t =~ s/\s*\*$//;
    return if $n =~ /curr_cfg/;
    unless (exists $ignore{$t}) {
	if ($p =~ /\*/) {
	    print "  pr_$t(fp, ip->$n, \"$n\");\n";
	} else {
	    print "  pr_$t(fp, \&ip->$n, \"$n\");\n";
	}
    }
}

1;
