package Console::ScopedTraceFile;

use strict;

use Console;

sub new
{
  my $class = shift;
  my $filename = shift;
  
  my $self = {};

  $self->{ traceFilename } = $filename;
  
  Console::AddTraceFile( $filename, @_ );
  
  bless $self, $class;
  return $self;
}

sub DESTROY
{
  my $self = shift;
  
  Console::RemoveTraceFile( $self->{ traceFilename } );
}

1;