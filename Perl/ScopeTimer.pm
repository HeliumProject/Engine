package ScopeTimer;

use strict;

use Timer;

use Time::HiRes qw( time );

sub new
{
  my $class = shift;
  my $accumulatorRef = shift;
  
  my $self = {};
  $self->{ timer } = Timer->new();
  $self->{ accumulatorRef } = $accumulatorRef;
  
  bless $self, $class;
  return $self;
}

sub DESTROY
{
  my $self = shift;
  ${ $self->{ accumulatorRef } }->{ value } += $self->{ timer }->Elapsed();
}

1;