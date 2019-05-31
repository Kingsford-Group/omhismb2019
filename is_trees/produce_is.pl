#!/usr/bin/perl
use strict;
use List::Util qw/shuffle/;

my $path = shift;
my $mut_rate = shift;

$mut_rate = 0 if $mut_rate eq "";

my @ISs = shuffle ("IS1","IS5","IS2","IS186"); #shuffle `sed "s/^\\(IS[0-9]*\\)\t.*/\\1/" /mnt/disk25/user/heewookl/ecoli/IS_postitions.txt | sort -u | grep "IS"`;
chomp @ISs;
print join(",", @ISs) . "\n";
#exit(0);

sub ins_and_child{
  my $fname = shift;
  my $cur_depth = shift;
  my $left = shift;
  return 0 if $left == 0;
  my $c = chr(ord('a') + $cur_depth);
  my $source_fname = "$path/sequences/$fname.fa";
  $source_fname = "NC_000913.fasta" if $fname eq "s";
  system("perl insert_is.pl $source_fname $ISs[$cur_depth] $fname.${c}1 $mut_rate > $path/sequences/$fname.${c}1.fa");
  system("perl insert_is.pl $source_fname $ISs[$cur_depth] $fname.${c}2 $mut_rate > $path/sequences/$fname.${c}2.fa");
  ins_and_child("$fname.${c}1",$cur_depth+1,$left-1);
  ins_and_child("$fname.${c}2",$cur_depth+1,$left-1);
}

system("mkdir -p $path/sequences/");
ins_and_child("s",0,scalar(@ISs));
