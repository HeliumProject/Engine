package Console::TraceFile;

use strict;

use File::Spec;
use File::Path;
use File::Basename;
use FileHandle;

sub new
{
  my $class = shift;
  my $self = {};
 
  $self->{ filename } = shift;
  $self->{ stream } = shift || 0;
  $self->{ level } = shift || -1;
  $self->{ append } = shift || 0;
  $self->{ refcount } = 0;
  $self->{ handle } = undef;
  
  bless $self, $class;
  return $self;
}

sub Open
{
  my $self = shift;
  my $stream = shift || 0;
  my $level = shift;
  
  $self->{ stream } |= $stream;

  if( defined( $level ) )
  {
    $self->{ level } = $level;
  }

  if ( defined( $self->{ handle } ) )
  {
    ++$self->{ refcount };
    return;
  }
  
  my $mode = $self->{ append } ? '+>>' : '+>';
  
  my $directory = File::Basename::dirname( $self->{ filename } );
  if ( !-d $directory )
  {
    mkpath( $directory );
  }
  
  $self->{ handle } = FileHandle->new();
  open( $self->{ handle }, $mode, $self->{ filename } );
    
  if ( !defined( $self->{ handle } ) )
  {
    die( "Could not open file '" . $self->{ filename } . "': $!\n" );
  }
    
  $self->{ refcount } = 1;
}

sub Close
{
  my $self = shift;
  my $force = shift || 0;
  
  $self->{ refcount } = $self->{ refcount } > 0 ? $self->{ refcount } - 1 : 0;
  
  if ( defined( $self->{ handle } ) && ( $self->{ refcount } <= 0 || $force ) )
  {
    if ( !$self->{ handle }->close() )
    {
      die( "Could not close file '" . $self->{ filename } . "': $!\n" );
    }
    
    $self->{ handle } = undef;
    
    return 1;
  }
  
  return 0;
}

sub DESTROY
{
  my $self = shift;
  $self->Close( 1 );
}

1;