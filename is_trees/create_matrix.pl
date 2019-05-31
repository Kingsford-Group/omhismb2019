#!/usr/bin/perl
use strict;

my $fname = shift;

my %distances;
my %names;

open FILE, $fname or die("$fname: $!\n");
while(<FILE>){
  chomp $_;
  my @spl = split(/\s+/,$_);
  my $k = $spl[0] . "," . $spl[1];
  my $kp = $spl[1] . "," . $spl[0];
  next if $kp eq $k;
  $names{$spl[0]} = 1;
  $names{$spl[1]} = 1;
  print STDERR "Values not the same $k $kp $distances{$kp} $spl[2]\n" if defined $distances{$kp} && $distances{$kp} != (1.0 - $spl[2]);
  next if defined $distances{$kp};
  $distances{$k} = (1.0 - $spl[2]);
}

my @v = sort keys %names;

for(my $j=0; $j<scalar(@v); $j++){
  #print "\t$v[$j]";
}
print "\t".scalar(@v)."\n";
for(my $i=0; $i<scalar(@v); $i++){
  print $v[$i];
  for(my $j=0; $j<scalar(@v); $j++){
    my $value = (exists($distances{"$v[$i],$v[$j]"}))?$distances{"$v[$i],$v[$j]"}:$distances{"$v[$j],$v[$i]"};
    $value = 0 if $value eq "";
    print "\t$value";
  }
  print "\n";
}
