#!/usr/bin/perl
use strict;

my $folder = shift;
foreach my $m(1000,500,100){
  foreach my $l(1,3){
    system ("../omh_compute/omh_compare_sketches -t 0 \"$folder/sketches/m${m}_l${l}/*d*\" \"$folder/sketches/m${m}_l$l/*d*\" > $folder/sketches/m${m}_l$l.leaf.out ; ");
    system ("./create_matrix.pl $folder/sketches/m${m}_l${l}.leaf.out > $folder/sketches/m${m}_l${l}.leaf.matrix ; ");
    system ("rapidnj -i pd $folder/sketches/m${m}_l${l}.leaf.matrix > $folder/sketches/m${m}_l${l}.leaf.newik\n");
    system ("./convert_to_binary.pl $folder/sketches/m${m}_l${l}.leaf.newick > $folder/sketches/m${m}_l${l}.binary.leaf.newik\n");
  }
}
