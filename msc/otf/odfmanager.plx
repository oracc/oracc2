#!/usr/bin/perl
use warnings; use strict;
use lib "$ENV{'ORACC'}/lib";
use ORACC::XPD::Util;
use Getopt::Long;

my $base = '';
my $driver = '';
my $driver_only = 0;
my @files = ();
my $list = '';
my $project = '';
my $tabular = 0;
my $title = '';
my $tmpdir = "01tmp/odt$$";

GetOptions (
    'driver'=>\$driver_only,
    'list:s'=>\$list,
    'project:s'=>\$project,
    'tabular'=>\$tabular,
    'title:s'=>\$title,
    );

@files = @ARGV;

if ($list && $#files >= 0) {
    die "odfmanager.plx: can't use -list and give filenames on command line\n";
}

if (!$list && $#files < 0) {
    if (-r "00lib/project.otf") {
	push @files, "00lib/project.otf";
    }
}

$project = ORACC::XPD::Util::project() unless $project;

if (!$title) {
    ORACC::XPD::Util::load_xpd();
    $title = ORACC::XPD::Util::option('name');
}

if ($#files == 0) {
    $driver = $files[0];
} else {
    $driver = make_driver();
}

if ($driver_only) {
    print "$driver\n";
    exit 0;
}

if ($tabular) {
    warn "odfmanager using tabular mode\n";
    exec 'otf2odt-tab.sh', $driver, $project;
} else {
    exec 'otf2odt.sh', $driver, $project;
}

#################################################################################

sub
make_driver {
    my @l = ();
    my $mode = '';
    my @listfiles = ();

    system 'mkdir', '-p', $tmpdir;

    if ($list) {
	@listfiles = list2files($list);
    } elsif ($#files >= 0) {
	foreach my $f (@files) {
	    if ($f =~ /\.otf$/) {
		push @listfiles, $f;
	    } elsif ($f =~ /\.atf$/) {
		push @listfiles, atf2files($f);
	    } else {
		my @tmp = ();
		@tmp = list2files($f);
		push @listfiles, @tmp;
	    }
	}
    } else {
	@l = <00atf/*.otf>;
	if ($#l == -1) {
	    push @listfiles, '01bld/lists/have-atf.lst';
	} else {
	    push @listfiles, @l;
	}
    }

    my $d = "$tmpdir/driver.otf";

    if ($#listfiles == 0 && $listfiles[0] =~ /\.otf/) {
	system 'cp', $listfiles[0], $d;
    } else {
	open(D,">$d") || die "odfmanager.plx: can't create driver `$d'\n";
	select D;
	print "\@document\n";
	print "\@project $project\n";
	print "\@title $title\n";
	foreach my $f (@listfiles) {
	    if ($f =~ /\.otf$/) {
		print "\@include /home/oracc/$project/$f\n";
	    } else {
		print "\@section edition\n\@atffile /home/oracc/$project/$f\n\n\@end section\n";
	    }
	}
	print "\@end document\n";
	close(D);
	select STDOUT;
    }

    $d;
}

sub
atf2files {
    my $a = shift;
    my @atf = `atfgrabpq.plx -atf $a`;
    if ($#atf == 0) {
	($a);
    } elsif ($#atf < 0) {
	();
    } else {
	my $atftmp = "$tmpdir/$a.d";
	system 'mkdir', '-p', $atftmp;
	system 'atfsplit.plx', '-dir', $atftmp, '-shallow', $a;
	@atf = `find $atftmp -name '*.atf'`;
	chomp @atf;
	@atf;
    }
}

sub
list2files {
    my $l = shift;
    my @f = ();
    open(L,$l) || die "odfmanager.plx: can't open $l\n";
    while (<L>) {
	chomp;
	if (/\.lst$/) {
	    push @f, list2files($_);
	} elsif (/\.otf$/) {
	    push @f, $_;
	} elsif (/\.atf$/) {
	    push @f, atf2files($_);
	} else {
	    warn "odfmanager.plx: unknown file type `$_'\n";
	}
    }
    @f;
}
