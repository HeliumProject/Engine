#include "ChangelistCommands.h"
#include "Tags.h"

#include "Foundation/Log.h"

using namespace Perforce;

void OpenedCommand::OutputStat( StrDict* dict )
{
  RCS::File file;

  file.m_DepotPath = dict->GetVar( g_DepotFileTag )->Text();
  file.m_LocalPath = dict->GetVar( g_ClientFileTag )->Text();
  file.m_LocalRevision = dict->GetVar( g_RevisionTag )->Atoi();
  file.m_Operation = GetOperationEnum( dict->GetVar( g_ActionTag )->Text() );
  file.m_ChangesetId = dict->GetVar( g_ChangeTag )->Atoi();
  file.m_FileType = GetFileType( dict->GetVar( g_TypeTag )->Text() );
  file.m_Username = dict->GetVar( g_UserTag )->Text();
  file.m_Client = dict->GetVar( g_ClientTag )->Text();

  m_FileList->push_back( file );
}

void SubmitCommand::OutputStat( StrDict* dict )
{
  if ( m_Changeset == NULL )
  {
    return;
  }

  StrPtr* submittedChange = dict->GetVar( g_SubmittedChangeTag );
  if ( submittedChange )
  {
    m_Changeset->m_Id = submittedChange->Atoi();
  }

  u32 numFiles = 0;
  StrPtr* openFiles = dict->GetVar( g_OpenFilesTag );
  if ( openFiles )
  {
    numFiles = openFiles->Atoi();
  }

  for( u32 i = 0; i < numFiles; ++i )
  {
    RCS::FilePtr file = new RCS::File();

    StrPtr* depotFile = dict->GetVar( g_DepotFileTag, i );
    if ( depotFile )
    {
      file->m_DepotPath = depotFile->Text();
    }

    StrPtr* revision = dict->GetVar( g_RevisionTag );
    if ( revision )
    {
      file->m_LocalRevision = revision->Atoi();
    }

    StrPtr* action = dict->GetVar( g_ActionTag );
    if ( action )
    {
      file->m_Operation = GetOperationEnum( action->Text() );
    }
  }
}

void RevertCommand::OutputStat( StrDict* dict )
{
  if ( m_File == NULL )
  {
    return;
  }

  m_File->m_DepotPath = dict->GetVar( g_DepotFileTag )->Text();
  m_File->m_LocalPath = dict->GetVar( g_ClientFileTag )->Text();
  m_File->m_LocalRevision = dict->GetVar( g_HaveRevTag )->Atoi();
  m_File->m_Operation = GetOperationEnum( dict->GetVar( g_OldActionTag )->Text() );
}

void CreateChangelistCommand::InputData( StrBuf *buf, Error *e )
{
  PERFORCE_SCOPE_TIMER( ( "" ) );

  std::string spec;
  spec  = "Change: new\n\n";
  spec += "Client: ";
  spec += m_Provider->m_ClientName + "\n\n";
  spec += "User: ";
  spec += m_Provider->m_UserName + "\n\n";
  spec += "Description:\n\t";
  spec += m_Changeset->m_Description;
  buf->Set( spec.c_str() );
}

void CreateChangelistCommand::Run()
{
  AddArg( "-i" );
  Command::Run();
}

// gah, the change command doesn't seem to have stat output... ?
void CreateChangelistCommand::OutputInfo( char level, const char* data )
{
  if ( sscanf_s( data, "Change %d", &m_Changeset->m_Id ) != 1 )
  {
    Log::Error( "Could not parse perforce changelist creation message:\n\n %s\n\nPlease contact support with this error message.\n" );
    m_ErrorCount++;
  }
}

void GetChangelistsCommand::Run()
{
  AddArg( "-c" );
  AddArg( m_Provider->m_ClientName );
  AddArg( "-s" );
  AddArg( "pending" );

  Command::Run();
}

void GetChangelistsCommand::OutputStat( StrDict* dict )
{
  RCS::Changeset changeset;

  changeset.m_Id = dict->GetVar( g_ChangeTag )->Atoi();
  changeset.m_Description = dict->GetVar( g_DescriptionTag )->Text();

  m_Changesets->push_back( changeset );
}