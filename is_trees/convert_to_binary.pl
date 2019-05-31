#!/usr/bin/perl
use strict;

my $string = `cat shift`;
$string =~ s/\.[abcd]1/0/g;
$string =~ s/\.[abcd]2/1/g;
$string =~ s/s//g;
print $string;
