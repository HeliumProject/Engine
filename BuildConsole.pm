package Console;

use strict;

our @ISA = qw( Exporter );
our @EXPORT_OK = qw( Print PrintStream PrintColor Warning Debug Error Profile SetColor Reset SetOutputLevel GetOutputLevel AddTraceFile RemoveTraceFile );

my $g_IndentationLevel = 0;
my $g_BulletLevel = 0;

my @g_Bullets = qw( o * > - );
my $g_TotalBullets = scalar( @g_Bullets );

my $g_ErrorCount = 0;
my $g_WarningCount = 0;

my $g_NormalColor = 'none';
my $g_ErrorColor = 'red';
my $g_WarningColor = 'yellow';

my $g_Console = undef;

our %g_Colors = (
                   none => 7,
                   black => 0,
                   darkblue => 1,
                   blue => 9,
                   darkred => 4,
                   red => 12,
                   darkgreen => 2,
                   green => 10,
                   darkpurple => 5,
                   purple => 13,
                   darkaqua => 3,
                   aqua => 11,
                   darkyellow => 6,
                   yellow => 14,
                   grey => 7,
                   white => 15
                 );

our %Stream = (
                 Normal   => 1 << 0,
                 Debug    => 1 << 1,
                 Profile  => 1 << 2,
                 Warning  => 1 << 3,
                 Error    => 1 << 4,

                 Count    => 5, # careful, this needs to be updated based on the number of streams

                 #Defaults -> see below
                 All      => 0xFFFFFFFF,
               );

$Stream{ Defaults } = $Stream{ Normal } | $Stream{ Warning } | $Stream{ Error };

our %g_StreamColors = (
                         $Stream{ Normal }  => 'none',
                         $Stream{ Debug }   => 'aqua',
                         $Stream{ Profile } => 'green',
                         $Stream{ Warning } => 'yellow',
                         $Stream{ Error }   => 'red',
                      );

our %Level = (
                Minimal => 1,
                Normal  => 2,
                Verbose => 3,
              );

$Level{ Default } = $Level{ Normal };
our $g_OutputLevel = $Level{ Default };

our $g_FileManager = undef;

# i don't know why, but putting a BEGIN here seems to break the module?
{
  eval
  {
    require Win32::Console;
  };
  
  if ( !$@ )
  {
    $g_Console = new Win32::Console( &Win32::Console::STD_OUTPUT_HANDLE() );
  }

  $| = 1;
}

=pod

=head1 NAME

  Console -- Console printing and logging library

=head1 SYNOPSIS

  use Console;
  
  Console::Print( "Here is some output!\n" );

  # the following are colored in supported terminals
  Console::Error( "This is an error.\n" );
  Console::Debug( "This is debug info.\n" ); 
  Console::Profile( "This is profiling info.\n" );
  
  # add some log files
  Console::AddTraceFile( 'tracefile.log' );
  Console::AddTraceFile( 'error.log', $Console::Stream{ Error } );
  Console::AddTraceFile( 'appendfile.log', $Console::Stream{ Normal }, 1 );
  Console::AddTraceFile( 'all.log', $Console::Stream{ All } );

  # somewhere else, maybe we add the error log again, perhaps in some deeper
  # function
  Console::AddTraceFile( 'error.log', $Console::Stream{ Error } );

  # print some stuff after we've started logging
  Console::Print( "This goes into the 'tracefile.log' and 'appendfile.log'" );
  Console::Error( "This goes into the 'tracefile.log' and 'error.log'" );
  Console::Debug( "This and everything above, goes into 'all.log'" );

  Console::Log( "This only goes into the appropriate logs, not to screen." );
  
  # remove some log files
  Console::RemoveTraceFile( 'error.log' );
  Console::RemoveTraceFile( 'all.log' );
  
  Console::Error( "This error is still logged, added that log file twice\n" );
  
  # with this, we'll have removed the error.log as many times as we added
  # it, so it will actually be removed
  Console::RemoveTraceFile( 'error.log' );
  
  Console::Error( "This error will not be logged." );
  
=head1 DESCRIPTION

Console provides some helpful methods for printing to the console
and for organizing and logging output.  It also supports colored output
on some terminals.

=head1 METHODS

=cut

=pod

=head2 Print( formatString[, argumentArray ] )

Prints to the console and appropriate logs.  Printf work-alike with
decoration.

=cut

sub Print
{
  my $format = shift;
  
  PrintStream( $Stream{ Normal }, $g_OutputLevel, *STDOUT, Decorate( $format ), @_ );
}

=pod

=head2 Log( formatString[, argumentArray ] )

Writes only to the logs, not to the console.  Printf work-alike with
decoration.

=cut

sub Log
{
  LogOnly( $Stream{ Normal }, $g_OutputLevel, @_ );
}

sub LogOnly
{
  if ( defined( $g_FileManager ) )
  {
    my $stream = shift;
    my $level = shift;
    my $format = shift;
    $g_FileManager->Redirect( $stream, $level, Decorate( $format ), @_ );
  }    
}

=pod

=head2 Warning( formatString[, argumentArray ] )

Prints to STDOUT and logs using the 'Warning' stream.

=cut

sub Warning
{
  my $format = shift;
  
  ++$g_WarningCount;

  PrintStream( $Stream{ Warning }, $g_OutputLevel, *STDOUT, 'Warning(' . $g_WarningCount . '): ' . $format, @_ );
}

=pod

=head2 Error( formatString[, argumentArray ] )

Prints to STDERR and logs using the 'Error' stream.

=cut

sub Error
{
  my $format = shift;

  ++$g_ErrorCount;

  PrintStream( $Stream{ Error }, $g_OutputLevel, *STDERR, 'Error(' . $g_ErrorCount . '): ' . $format, @_ );
}

=pod

=head2 Debug( formatString[, argumentArray ] )

Prints to STDOUT and logs using the 'Debug' stream.

=cut

sub Debug
{
  my $format = shift;

  PrintStream( $Stream{ Debug }, $g_OutputLevel, *STDOUT, 'Debug: ' . Decorate( $format ), @_ );
}

=pod

=head2 Profile( formatString[, argumentArray ] )

Prints to STDOUT and logs using the 'Profile' stream.

=cut

sub Profile
{
  my $format = shift;

  PrintStream( $Stream{ Profile }, $g_OutputLevel, *STDOUT, 'Profile: ' . Decorate( $format ), @_ );
}

=pod

=head2 PrintRaw( formatString[, argumentArray ] )

Prints to the console and the logs.  Printf work-alike *without*
decoration. (No bulletting or indentation.)

=cut

sub PrintRaw
{
  my $format = shift;
  
  print sprintf( $format, @_ );

  if ( defined( $g_FileManager ) )
  {
    $g_FileManager->Redirect( $Stream{ Normal }, $g_OutputLevel, $format, @_ );
  }
}

=pod

=head2 PrintStream( stream, IO handle, formatString[, argumentArray ] )

Prints to the specified stream if it is one of the enable streams.
Colors the output based on the stream.  Prints to the specified IO handle.

Logs to log files looking for the specified stream.

=cut


sub PrintStream
{
  my $stream = shift;
  my $level  = shift;
  my $handle = shift;
  my $format = shift;
  
  my $shouldPrint = (     ( ( $Stream{ Defaults } & $stream ) == $stream )
                       && ( $level <= $g_OutputLevel )
                    );

  if ( $shouldPrint )
  {
    PrintColorHandle( GetPrintColor( $stream ), $handle, $format, @_ );
  }
  
  if ( defined( $g_FileManager ) )
  {
    $g_FileManager->Redirect( $stream, $level, $format, @_ );
  }
}

=pod

=head2 PrintColor( color, formatString[, argumentArray ] )

Prints to STDOUT using the specified color.  Printf work-alike with
decoration.

=cut

sub PrintColor
{
  my $color = shift;
  my $format = shift;
  
  PrintColorHandle( $color, *STDOUT, Decorate( $format ), @_ );
}

=pod

=head2 PrintColorHandle( color, IO handle, formatString[, argumentArray ] )

Prints to the specified handle using the specified color.  Printf work-alike
*without* decoration.

=cut

sub PrintColorHandle
{
  my $color = shift;
  my $handle = shift;
  my $format = shift;
  
  SetColor( $color );
  print $handle sprintf( $format, @_ );
  Reset();
}

=pod

=head2 GetPrintColor( stream )

Returns the color to use for the given stream.

=cut

sub GetPrintColor
{
  my $stream = shift;
  
  return exists( $g_StreamColors{ $stream } ) ? $g_StreamColors{ $stream } : $g_NormalColor;
}

=pod

=head2 SetColor( color )

Sets the output color.

=cut


sub SetColor
{
  return if !defined( $g_Console );
  
  my $color = shift;
  
  if ( !defined( $g_Colors{ $color } ) )
  {
    die( "No such color: $color" );
  }
  
  $g_Console->Attr( $g_Colors{ $color } );
}

=pod

=head2 Reset()

Resets the output color.

=cut

sub Reset
{
  return if !defined( $g_Console );
  
  $g_Console->Attr( $g_Colors{ $g_NormalColor } );
}

sub SetOutputLevel
{
  $g_OutputLevel = shift;
}

sub GetOutputLevel
{
  return $g_OutputLevel;
}

=pod

=head2 string Decorate( string )

Decorates the given string based on the indentation and bullet levels.
Returns the decorated string.

=cut


sub Decorate
{
  my $string = shift;

  $string = ( '  ' x $g_IndentationLevel ) . $string;

  if ( $g_BulletLevel )
  {
    my $bullet = $g_Bullets[ ( $g_BulletLevel - 1 ) % $g_TotalBullets ];
    $string = ' ' . ( ' ' x ( $g_BulletLevel - 1 ) ) . ( $bullet . ' ' ) . $string;
  }

  return $string;
}

=pod

=head2 AddTraceFile( filename[, stream[, level[, append]]] )

Adds the given trace file.

If a stream is specified, the log file will only contain things sent to
that stream.

Streams can be or-ed together, eg:

  my $streams = $Console::Stream{ Normal } | $Console::Stream{ Debug };
  AddTraceFile( 'somefile.log', $streams );

Adding a tracefile twice will add to the file's reference count.  A file
must be removed the same number of times it has been added to fully
remove logging to the file.

=cut

sub AddTraceFile
{
  my $filename = shift;
  my $stream = shift || $Stream{ Defaults };
  my $level = shift || $Level{ Default };
  my $append = shift || 0;

  if ( !defined( $g_FileManager ) )
  {
    $g_FileManager = Console::FileManager->new();
  }
  
  $g_FileManager->Open( File::Spec->rel2abs( $filename ), $stream, $level, $append );
}

=pod

=head2 RemoveTraceFile( filename )

Decrements the reference count for the given file in the logging system.

If the reference count for a file reaches zero, the file will be fully
removed and closed.

=cut

sub RemoveTraceFile
{
  my $filename = shift;
  $g_FileManager->Close( File::Spec->rel2abs( $filename ) );
}

1;

=pod

=head1 STACK OBJECTS

=head2 Console::Indentation

An indentation object.  Adding one within a given scope will increase
the indentation of the output until that scope is exited.  Eg:

  Console::Print( "This line is unindented.\n" );
  
  {
    my $firstIndent = Console::Indentation->new();
    
    Console::Print( "This line is indented one level.\n" );
    
    {
      my $secondIndent = Console::Indentation->new();
      
      Console::Print( "This line is indented two levels.\n" );
    }
    
    Console::Print( "This line is only indented one level.\n" );
  }
  
  Console::Print( "This line is unindented.\n" );
  
Would produce the output:

  This line is unindented.
    This line is indented one level.
      This line is indented two levels.
    This line is only indented one level.
  This line is unindented.

=cut

package Console::Indentation;

sub new
{
  my $class = shift;
  my $self = [];
  
  ++$g_IndentationLevel;
  
  bless $self, $class;
  return $self;
}

sub DESTROY
{
  my $self = shift;
  --$g_IndentationLevel;
}

1;

=head2 Console::Bullet

A bulletting object.  Adding one within a given scope will cause
output to be bulletted appropriately. Eg:

  Console::Print( "This line is unbulletted.\n" );
  
  {
    my $firstBullet = Console::Bullet->new();
    
    Console::Print( "This line is bulletted one level.\n" );
    
    {
      my $secondBullet = Console::Bullet->new();
      
      Console::Print( "This line is bulletted two levels.\n" );
    }
    
    Console::Print( "This line is only bulletted one level.\n" );
  }
  
  Console::Print( "This line is unbulletted.\n" );
  
Would produce the output:

  This line is unbulletted.

   o This line is bulletted one level.
     - This line is bulletted two levels.
   o This line is only bulletted one level.
  This line is unbulletted.

=cut

package Console::Bullet;

sub new
{
  my $class = shift;
  my $self = [];
  
  ++$g_BulletLevel;
  
  if ( $g_BulletLevel == 1 )
  {
    print "\n";
  }
  
  bless $self, $class;
  return $self;
}

sub DESTROY
{
  my $self = shift;
  --$g_BulletLevel;
}

1;

#
# TODO: Document
#

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

package Console::ScopedTraceFile;

use strict;

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

package Console::FileManager;

use strict;

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
