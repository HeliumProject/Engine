#!/usr/bin/perl -w
use strict;

use Cwd;
use File::Path;
use File::Spec;
use File::Basename;
use Data::Dumper;
use Getopt::Long;

use lib (dirname $0);
use BuildConsole;
use BuildTimer;

my $g_Config = 'Debug';
my $g_Clean = 0;
my $g_Verbose = 0;
my $g_Incred = 0;
my $g_NoPrompt = 0;
my $g_Architecture = ( $ENV{ PATH } =~ /x64/ ? 'x64' : 'Win32' );

my @supportedConfigs = ( 'Debug', 'Intermediate', 'Profile', 'Release' ); 

# look in their path for the incredibuild executable, use it if they have it, can still be disabled using -noincred
map { $g_Incred = 1 if ( -x File::Spec->catfile( $_, 'BuildConsole.exe' ) ) } split( ';', $ENV{PATH} );

# if you add an option, please add it to PrintUsage as well.... 
my $getHelp = 0;
my $optionsOk = GetOptions( "config=s"  => \$g_Config,
                            "clean"     => \$g_Clean, 
                            "verbose"   => \$g_Verbose, 
                            "incred!"   => \$g_Incred,
                            "no_prompt" => \$g_NoPrompt,
                            "arch=s"    => \$g_Architecture,
                            "h"         => \$getHelp,
                            "help"      => \$getHelp,
                            "usage"     => \$getHelp );

sub PrintUsage()
{
  print STDERR "\nUsage: build < preset | solution file path >\n"; 
  print STDERR "\nOptions:\n"; 
  print STDERR "   -config <build config>   : One of the support configs\n";
  print STDERR "   -clean                   : Clean the output, then build\n"; 
  print STDERR "   -verbose                 : Display the build log inline\n"; 
  print STDERR "   -incred / -noincred      : Force or ignore Incredibuild\n"; 
  print STDERR "   -no_prompt               : Do not prompt to open log if the build fails\n"; 
  print STDERR "   -sln <comma delimited>   : Build specific solutions from a preset\n"; 
  print STDERR "\nSupported Configurations:\n\n"; 
  print STDERR join( "\n", @supportedConfigs ) . "\n";
}

if ( !$optionsOk || $getHelp )
{
  PrintUsage(); 
  exit 1; 
}


#---------------------------------------------------------------------
# shift the arg from the command line
#

my $arg = shift;

if ( !defined $arg )
{
  PrintUsage();
  exit 1;
}

#---------------------------------------------------------------------
# use the arg to see what to do
#

my $preset = undef;
my $presetFile = undef;
my $solution = undef;

if ( -e $arg && -f $arg )
{
  $solution = File::Spec->rel2abs( $arg );
}
else
{
  $preset = $arg;
  $presetFile = "build_preset_$preset.txt";
}

#---------------------------------------------------------------------
# validate the configuration to build
# 

if ( !grep( /$g_Config/i, @supportedConfigs ) )
{
  Console::Error( "Config '$g_Config' is not supported\n" );
  PrintUsage(); 
  exit 1; 
}

Console::Print( "\n" );
Console::Print( "Architecture : $g_Architecture\n" );
Console::Print( "Incredibuild : " . ( $g_Incred ? 'Enabled' : 'Disabled' ) . "\n" );

#---------------------------------------------------------------------
# get the list of solutions
# 

my @solutions;

if ( defined( $solution ) )
{
  push( @solutions, $solution );
}
else
{
  unless ( open IN, "<" . $presetFile )
  {
    Console::Error( "Failed to read config file: $presetFile\n" );
    exit 1;
  }
  
  while ( my $line = <IN> )
  {
    chomp( $line );
    push( @solutions, _ExpandEnvironmentStrings( $line ) );
  }

  close IN;
}

my @logBaseNames;

_DoBuild( \@solutions, "Clean", \@logBaseNames ) if $g_Clean;

my $result = _DoBuild( \@solutions, "Build", \@logBaseNames );

_ProcessLogs(\@logBaseNames);

exit( $result );


#---------------------------------------------------------------------
# Build master log file
# 
sub _ProcessLogs
{
  my $logBaseNames = shift;
  
  my $logDir = File::Spec->catfile( "Logs" );
  mkpath $logDir;

  foreach my $file (@{$logBaseNames})
  {
    if ( open IN, "<$file.log" )
    {
      my $log = File::Spec->catfile( $logDir, basename( $0, '.pl' ) . ".log" );
      open( LOG, ">$log" ) or die ( $! );
      while ( <IN> ) 
      {
        print LOG $_;
      }
      close( LOG );
      close( IN );
    } 

    if ( open IN, "<$file" . "_errors.log" )
    {
      my $log = File::Spec->catfile( $logDir, basename( $0, '.pl' ) . "errors.log" );
      open( LOG, ">$log" ) or die ( $! );
      while ( <IN> ) 
      {
        print LOG $_;
      }
      close( LOG );
      close( IN );
    }  

    if ( open IN, "<$file" . "_warnings.log" )
    {
      my $log = File::Spec->catfile( $logDir, basename( $0, '.pl' ) . "warnings.log" );
      open( LOG, ">$log" ) or die ( $! );
      while ( <IN> ) 
      {
        print LOG $_;
      }
      close( LOG );
      close( IN );
    }  
  }
}


#---------------------------------------------------------------------
# Build the solutions
# 
sub _DoBuild
{
  my $result = 0;

  my $solutions = shift;
  my $command = shift;
  my $logBaseNames = shift;

  my $buildTimeAccumulator = Accumulator->new();

  {
    my $st = ScopeTimer->new( \$buildTimeAccumulator );

    my $logDir = File::Spec->catfile( "Logs" );
    mkpath $logDir;

    foreach my $solution ( @{$solutions} )
    {
      # normally we default to incred
      my $buildType = $g_Incred ? 'Incred' : 'MSBuild';
  
      # arch
      my $buildArch = $g_Architecture;

      # get the solution name, parse out and strip optional build config override
      my $solutionPath = $solution;
      
      # check for overridden build configuration for this solution
      if ($solutionPath =~ /\|/)
      {
      
        ( $solutionPath, $buildType, $buildArch ) = split( /\|/, $solutionPath );

        # turn off incredibuild if we don't have it selected      
        if (!($g_Incred) && $buildType eq "Incred")
        {
          $buildType = "MSBuild";
        }
      }
      
      my $solutionName = basename( $solutionPath, ".sln" ); # strip the dir and extension off of the path to make the name
      my $solutionBuildTimeAccumulator = Accumulator->new();

      {
        my $st = ScopeTimer->new( \$solutionBuildTimeAccumulator );
        my $commandLine = undef;

        if ($buildType eq "Incred")
        {
          if ($command eq "Build")
          {
            $commandLine = "BuildConsole $solutionPath /CFG=\"$g_Config|$buildArch\" /ALL /WAIT";
          }
          else
          {
            $commandLine = "BuildConsole $solutionPath /CFG=\"$g_Config|$buildArch\" /CLEAN /WAIT";
          }      
        }
        else
        {
          $commandLine = "\"$ENV{VSINSTALLDIR}\\Common7\\IDE\\devenv.com\" \"$solutionPath\" \/$command \"$g_Config|$buildArch\"";
        }

        Console::Print( "\n o ${command}ing $solutionName...\n" );

        if ( $g_Verbose )
        {
          Console::Print( "  - Command: $commandLine\n" );
        }

        my $cleanConfig = $g_Config;
        $cleanConfig =~ s/ /_/g;
        my $logBaseName = File::Spec->catfile( $logDir, "${command}_${solutionName}_${buildArch}_${buildType}_${cleanConfig}" );
        push( @{ logBaseNames }, $logBaseName );
        Console::Print( "  - Logs: $logBaseName\n" );

        my $logFile = "$logBaseName.log";
        my $errorsFile = "$logBaseName" . "_errors.log";
        my $warningsFile = "$logBaseName" . "_warnings.log";

        open( LOG, ">$logFile" ) or die ( $! );
        open( ERRORS, ">$errorsFile" ) or die ( $! );
        open( WARNINGS, ">$warningsFile" ) or die ( $! );
        open( BUILD, "$commandLine 2>&1 |" ) or die ( $! );

        my $buildInformation;
        while ( my $line = <BUILD> )
        {
          print LOG $line;

          if ($g_Verbose)
          {
            print $line;
          }
          else
          {
            # handle the funky newlines
            chomp $line;

            if ( $line =~ /^------ Build started: Project: (.+), Configuration: .+ .+ ------/ )
            {
              print ( "  - Building $1\n" );
            }
            elsif ( $line =~ /^--------------------Configuration: (.+) - .+\|.+-+/ )
            {
              print ( "  - Building $1\n" );
            }
            else
            {
              #
              # Visual Studio
              #
              if (!($line =~ /: TODO /i))
              {
                if ( $line =~ /: fatal error (C|LNK)[0-9]+/i)
                {
                  print ERRORS "$line\n";
                  Console::PrintColor( 'red', "$line\n" );
                }
                if ( $line =~ /: error (C|LNK)[0-9]+/i)
                {
                  print ERRORS "$line\n";
                  Console::PrintColor( 'red', "$line\n" );
                }
                if ( $line =~ /: warning (C|LNK)[0-9]+/i)
                {
                  print WARNINGS "$line\n";
                  Console::PrintColor( 'yellow', "$line\n" );
                }
              }
              if ( $line =~ /Build: (.*)/ )
              {
                $buildInformation = $1;
                $buildInformation =~ s/=+$//; # eat trailing equals from MSBuild output
              }

              #
              # Incred
              #
              
              if ( $line =~ /[0-9]+ build system error\(s\)/ )
              {
                print ERRORS "$line\n";
                Console::PrintColor( 'red', "$line\n" );
              }             

              #
              # VSI
              #
              if ( $line =~ /: error: / )
              {
                print ERRORS "$line\n";
                Console::PrintColor( 'red', "$line\n" );
              }
              if ( $line =~ /(.*): error: / ) # vsi build error
              {
                print ERRORS "$line\n";
                Console::PrintColor( 'red', "$line\n" );
              }
              if ( $line =~ /\([0-9]+\): error:/ ) # vsi gcc error
              {
                print ERRORS "$line\n";
                Console::PrintColor( 'red', "$line\n" );
              }
              if ( $line =~ /\([0-9]+\): warning:/ ) # vsi gcc warning
              {
                print WARNINGS "$line\n";
                Console::PrintColor( 'yellow', "$line\n" );
              }
              if ( $line =~ /\([0-9]+,[0-9]+\): error [0-9]+:/ ) # vsi snc error
              {
                print ERRORS "$line\n";
                Console::PrintColor( 'red', "$line\n" );
              }
              if ( $line =~ /\([0-9]+,[0-9]+\): warning [0-9]+:/ ) # vsi snc warning
              {
                print WARNINGS "$line\n";
                Console::PrintColor( 'yellow', "$line\n" );
              }
            }
          }
        }
        
        close ( BUILD );

        if ( $result == 0 )
        {
          $result = $? >> 8;
        }

        close ( LOG );
        close ( ERRORS );
        close ( WARNINGS );

        if (!$g_Verbose && defined $buildInformation)
        {
          Console::Print( "  - $buildInformation\n" );
        }
      }

      # useful information
      Console::Print( "  - Took %s\n", $solutionBuildTimeAccumulator->AsString() );
    }
  }
  
  # useful information
  Console::Print( "\nTook %s\n", $buildTimeAccumulator->AsString() );
  
  return $result;
}


#---------------------------------------------------------------------
# Process nested environment vars
# 
sub _ExpandEnvironmentStrings
{
  my $string = shift;
  
  while ( $string =~ /\%(.*?)\%/ )
  {
    my $envVarName = $1;

    if ( defined $ENV{ $envVarName } )
    {
      $string =~ s/\%\Q$envVarName\E\%/$ENV{ $envVarName }/g;
    }
    else
    {
      Console::Error("$envVarName is not defined in your environment\n");
      exit( 1 );
    }
  }
  
  return $string;
}
