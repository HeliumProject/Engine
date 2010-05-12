use strict;

use Cwd;
use Data::Dumper;
use File::Spec;
use File::Find;
use Getopt::Long;

sub PrintUsage
{
  # get just the binary name, not the full path to it
  my $scriptName = $0;
  $scriptName =~ s/^.*\\(.*?)$/$1/;
  
  print qq{
Usage: $scriptName -v <VERSION>
};
}

my $version = undef;
my $options = undef;

my $result = GetOptions ("version=s" => \$version);

if ( !$result )
{
  PrintUsage();
  exit 1;
}

if ( !defined $version )
{
  print( "Please specify the version to build\n" );
  PrintUsage();
  exit 1;
}

BuildConfig();

sub BuildConfig
{
  print STDOUT ("\nBuilding\n\n");

  my $path = "SDK\\boost\\$version";

  chdir "$path";

  Do( "bootstrap" );
  Do( "bjam" );
}

sub Do
{
  my $command = shift; 
  print( $command . "\n" );
  system( $command );
  
  my $result = $? >> 8;
  if ( $result != 0 )
  {
    print( "$command failed with error $result" );
    exit 1;
  }
}
