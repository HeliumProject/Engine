use strict;
use File::Path;
use File::Basename;

if ( !defined $ENV{wxWidgets} )
{
  die "wxWidgets is not defined in your environment!";
}

if ( !-d $ENV{wxWidgets} )
{
  die "wxWidgets environment variable does not point to a valid folder";
}

if ( -d File::Spec->catfile( $ENV{wxWidgets}, ".git" ) )
{
    print( "\n o Changing CWD to $ENV{wxWidgets}\n");
    chdir $ENV{wxWidgets};

    print( "\n o Svn update\n");
    system("svn update");

    my $origin = dirname $0;
    print( "\n o Changing CWD to $origin\n");
    chdir $origin;
}

print( "\n o Building wxWidgets\n");
do "build_wx.pl"