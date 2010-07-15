package Accumulator;

use strict;

use Timer;

use Time::HiRes qw( time );

sub new
{
  my $class = shift;
  
  my $self = {};
  $self->{ value } = 0;
  
  bless $self, $class;
  return $self;
}

sub AsString
{
  my $self = shift;
  my $timer = Timer->new();
  return $timer->AsString( $self->{ value } );
}

sub AsHighPrecisionString
{
  my $self = shift;
  my $timer = Timer->new();
  return $timer->AsHighPrecisionString( $self->{ value } );
}

1;