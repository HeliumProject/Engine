use strict;

use Cwd;
use Data::Dumper;
use File::Basename;
use File::Copy;
use File::Find;
use File::Path;
use File::Spec;
use Getopt::Long;
use Time::Local;

my $verbose = 0;
my $undo = 0;

  
my @replaceSTDFuncs = (
  "endl",
  "less",
  "logic_error",
  "make_pair",
  "sort",
);

my @replaceSTDContainers = (
  "map",
  "pair",
  "set",
  "vector",
  "auto_ptr",
);

my $replaceLiteSQLHash = {
  "char(?=\\s*\\*)" => "LITESQL_Char",
  "(?:std\\:\\:){0,1}string" => "LITESQL_String",
  "(?:std\\:\\:){0,1}istream" => "LITESQL_iStream",
  "(?:std\\:\\:){0,1}ostream" => "LITESQL_oStream",
  "(?:std\\:\\:){0,1}iostream" => "LITESQL_ioStream",
  "(?:std\\:\\:){0,1}ifstream" => "LITESQL_ifStream",
  "(?:std\\:\\:){0,1}ofstream" => "LITESQL_ofSstream",
  "(?:std\\:\\:){0,1}fstream" => "LITESQL_fSstream",
  "(?:std\\:\\:){0,1}istringstream" => "LITESQL_iStringstream",
  "(?:std\\:\\:){0,1}ostringstream" => "LITESQL_oStringstream",
  "(?:std\\:\\:){0,1}stringstream" => "LITESQL_Stringstream",
  "cin" => "LITESQL_cin",
  "cout" => "LITESQL_cout",
  "cerr" => "LITESQL_cerr",
  "clog" => "LITESQL_clog",
  "strdup" => "_tcsdup",
  "strstr" => "_tcsstr",
  "strtol" => "_tcstol",
  "fprintf" => "_ftprintf",
  "fopen" => "_tfopen",
};

my $replaceHash = {
  "char(?=\\s*\\*)" => "tchar",
  "(?:std\\:\\:){0,1}string" => "tstring",
  "(?:std\\:\\:){0,1}istream" => "tistream",
  "(?:std\\:\\:){0,1}ostream" => "tostream",
  "(?:std\\:\\:){0,1}iostream" => "tiostream",
  "(?:std\\:\\:){0,1}ifstream" => "tifstream",
  "(?:std\\:\\:){0,1}ofstream" => "tofstream",
  "(?:std\\:\\:){0,1}fstream" => "tfstream",
  "(?:std\\:\\:){0,1}istringstream" => "tistringstream",
  "(?:std\\:\\:){0,1}ostringstream" => "tostringstream",
  "(?:std\\:\\:){0,1}stringstream" => "tstringstream",
  "strdup" => "_tcsdup",
  "strstr" => "_tcsstr",
  "strtol" => "_tcstol",
  "fprintf" => "_ftprintf",
  "fopen" => "_tfopen",
  "strcmp" => "_tcscmp",
  "strtoll" => "_tcstol",
  "strtod" => "_tcstod",
  "strlen" => "_tcslen",
  "perror" => "_tperror",
};


my $headerFile = "litesql_char.hpp";
my $stringLiteralMacro = "LITESQL_L";
my $replaceHashRef = $replaceLiteSQLHash;

my @skipFiles = (
  "md5.cpp",
  "md5.hpp",
  "config.h",
  "sqlite3.c",
  "sqlite3.h",
  "sqlite3.hpp",
  "sqlite3_backend.cpp",
  "sqlite3ext.c",
  $headerFile,
);

###############################################################################
sub PrintUsage
{
  # get just the binary name, not the full path to it
  my $scriptName = $0;
  $scriptName =~ s/^.*\\(.*?)$/$1/;
  
  print qq{
  
Usage: $scriptName <file>|<folder> [<out file>|<out folder>]

Examples:
    $scriptName "SDK\\LiteSQL\\0.3.8\\src - Copy" "SDK\\LiteSQL\\0.3.8\\src"
    $scriptName "SDK\\LiteSQL\\0.3.8\\src\\generator\\xmlparser.cpp"
};
}
###############################################################################


my $inputPath = shift @ARGV;
if ( not defined $inputPath )
{
	print "Please pass in file or folder path.\n";
	PrintUsage();
	exit 1;
}

$inputPath = File::Spec->rel2abs( $inputPath );
if ( not -e $inputPath )
{
	print "Can't find file at: $inputPath.\n";
	PrintUsage();
	exit 1;
}

my $outputPath = shift @ARGV;
if ( not defined $outputPath )
{
	$outputPath = $inputPath;
}
$outputPath = File::Spec->rel2abs( $outputPath );

if ( -d $inputPath )
{
	if ( not -e $outputPath )
	{
    mkdir( $outputPath );
	}
	elsif( not -d $outputPath )
	{
		print "Output path must ALSO be a folder.\n";
		PrintUsage();
		exit 1;
	}

  if( "$inputPath" eq "$outputPath" )
	{
		print "Will not overwrite input files.\n";
		PrintUsage();
		exit 1;
	}
  
  print "IN: $inputPath\n";
  print "OUT: $outputPath\n";
  print "---------------------\n";
	find( \&FindSourceFiles, $inputPath );
}
else
{
	CleanFile( $inputPath, $outputPath );
}

exit 0;

###############################################################################
sub FindSourceFiles
{
  if ( ( /\.cpp$/ ) or ( /\.hpp$/ ) or ( /\.h$/ ) )
  {
    my $filepath = File::Spec->canonpath( $File::Find::name );
        
    print "\nIN: $filepath\n";
    
    my $cleanFilepath = $filepath;
    if ( "$inputPath" ne "$outputPath" )
    {
      #$cleanFilepath = File::Spec->abs2rel( $cleanFilepath, $inputPath );
      $cleanFilepath = File::Spec->catfile( $outputPath, File::Spec->abs2rel( $cleanFilepath, $inputPath ) );
      print "OUT: $cleanFilepath\n";
    }
    
    unlink ( $cleanFilepath ) if -e $cleanFilepath;
    if ( $undo )
    {
      copy( $filepath, $cleanFilepath );
    }
    else
    {
      CleanFile( $filepath, $cleanFilepath );
    }
  }
}

###############################################################################
sub CleanFile
{
	my $filepath = shift;
	my $cleanFilepath = shift;

  foreach my $skipFile ( @skipFiles )
  {
      if ( $filepath =~ /\Q$skipFile\E$/i )
      {
         print "\n********SKIPPING: $filepath\n";
         return;
      }
  }
  
	open( IN, "<$filepath" ) or die $!;
	my @file = <IN>;
	chomp @file;
	close IN or die $!;
	
	foreach my $line ( @file )
	{
		chomp $line;
	   
    #$line =~ s/\Q#include <string>\E/#include "$headerFile"/g;
    
		#Fixup includes to header hpp file
		next if( $line =~ /\#include/ );
		
		# Quote string literals
		$line =~ s/("(?:[^"\\]+|\\.)*")/$stringLiteralMacro(\1)/g;
	  
    foreach my $func ( @replaceSTDFuncs )
    {
        #print "$func\n";
        $line =~ s/([^\w\:]|^)$func([^\w]|$)/\1std::$func\2/mg;
    }
    
    foreach my $func ( @replaceSTDContainers )
    {
        #print "$func\n";
        $line =~ s/([^\w\:]|^)$func(\s*\<)/\1std::$func\2/mg;
    }
     
	  # Fixup string vars
    for my $key ( keys %{$replaceHashRef} )
    {
        my $value = $replaceHashRef->{$key};
        #print "$key => $value\n";
        $line =~ s/([^\w]|^)$key([^\w]|$)/\1\Q${value}\E\2/mg;
    }
	}

	my $fileLen = @file;
	for( my $i = 0; defined $file[$i]; $i++ )
	{		
		while ( $file[$i] =~ /^\s*using namespace std;\s*$/ )
		{
			splice ( @file, $i, 1 );
		}
	}
	
	my $fileContents = join( "\n", @file );
	
  unlink ( $cleanFilepath ) if -e $cleanFilepath;
  open( OUT, ">$cleanFilepath" ) or die $!;
  print OUT $fileContents;
  close OUT or die $!;
}