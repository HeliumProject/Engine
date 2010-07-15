package Timer;

use strict;

use Time::HiRes qw( time );

sub new
{
  my $class = shift;
  
  my $self = {};
  bless $self, $class;

  $self->Reset();
    
  return $self;
}

sub Reset
{
  my $self = shift;
  $self->{ startTime } = time();
}

sub Elapsed
{
  my $self = shift;
  return time() - $self->{ startTime };
}

sub GetComponents
{
  my $self = shift;
  
  my $time = shift || $self->Elapsed();
  
  my $hours = int( $time / ( 60 * 60 ) );
  $time -= $hours * ( 60 * 60 );
  
  my $minutes = int( $time / 60 );
  $time -= $minutes * 60;

  return ( $hours, $minutes, $time );
}

sub AsString
{
  my $self = shift;
  my $time = shift;
  
  return sprintf( "%02d:%02d:%05.2f", $self->GetComponents( $time ) );
}

sub AsHighPrecisionString
{
  my $self = shift;
  my $time = shift;

  return sprintf( "%02d:%02d:%09.6f", $self->GetComponents( $time ) );
}

1;