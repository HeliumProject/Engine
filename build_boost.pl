use strict;

use Cwd;
use Data::Dumper;
use File::Spec;
use File::Find;
use File::Path;
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
  
  my $target;
  if ( $ENV{PATH} =~ /VC\\BIN\\amd64;/i )
  {
    $target = "address-model=64";
  }
  elsif ( $ENV{PATH} =~ /VC\\BIN;/i )
  {
    $target = "";
  }
  else
  {
    die "Microsoft Visual Studio Tools cannot be found in your PATH";
  }

  Do( "bootstrap" );
  Do( "bjam -j$ENV{NUMBER_OF_PROCESSORS} $target" );

  my $target;
  if ( $ENV{PATH} =~ /VC\\BIN\\amd64;/i )
  {
    rmtree( "stage\\lib\\x64" );
    mkpath( "stage\\lib\\x64" );
    system( "move stage\\lib\\*.* stage\\lib\\x64" );
  }
  elsif ( $ENV{PATH} =~ /VC\\BIN;/i )
  {
    rmtree( "stage\\lib\\Win32" );
    mkpath( "stage\\lib\\Win32" );
    system( "move stage\\lib\\*.* stage\\lib\\Win32" );
  }
  else
  {
    die "Microsoft Visual Studio Tools cannot be found in your PATH";
  }
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
