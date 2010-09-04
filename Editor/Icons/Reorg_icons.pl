use strict;

use Cwd;
use Data::Dumper;
use File::Copy;
use File::Find;
use File::Path;
use File::Spec;

my $inFolder = shift @ARGV;
my $outFolder = shift @ARGV or die "Where's the ouptup?\n";

# C:\Projects\github\nocturnal\Helium\Editor\Icons\Helium.png\Core\16add.png
# C:\Projects\github\nocturnal\Helium\Editor\Icons\16x16\Core\add.png
opendir( my $dh, $inFolder ) || die;
while( readdir $dh )
{
  my $folderName = "$_";
  
  next if ( $folderName eq '.' or $folderName eq '..' );
  
  my $folderPath = "$inFolder/$folderName";
  $folderName =~ tr/A-Z/a-z/;
  
  if ( -d $folderPath )
  {
    print "$folderName\n";
    opendir( my $dh2, $folderPath ) || die;
    while( readdir $dh2 )
    {
      my $filePath = "$folderPath/$_";
      
      #my $fileName = "$_";
      my ( $size, $fileName ) = $_ =~ /(\d+)(.*)/;
      
      next if ( $fileName eq '.' or $fileName eq '..' );
      next if ( -d $filePath );
      
      if ( -e $filePath )
      {
        my $newFilePath = "$outFolder\\${size}x${size}\\$folderName\\$fileName";
        mkpath( "$outFolder\\${size}x${size}\\$folderName" );
        copy( $filePath, $newFilePath );
        #print "$newFilePath\n";
      }
    }
    closedir $dh2;
  }
  elsif ( -e $folderPath )
  {
    print "ERROR-----------------------\n";
  }
    
}
closedir $dh;

exit 0;

sub RecursiveGetFiles()
{
  my $folder = shift;
  
  opendir( my $dh, $folder ) || die;
  while( readdir $dh )
  {
    next if ( $_ eq '.' or $_ eq '..' );
    
    my $path = "$folder/$_";
    if ( -d $path )
    {
      print "$path\n";
      RecursiveGetFiles( $path );
    }
    elsif ( -e $path )
    {
      print "$path\n";
    }
      
  }
  closedir $dh;
}