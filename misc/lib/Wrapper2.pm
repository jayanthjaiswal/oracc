package ORACC::Wrapper2;
use warnings; use strict;
use lib '@@ORACC@@/lib';
use ORACC::Expand2;
use ORACC::XML;
use Getopt::Long;
use Pod::Usage;

my $cat = 0;
my $from_project = '';
my $help = 0;
my $man = 0;
my $quiet = 0;
my $project = '';
my $stdout = 0;
my $trans = 'en';
my $verbose = 0;
my $webdir = '';

my %args = ();
my @files = ();
my @list = ();

sub
files {
    if ($#list >= 0) {
	foreach my $list (@list) {
	    open(IN, $list) 
		|| (warn "$args{'prog'}: can't open list $list\n" and next);
	    while (<IN>) {
		chomp;
		push @files, $_;
	    }
	    close(IN);
	}
    } elsif ($#ARGV >= 0) {
	@files = @ARGV;
    } else {
	while (<>) {
	    chomp;
	    push @files, $_;
	}
    }
}

sub
options {
    GetOptions(
	'help'=>\$help,
	'list:s'=>\@list,
	'man'=>\$man,
	'from:s'=>\$from_project,
	'project:s'=>\$project,
	'quiet'=>\$quiet,
	'stdout'=>\$stdout,
	'verbose'=>\$verbose,
	'webdir:s'=>\$webdir,
	) || pod2usage(2);
    pod2usage(1) if $help;
    pod2usage(-exitstatus=>0,-verbose=>2) if $man;
}

sub
xslt {
    %args = @_;
    $args{'prog'} = $0;
    $trans = $args{'trans'} if $args{'trans'};
    options();
    $args{'outdir'} = "$webdir/t"
	if $webdir;
    files();
    my $callback = $args{'callback'};
    my $transformer = load_xsl($args{'script'});
    foreach my $f (@files) {
	my $orig_f = $f;
	my $file_project = undef;
	if ($f =~ /:/) {
	    ($file_project,$f) = ($f =~ /^(.*?):(.*?)$/);
#	    if ($from_project) {
#		next if $file_project ne $from_project;
#	    } elsif ($project) {
#		next if $file_project ne $project;
#	    }
	}
	if ($f =~ /^[PQX]\d+(?:[a-z]*)$/) {
	    if ($file_project) {
		$f = expand_in_project(undef,"$f.$args{'from'}", $file_project);
	    } elsif ($from_project) {
		$f = expand_in_project(undef,"$f.$args{'from'}", $from_project);
	    } elsif ($project) {
		$f = expand_in_project(undef,"$f.$args{'from'}", $project);
	    } else {
		$f = expand("$f.$args{'from'}");
	    }
	} else {
	    $f =~ s/\.[^.]*$/.$args{'from'}/;
	}
	warn("$0: no such input file: $f\n") and next
	    unless -f $f;
	my $xf = load_xml($f);
	my $vprefix = ($args{'to'} eq '-') ? '' : '*.';
	print STDERR "$f => $vprefix$args{'to'} ... " if $verbose;
	my $res = eval { $transformer->transform($xf, project=>"'$project'", trans=>"'$trans'") };
	unless ($res) {
	    undef $xf;
	    print STDERR $f unless $verbose;
	    print STDERR " failed\n";
	    print STDERR "---\nTransform reports:\n$@\n---\n";
	    next;
	} else {
	    print STDERR " ok\n" if $verbose;
	    &$callback($res) if $callback;
	}
	if ($args{'to'} eq '-') {
	    binmode STDOUT, ':raw';
	    print $transformer->output_string($res);
	} else {
	    my $outf = $f;
	    if ($from_project) {
		$outf = expand_in_project($project,"$orig_f.$args{'from'}");
	    }
	    if ($args{'to'} =~ /-/) {
		$outf =~ s/\.[^.]+$/$args{'to'}/ 
		    || (warn("$0: $f: input and output files are the same; skipping\n") and next);
	    } else {
		$outf =~ s/\.[^.]+$/.$args{'to'}/ 
		    || (warn("$0: $f: input and output files are the same; skipping\n") and next);
		if ($args{'outdir'}) {
		    $outf =~ s#^.*?/([^/]+)$#$args{'outdir'}/$1#;
		}
		if ($trans ne 'en') {
		    $outf .= ".$trans";
		}
	    }
	    # N.B: this stream should not be opened in ':utf8'!  
	    # output_string() comes from outside Perl and does
	    # not have the utf8 flag set, so setting the output stream 
	    # to utf8 results in garbled UTF8.
	    if ($stdout) {
		binmode STDOUT, ':raw';
		print $transformer->output_string($res);
	    } else {
		open(OUT,">$outf") 
		    || (warn("$args{'prog'}: can't write result to $outf\n") and next);
		binmode OUT, ':raw';
		print OUT $transformer->output_string($res);
		close(OUT);
	    }
	}
	undef $xf;
	undef $res;
    }
    undef $transformer;
    return 1;
}

1;
