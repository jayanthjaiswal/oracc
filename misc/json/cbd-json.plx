#!/usr/bin/perl
use warnings; use strict; use open 'utf8';
binmode STDIN, ':utf8'; binmode STDOUT, ':utf8';
use lib "$ENV{'ORACC'}/lib";
use ORACC::XML;
use ORACC::JSON;

glossary_howtos();

my $projcbd = shift @ARGV;
my ($project,$lang) = split(/:/, $projcbd);

my $cbd_ns = "$ENV{'ORACC'}/bld/$project/$lang/$lang.g2x";
my $xis_ns = "$ENV{'ORACC'}/bld/$project/$lang/$lang.xis";

print "{\n";
print "\t\"type\": \"glossary\",\n";
print "\t\"project\": \"$project\",\n";
print "\t\"lang\": \"$lang\",\n";

my @in = `cat $cbd_ns | $ENV{'ORACC'}/bin/xns`;
my $cbd_nons = join('', @in);
my $xcbd = load_xml_string($cbd_nons);
ORACC::JSON::iterate($xcbd->getDocumentElement());
$xcbd = undef;
$cbd_nons = undef;
print "\n,\n";

ORACC::JSON::reset();

@in = `cat $xis_ns | $ENV{'ORACC'}/bin/xns`;
my $xis_nons = join('',@in);
my $xxis = load_xml_string($xis_nons);
ORACC::JSON::iterate($xxis->getDocumentElement());
$xxis = undef;

print "\n}\n";

###########################################################

sub
glossary_howtos {
    my %howto = ();

    $howto{'cbd_entries'} = { nam=>"entries", val=>'[' };
    $howto{'cbd_entry'} = { type=>"{", nam=>'headword',val=>'@n', att=>'-n' };
    $howto{'cbd_cf'} = { nam=>'cf',val=>'text()' };
    $howto{'cbd_gw'} = { nam=>'gw',val=>'text()' };
    $howto{'cbd_pos'} = { nam=>'pos',val=>'text()' };
    $howto{'cbd_epos'} = { nam=>'epos',val=>'text()' };
    $howto{'cbd_mng'} = { nam=>'mng',val=>'text()' };
    $howto{'cbd_forms'} = { nam=>'forms',val=>'[' };
    $howto{'cbd_form'} = { type=>"{",nam=>'type',val=>'form',att=>'' };
    $howto{'cbd_form-sanss'} = { nam=>'form-sanss',val=>'[' };
    $howto{'cbd_form-sans'} = { type=>"{",nam=>'type',val=>'form-sans',att=>'' };
    $howto{'cbd_t'} = { type=>'#ignore' };
    $howto{'cbd_cof-form-norm'} = { nam=>'cof-form-norm',val=>'text()',att=>'' };
    $howto{'cbd_norms'} = { nam=>'norms',val=>'[' };
    $howto{'cbd_norm'} = { type=>"{",nam=>'#ignore',val=>'#ignore',att=>'' };
    $howto{'cbd_n'} = { nam=>'n',val=>'text()',att=>'' };
    $howto{'cbd_f'} = { type=>"{",nam=>'type',val=>'normform',att=>'' };
    $howto{'cbd_bases'} = { nam=>'bases',val=>'[' };
    $howto{'cbd_base'} = { type=>"{",nam=>'type',val=>'base',att=>'' };
    $howto{'cbd_morphs'} = { nam=>'morphs',val=>'[' };
    $howto{'cbd_morph'} = { type=>"{",nam=>'type',val=>'morph',att=>'' };
    $howto{'cbd_morph2s'} = { nam=>'morph2s',val=>'[' };
    $howto{'cbd_morph2'} = { type=>"{",nam=>'type',val=>'morph2',att=>'' };
    $howto{'cbd_conts'} = { nam=>'conts',val=>'[' };
    $howto{'cbd_cont'} = { type=>"{",nam=>'type',val=>'cont',att=>'' };
    $howto{'cbd_compound'} = { nam=>'compound',val=>'[' };
    $howto{'cbd_cpd'} = { type=>"{",nam=>'type',val=>'cpd',att=>'' };
    $howto{'cbd_senses'} = { nam=>'senses',val=>'[' };
    $howto{'cbd_sense'} = { type=>"{",nam=>'type',val=>'sense',att=>'' };
    $howto{'cbd_sigs'} = { nam=>'sigs',val=>'[' };
    $howto{'cbd_sig'} = { type=>"{",nam=>'type',val=>'sig',att=>'' };
    $howto{'cbd_cof-data'} = { nam=>'cof-data',val=>'{',att=>'' };
    $howto{'cbd_cof-head'} = { nam=>'head',val=>'text()',att=>'0' };
    $howto{'cbd_cof-tail'} = { nam=>'tail',val=>'{',att=>'',text=>'sig' };

    $howto{'xis_xisses'} = { nam=>'instances', val=>'{' };
    $howto{'xis_xis'} = { nam=>'@xml:id', val=>"[", att=>'0' };
    $howto{'xis_r'} = { nam=>'#ignore', val=>'text()' };
    $howto{'xis_periods'} = { type=>'#ignore' };

    ORACC::JSON::setHowTos(%howto);
}
1;
