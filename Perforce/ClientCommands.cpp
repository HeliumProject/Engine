#include "ClientCommands.h"
#include "Tags.h"

#include "Console/Console.h"

#include <p4/errornum.h>
#include <sstream>
#include <time.h>

using namespace Perforce;

void SyncCommand::Run()
{
  if ( m_SyncTime )
  {
    std::string spec = m_File->m_LocalPath;

    struct tm* t = _localtime64( (__time64_t*)&m_SyncTime );

    char timeBuf[ 32 ];
    strftime( timeBuf, 32, "%Y/%m/%d:%H:%M:%S", t );

    spec += "@";
    spec += timeBuf;

    AddArg( spec );
  }
  else
  {
    AddArg( m_File->m_LocalPath );
  }

  Command::Run();  
}

void SyncCommand::HandleError( Error* error ) 
{
  // If it is a 'file up-to-date' warning, eat it
  // Otherwise, let the base class handle it
  if ( !error->IsWarning() || error->GetGeneric() != EV_EMPTY )
  {
    __super::HandleError( error );
  }
}


void SyncCommand::OutputStat( StrDict* dict )
{
  m_File->m_DepotPath = dict->GetVar( g_DepotFileTag )->Text();
  m_File->m_LocalPath = dict->GetVar( g_ClientFileTag )->Text();
  m_File->m_LocalRevision = dict->GetVar( g_RevisionTag )->Atoi();
  if ( dict->GetVar( g_ChangeTag ) )
  {
    m_File->m_ChangesetId = dict->GetVar( g_ChangeTag )->Atoi();
  }

  // deleted files don't have a size
  StrPtr* fileSize = dict->GetVar( g_FileSizeTag );
  if ( fileSize )
  {
    m_File->m_Size = fileSize->Atoi64();
  }
}

void OpenCommand::OutputStat( StrDict* dict )
{
  m_File->m_DepotPath = dict->GetVar( g_DepotFileTag )->Text();
  m_File->m_LocalRevision = dict->GetVar( g_WorkRevTag )->Atoi();

  StrPtr* localPath = dict->GetVar( g_ClientFileTag );
  if ( localPath )
  {
    m_File->m_LocalPath = localPath->Text();
  }

  StrPtr* action = dict->GetVar( g_ActionTag );
  if ( action )
  {
    m_File->m_Operation = GetOperationEnum( action->Text() );
  }

  StrPtr* type = dict->GetVar( g_TypeTag );
  if ( type )
  {
    m_File->m_FileType = GetFileType( type->Text() );
  }

  StrPtr* change = dict->GetVar( g_ChangeTag );
  if ( change )
  {
    m_File->m_ChangesetId = change->Atoi();
  }
}

void OpenCommand::Run()
{
  PERFORCE_SCOPE_TIMER( ( "" ) );

  AddArg( "-c" );
  AddArg( RCS::GetChangesetIdAsString( m_File->m_ChangesetId ) );

  AddArg( m_File->m_LocalPath.c_str() );

  Command::Run();
}

void IntegrateCommand::Run()
{
  AddArg( "-c" );
  AddArg( RCS::GetChangesetIdAsString( m_Dest->m_ChangesetId ) );

  AddArg( "-d" );
  AddArg( m_Source->m_LocalPath );
  AddArg( m_Dest->m_LocalPath );

  Command::Run();
}

void IntegrateCommand::OutputStat( StrDict* dict )
{
  m_Dest->m_DepotPath = dict->GetVar( g_DepotFileTag )->Text();
  m_Dest->m_LocalPath = dict->GetVar( g_ClientFileTag )->Text();
  m_Dest->m_LocalRevision = dict->GetVar( g_WorkRevTag )->Atoi();
  m_Dest->m_Operation = GetOperationEnum( dict->GetVar( g_ActionTag )->Text() );
}