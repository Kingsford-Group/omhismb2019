#!/usr/bin/perl
use strict;

my $search_foler = shift;
my @files = `ls $search_folder/*/sequences/is.*d*`;

foreach my $f(@files){
  chomp $f;
  my $folder = $f;
  $folder =~ s/sequences.*//;
  for my $m(1000, 500, 100){
    for my $l(1,3){
      system("mkdir -p $folder/sketches/m${m}_l$l");
      print "cd $folder/sketches/m${m}_l$l ; ../omh_compute/omh_sketch --seedin ../omh_compute/seed -m $m -k 22 -l $l $f\n";  
    }
  }
}
