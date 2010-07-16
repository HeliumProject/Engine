use strict;

use Cwd;
use Data::Dumper;
use File::Basename;
use File::Find;
use File::Path;
use File::Spec;
use Getopt::Long;

my $verbose = 0;

###############################################################################
sub PrintUsage
{
  # get just the binary name, not the full path to it
  my $scriptName = $0;
  $scriptName =~ s/^.*\\(.*?)$/$1/;
  
  print qq{
  
Usage: $scriptName <file> <lite-sql.exe path>

Examples:
    $scriptName "Luna\\Vault\\AssetCacheDB.xml"
    $scriptName "Luna\\Vault\\AssetCacheDB.xml" "SDK\\LiteSQL\\0.3.8_prebuilt\\bin\\litesql-gen.exe"
};
}
###############################################################################


my $xmlFilename = shift @ARGV;
if ( not defined $xmlFilename )
{
  print "Please pass in LiteSQL config file.\n";
  PrintUsage();
  exit 1;
}
if ( not -e $xmlFilename )
{
  print "Can't find LiteSQL config file at: $xmlFilename.\n";
  PrintUsage();
  exit 1;
}
$xmlFilename = File::Spec->rel2abs( $xmlFilename );

my $liteSQLPath = shift @ARGV;
if ( not defined $liteSQLPath )
{
  $liteSQLPath = File::Spec->rel2abs( "SDK\\LiteSQL\\0.3.8_prebuilt\\bin\\litesql-gen.exe" );
}
if ( not -e $liteSQLPath )
{
  print "Can't find litesql-gen.exe at: $liteSQLPath.\n";
  PrintUsage();
  exit 1;
}
$liteSQLPath = File::Spec->rel2abs( $liteSQLPath );

my($dbName, $outFolder, $suffix) = fileparse( $xmlFilename, qr/\.[^.]*/ );

my $lowDBName = $dbName;
$lowDBName =~ tr/[A-Z]/[a-z]/;

my $inFolder = dirname( $liteSQLPath );

my $inCppFile = File::Spec->catfile( $inFolder, "$lowDBName.cpp" );
my $outCppFile = File::Spec->catfile( $outFolder, "$dbName.cpp" );
my $inHFile = File::Spec->catfile( $inFolder, "$lowDBName.hpp" );
my $outHFile = File::Spec->catfile( $outFolder, "$dbName.h");

if ( $verbose )
{
	print "\n";
	print "dbName:    $dbName\n";
	print "lowDBName: $lowDBName\n";
	print "inFolder:  $inFolder\n";
	print "outFolder: $outFolder\n";
	print "suffix:    $suffix\n";
}

unlink( $inCppFile ) if -e $inCppFile;
unlink( $inHFile ) if -e $inHFile;

chdir "$inFolder";

DoSystemCommand( "\"$liteSQLPath\" -t c++ \"$xmlFilename\"" );
CleanFile( $inHFile, $outHFile );
CleanFile( $inCppFile, $outCppFile );

exit 0;

###############################################################################
sub CleanFile
{
	my $inFile = shift;
	my $outFile = shift;

	open( IN, "<$inFile" ) or die $!;
	my @file = <IN>;
	chomp @file;
	close IN;
	
	my($inFilename, $inFolder, $inSuffix) = fileparse( $inFile, qr/\.[^.]*/ );
	my($outFilename, $outFolder, $outSuffix) = fileparse( $outFile, qr/\.[^.]*/ );

	if ( $verbose )
	{
		print "\n";
		print "inSuffix:  $inSuffix\n";
		print "outSuffix: $outSuffix\n";
		print "in:        $inFilename$inSuffix\n";
		print "out:       $outFilename$outSuffix\n";
		print "\n";
		print "inFile:      $inFile\n";
		print "\n";
		print "outFile:     $outFile\n";
    }
	
	foreach my $line ( @file )
	{
		chomp $line;

		#Fixup includes to header hpp file
		$line =~ s/$inFilename\.hpp/$outFilename.h/img;
		next if( $line =~ /\#include/ );

		# Quote string literals
		$line =~ s/("(?:[^"\\]+|\\.)*")/ TXT( \1 )/g;
	  
	    # Fixup string vars
		$line =~ s/([^\w]+)char(?=\s*\*)/\1tchar/mg;
		
		$line =~ s/std\:\:string/tstring/mg;
		
		$line =~ s/std\:\:istream/tistream/mg;
		$line =~ s/std\:\:ostream/tostream/mg;
		$line =~ s/std\:\:iostream/tiostream/mg;
		
		$line =~ s/std\:\:ifstream/tifstream/mg;
		$line =~ s/std\:\:ofstream/tofstream/mg;
		$line =~ s/std\:\:fstream/tfstream/mg;
		
		$line =~ s/std\:\:istringstream/tistringstream/mg;
		$line =~ s/std\:\:ostringstream/tostringstream/mg;
		$line =~ s/std\:\:stringstream/tstringstream/mg;
	}

	my $fileContents = "";

	if ( $inSuffix =~ /\.cpp/i )
	{
	  $fileContents .="#include \"Precompile.h\"\n\n";
	}
	elsif ( $inSuffix =~ /\.h(?:pp){0,1}/i )
	{
	  $fileContents .="#pragma once\n\n#include \"Platform/Types.h\"\n\n";
	}
	else
	{
	  die "What kind of file is this?";
	}

	$fileContents .= join( "\n", @file );


	
	
	
	unlink ( $outFile ) if -e $outFile;
	open( OUT, ">$outFile" ) or die $!;
	print OUT $fileContents;
	
	unlink( $inFile ) if -e $inFile;
}

###############################################################################
sub DoSystemCommand
{
  my $command = shift; 
  if( $verbose )
  {
	print( $command . "\n" );
  }
  system( $command );
  
  my $result = $? >> 8;
  if ( $result != 0 )
  {
    print( "$command failed with error $result" );
    exit 1;
  }
}