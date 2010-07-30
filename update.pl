use strict;

use File::Spec;
use File::Basename;
use Getopt::Long;

# flags
my $help = 0;

# result
my $result = 0;

if ( !GetOptions( "h"         => \$help,
                  "help"      => \$help,
                  "usage"     => \$help ) || $help )
{
  print( "\nUpdate.pl - Update your stuff\n" );
  print( "\nOptions:\n" );
  exit( 1 );
}

$result += _Do("git pull", "Git Pull");

$result += _Do("perl.exe " . File::Spec->catfile( dirname( $0 ), "build.pl Dependencies.sln" ), "Build Dependencies");

$result += _Do("perl.exe " . File::Spec->catfile( dirname( $0 ), "build.pl Helium.sln" ), "Build Luna");

if ( $result )
{
  print("\n$result commands failed during update!\n");
  exit( 1 );
}

sub _Do
{
  my $command = shift;
  my $description = shift;
  
  if (defined $description)
  {
    print("\n o $description\n");
  }
  else
  {
    $description = "Command";
  }
  
  system ("$command");
  my $result = $? >> 8;

  if ( $result )
  {
    print("$description failed with result $result:\n> $command\n");
  }

  return $result > 0 ? 1 : 0;
}

sub _Prompt
{
  my $message = shift;

  print "$message> ";
    
  my $input = <>;
  chomp( $input );
    
  return $input;
}
  
sub _PromptYesNo
{
  my $message = shift;
  
  while ( 1 )
  {
    my $input = _Prompt( $message );
    
    if ( length( $input ) )
    {
      if ( $input =~ /^y/i )
      {
        return 1;
      }
      elsif( $input =~ /^n/i )
      {
        return 0;
      }
      else
      {
        print "Unknown response, please enter 'y' or 'n'.\n\n";
      }
    }
    else # no input, so return -1
    {
      return -1;
    }
  }
}
