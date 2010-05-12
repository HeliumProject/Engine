#include "Provider.h"
#include "Perforce.h"
#include "Exceptions.h"
#include "QueryCommands.h"
#include "ClientCommands.h"
#include "ChangelistCommands.h"

#include "Common/Assert.h"
#include "Common/Version.h"
#include "AppUtils/AppUtils.h"
#include "Windows/Windows.h"
#include "Platform/Mutex.h"
#include "RCS/RCS.h"

using namespace Perforce;

Provider::Provider()
: m_Enabled( true )
, m_Connected( false )
{
  m_Client = new ClientApi();
  m_Mutex = new Platform::Mutex();
}

Provider::~Provider()
{
  _flushall();

  PERFORCE_SCOPE_TIMER( ( "" ) );

  if ( m_Connected && !m_Client->Dropped() )
  {
    Error e;
    m_Client->Final( &e );
  }

  delete m_Client;
  delete m_Mutex;
}


///////////////////////////////////////////////////////////////////
// Implementation

bool Provider::IsEnabled()
{
  return m_Enabled;
}

void Provider::SetEnabled( bool enabled )
{
  m_Enabled = enabled;
}

const char* Provider::GetName()
{
  return "Perforce";
}

void Provider::Sync( RCS::File& file, const u64 timestamp )
{
  PERFORCE_SCOPE_TIMER( ( "" ) );

  SyncCommand command( this, &file, timestamp );

  command.Run();
}

void Provider::GetInfo( RCS::File& file, const RCS::GetInfoFlag flags )
{
  SingleFStatCommand command( this, &file );
  command.Run();

  if ( ( flags & RCS::GetInfoFlags::GetHistory ) == RCS::GetInfoFlags::GetHistory )
  {
    FileLogCommand filelogCommand( this, &file, ( ( flags & RCS::GetInfoFlags::GetIntegrationHistory ) == RCS::GetInfoFlags::GetIntegrationHistory ) );
    filelogCommand.Run();
  }
}

void Provider::GetInfo( const std::string& folder, RCS::V_File& files, bool recursive, u32 fileData, u32 actionData )
{
  MultiFStatCommand command( this, folder, &files, recursive, fileData, actionData );
  command.Run();
}

void Provider::Add( RCS::File& file )
{
  PERFORCE_SCOPE_TIMER( ( "" ) );

  OpenCommand command( this, "add", &file );
  command.Run();
}

void Provider::Edit( RCS::File& file )
{
  PERFORCE_SCOPE_TIMER( ( "" ) );

  OpenCommand command ( this, "edit", &file );
  command.Run();
}

void Provider::Delete( RCS::File& file )
{
  PERFORCE_SCOPE_TIMER( ( "" ) );

  OpenCommand command ( this, "delete", &file );
  command.Run();
}

void Provider::Integrate( RCS::File& source, RCS::File& dest )
{
  PERFORCE_SCOPE_TIMER( ( "" ) );

  IntegrateCommand command( this, &source, &dest );
  command.Run();
}

void Provider::Reopen( RCS::File& file )
{
  PERFORCE_SCOPE_TIMER( ( "" ) );

  OpenCommand command( this, "reopen", &file );
  command.Run();
}

void Provider::GetChangesets( RCS::V_Changeset& changesets )
{
  PERFORCE_SCOPE_TIMER( ( "" ) );

  GetChangelistsCommand command( this, &changesets );
  command.Run();
}

void Provider::CreateChangeset( RCS::Changeset& changeset )
{
  PERFORCE_SCOPE_TIMER( ( "" ) );

  CreateChangelistCommand command( this, &changeset );
  command.Run();
}

void Provider::GetOpenedFiles( RCS::V_File& files )
{
  PERFORCE_SCOPE_TIMER( ( "" ) );

  OpenedCommand command( this, &files );
  command.Run();
}

void Provider::Commit( RCS::Changeset& changeset )
{
  PERFORCE_SCOPE_TIMER( ( "" ) );

  SubmitCommand command( this, &changeset );

  command.AddArg( "-c" );
  command.AddArg( RCS::GetChangesetIdAsString( changeset.m_Id ) );

  command.Run();
}

void Provider::Revert( RCS::Changeset& changeset, bool revertUnchangedOnly )
{
  RevertCommand command( this );

  command.AddArg( "-c" );
  command.AddArg( RCS::GetChangesetIdAsString( changeset.m_Id ) );

  if ( revertUnchangedOnly )
  {
    command.AddArg( "-a" );
  }

  command.AddArg( "//..." ); // careful

  command.Run();
}


void Provider::Revert( RCS::File& file, bool revertUnchangedOnly )
{
  PERFORCE_SCOPE_TIMER( ( "" ) );

  RevertCommand command( this, &file );

  command.AddArg( "-c" );
  command.AddArg( RCS::GetChangesetIdAsString( file.m_ChangesetId ) );

  if ( revertUnchangedOnly )
  {
    command.AddArg( "-a" );
  }

  command.AddArg( file.m_LocalPath );

  command.Run();
}

void Provider::Rename( RCS::File& source, RCS::File& dest )
{
  IntegrateCommand integrateCommand( this, &source, &dest );
  integrateCommand.Run();

  OpenCommand deleteCommand( this, "delete", &source );
  deleteCommand.Run();
}
