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
Usage: $scriptName -v <VERSION> [-o|-options <OPTIONS>]
};
}

my $version = undef;
my $options = undef;

my $result = GetOptions (
  "version=s" => \$version,
  "options=s" => \$options);

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

BuildConfig( 0 );
BuildConfig( 1 );

sub BuildConfig
{
  my $static = shift;
  my $shared = $static ? 0 : 1 ;

  print STDOUT ("\nBuilding with options: SHARED=$shared\n\n");

  my $path = "SDK\\wxWidgets\\$version";

  chdir "$path\\build\\msw";

  Build( "BUILD=debug SHARED=$shared MONOLITHIC=1 DEBUG_INFO=1 $options" );
  Build( "BUILD=release SHARED=$shared MONOLITHIC=1 DEBUG_INFO=1 $options" );
}

sub Build
{
  my $args = shift; 
  my $command = "nmake.exe -f makefile.vc $args";
  print( $command . "\n" );
  system( $command );
  
  my $result = $? >> 8;
  if ( $result != 0 )
  {
    print( "$command failed with error $result" );
    exit 1;
  }
}
