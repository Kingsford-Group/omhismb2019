#!/usr/bin/perl
use strict;

my $fname = shift; 
open FILE, $fname or die("$fname: $!\n");

my $ofile = shift;
open OFILE, ">$ofile" or die("$ofile: $!\n");

my $k = shift;
die("k is not set in run_omh_and_combine.pl\n") if $k eq "";

while(<FILE>){
  chomp $_; 
  my @spl = split(/\s+/,$_);
  my $s1 = $spl[0];
  my $s2 = $spl[1];
  $s1 =~ tr/0123/actg/;
  $s2 =~ tr/0123/actg/;
  $s1 =~ tr/ED/TG/;
  $s2 =~ tr/ED/TG/;
  my $omh = `../omh_compue/omh_compare_sequences -k $k -l 2 -m 500 -s ../omh_compute/seed $s1 $s2`;
  chomp $omh;
  print OFILE "$spl[2] $spl[5] $spl[6] $omh $spl[7]\n";
}
close OFILE;
