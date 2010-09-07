use strict;

use File::Spec;
use File::Basename;
use Getopt::Long;

# flags
my $help      = 0;
my $clean     = 0;
my $git_clean = 0;
my $verbose   = 0;
my $config    = '';

# result
my $result = 0;

if ( !GetOptions( "clean"      => \$clean,
                  "git_clean"  => \$git_clean,
                  "verbose"    => \$verbose,
                  "c|config=s" => \$config,
                  "h|help"     => \$help,
                  "usage"      => \$help ) || $help )
{
  print( "\nUpdate.pl - Update your stuff\n" );
  print( "\nOptions:\n" );
  print( "\t-clean: passes -clean to each build step.\n" );
  print( "\t-git_clean: cleans your git workspace \n\t  -NOTE: This deletes all files not under revision control.\n" );
  print( "\t-verbose: passes -verbose to each build step.\n" );
  print( "\t-config <config name>: override default build config.\n" );
  exit( 1 );
}

my $git_command;
if ( $git_clean )
{
  my $git_command = 'git clean -fdx';
  $result = _Do( $git_command, "Git Clean" );
}
$git_command = 'git pull';
$result += _Do( $git_command, "Git Pull" );

if ( $result )
{
  print("\nError during Git commands - Aborting script.\n");
  exit( 1 );
}


my $dep_command    = 'perl.exe ' . File::Spec->catfile( dirname( $0 ), "build.pl Dependencies.sln" );
my $helium_command = 'perl.exe ' . File::Spec->catfile( dirname( $0 ), "build.pl Helium.sln" );

if($config)
{
  $dep_command    .= " -config \"$config\"";
  $helium_command .= " -config \"$config\"";
}
if($clean)
{
  $dep_command    .= ' -clean';
  $helium_command .= ' -clean';
}
if($verbose)
{
  $dep_command    .= ' -verbose';
  $helium_command .= ' -verbose';
}

$result += _Do( $dep_command, "Build Dependencies" );
$result += _Do( $helium_command, "Build Helium" );

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
