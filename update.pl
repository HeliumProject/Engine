use strict;
use Cwd;
use File::Spec;
use File::Basename;
use Getopt::Long;

# flags
my $autobuild = 0;
my $pull      = 0;
my $clean     = 0;
my $git_clean = 0;
my $verbose   = 0;
my $config    = '';
my $help      = 0;

# result
my $result = 0;

if ( !GetOptions( "autobuild"  => \$autobuild,
                  "pull"       => \$pull,
                  "clean"      => \$clean,
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

if ( $autobuild ) # dont mess with source control status if its an autobuild
{
  $pull = 0;
}

my $git_command;

if( $git_clean )
{
  $git_command = 'clean -fdx';
  $result = _DoGit( $git_command, "Git Clean" );
}

if( $pull )
{
  $git_command = 'pull';
  $result += _DoGit( $git_command, "Git Pull" );
}

my $premake = 'premake4 vs2008';
my $depends = 'perl "' . File::Spec->catfile( dirname( $0 ), "build.pl" ) . '" "' . File::Spec->catfile( getcwd(), "Premake", "Dependencies.sln" ) . '"';
my $compile = 'perl "' . File::Spec->catfile( dirname( $0 ), "build.pl" ) . '" "' . File::Spec->catfile( getcwd(), "Premake", basename( getcwd() ) . ".sln" ) . '"';

if($config)
{
  $depends .= " -config \"$config\"";
  $compile .= " -config \"$config\"";
}
if($clean)
{
  $depends .= ' -clean';
  $compile .= ' -clean';
}
if($verbose)
{
  $depends .= ' -verbose';
  $compile .= ' -verbose';
}

$result += _Do( $premake, "Running Premake" );
$result += _Do( $depends, "Build Dependencies" );
$result += _Do( $compile, "Build " . basename( getcwd() ) );

if ( $result )
{
  print("\n$result commands failed during update!\n");
  exit( 1 );
}

sub _DoGit
{
  my $args = shift;
  my $description = shift; 
  my $command = 'perl "' . File::Spec->catfile( dirname( $0 ), "gitr.pl" ) . '" "' . $args;
  _Do( $command, $description );

  if ( $result )
  {
    my $prompt = _PromptYesNo("\nError during Git commands, continue (y/n)? ");
    if ( !$prompt )
    {
      print("\nError during Git commands - Aborting script.\n");
      exit( 1 );
    }
  }
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
    print("$description failed with result $result:\n> $compile\n");
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
