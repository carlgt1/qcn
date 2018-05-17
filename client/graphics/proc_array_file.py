#!/usr/bin/perl

my $fname = $ARGV[0];

open(ARYIN, "<$fname") or die;
open(ARYOUT, ">$fname.proc") or die;
my $line;

while(<ARYIN>)
{
   $line = $_;
   chomp $line;
   print ARYOUT "{ $line },\n";
}

close(ARYIN);
close(ARYOUT);

