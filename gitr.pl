#!/usr/bin/perl

use strict;

my $args;
foreach my $arg ( @ARGV )
{
	$args .= " \"" . $arg . "\"";
}

my $cmd = "git$args\n";
print( $cmd );
system( $cmd );

# nested calls need single quotes to delimit args
#  (double quotes escape the command)
$args =~ s/\"/'/g;

$cmd = "git submodule foreach --recursive \"git$args\"\n";
print( $cmd );
system( $cmd );
