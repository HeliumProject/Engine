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
  my $path = "SDK\\wxWidgets\\$version";

  chdir "$path\\build\\msw";
  
  my $target;
  if ( $ENV{PATH} =~ /VC\\BIN\\amd64;/i )
  {
    $target = "TARGET_CPU=AMD64";
  }
  elsif ( $ENV{PATH} =~ /VC\\BIN;/i )
  {
    $target = "";
  }
  else
  {
    die "Microsoft Visual Studio Tools cannot be found in your PATH";
  }

  Build( "BUILD=debug SHARED=0 UNICODE=0 DEBUG_INFO=1 $target" );
  Build( "BUILD=release SHARED=0 UNICODE=0 DEBUG_INFO=1 $target" );
  Build( "BUILD=debug SHARED=0 UNICODE=1 DEBUG_INFO=1 $target" );
  Build( "BUILD=release SHARED=0 UNICODE=1 DEBUG_INFO=1 $target" );
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
