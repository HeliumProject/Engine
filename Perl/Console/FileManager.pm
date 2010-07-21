package Console::FileManager;

use strict;

use Console::TraceFile;

sub new
{
  my $class = shift;
  my $self = {};
  
  $self->{ files } = {};
  
  bless $self, $class;
  return $self;
}

sub Redirect
{
  my $self = shift;
  my $stream = shift;
  my $level = shift;
  my $format = shift || '';

  foreach my $file ( values( %{ $self->{ files } } ) )
  {
    if (     ( ( $file->{ stream } & $stream ) == $stream )
          && ( $file->{ level } < 0 || $level <= $file->{ level } ) )
    {
      my $fh = $file->{ handle };
      print $fh sprintf( $format, @_ );
    }
  }
}

sub Find
{
  my $self = shift;
  my $filename = shift;
  
  return $self->{ files }->{ $filename };
}

sub Open
{
  my $self = shift;
  my $filename = shift;
  my $stream = shift;
  my $level = shift;
  my $append = shift;
  
  my $file = $self->Find( $filename );

  if ( defined( $file ) )
  {
    # should already be open, just potentially add this new stream/level
    $file->Open( $stream, $level );
    return;
  }

  $self->{ files }->{ $filename } = Console::TraceFile->new( $filename, $stream, $level, $append );
  $self->{ files }->{ $filename }->Open();

}

sub Close
{
  my $self = shift;
  my $filename = shift;
  
  my $file = $self->Find( $filename );
  if ( defined( $file ) )
  {
    # check the return of close to see if we should actually get rid
    # of it (close can return 0 to indicate the file did not have a zero
    # refcount)
    if ( $file->Close() )
    {
      delete( $self->{ files }->{ $filename } );
    }
  }
}

sub DESTROY
{
  my $self = shift;
  
  while ( my ( $filename, $file ) = each( %{ $self->{ files } } ) )
  {
    if ( defined( $file ) )
    {
      $file->Close( 1 );
    }

    delete( $self->{ files }->{ $filename } );
  }
}

1;
