#!/usr/bin/perl
use strict;
use warnings;

my $target_fname = shift;
my $insert_name = shift;
my $new_name = shift;
my $mut_rate = shift;

$mut_rate = 0 if $mut_rate eq "";

open IF, "IS_all.fa" or die("IS_all.fa: $!\n");
my $next_line = <IF>;
chomp $next_line;
while(!($next_line =~ /^>$insert_name/)){
  die("Reached end of file without finding $insert_name (1)\n") if eof(IF);
  #print "$next_line\n" if $next_line =~ /^>/;
  $next_line = <IF>;
  chomp $next_line;
}
my $insert_seq = "";
$next_line = <IF>;
chomp $next_line;
while(!eof(IF) && !($next_line =~ /^>/)){
  die("Reached end of file without finding $insert_name\n") if eof(IF);
  $insert_seq .= $next_line;
  $next_line = <IF>;
  chomp $next_line;
}
close IF;

open TF, $target_fname or die("$target_fname: $!\n");
my $target_name = <TF>;
chomp $target_name;
$target_name =~ s/^>//;
my $target_seq = "";
while(<TF>){
  chomp $_;
  $target_seq .= $_;
}

my @list = `grep "^$insert_name\t" IS_postitions.txt | sed "s/.*\\s\\([0-9]*\\)\$/\\1/" | sort -u`;
chomp @list;
my $insert_loc = $list[int(rand(scalar(@list)))];

#my $insert_loc = int(rand(length($target_seq)));
#print STDERR "$new_name $target_name $insert_name $insert_loc\n";

print ">$new_name\n";
my $new_seq = substr($target_seq,0,$insert_loc) . $insert_seq . substr($target_seq,$insert_loc);

my $num_mut = int($mut_rate * length($new_seq));
for(1...$num_mut){
  my $mut_loc = int(rand(length($new_seq)));
  my $mut_char = substr($new_seq, int(rand(length($new_seq))),1);
  substr($new_seq, $mut_loc, 1) =~ s/./$mut_char/;
}
print "$new_seq\n";
