#include "P4ClientCommands.h"
#include "P4Tags.h"

#include "Foundation/Log.h"
#include "Platform/String.h"

#include <p4/errornum.h>
#include <sstream>
#include <time.h>

using namespace Perforce;

void SyncCommand::Run()
{
    if ( m_SyncTime )
    {
        tstring spec = m_File->m_LocalPath;

        struct tm* t = _localtime64( (__time64_t*)&m_SyncTime );

        tchar timeBuf[ 32 ];
        _tcsftime( timeBuf, 32, TXT( "%Y/%m/%d:%H:%M:%S" ), t );

        spec += TXT( "@" );
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
    bool converted = Platform::ConvertString( dict->GetVar( g_DepotFileTag )->Text(), m_File->m_DepotPath );
    HELIUM_ASSERT( converted );

    converted = Platform::ConvertString( dict->GetVar( g_ClientFileTag )->Text(), m_File->m_LocalPath );
    HELIUM_ASSERT( converted );

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
    bool converted = Platform::ConvertString( dict->GetVar( g_DepotFileTag )->Text(), m_File->m_DepotPath );
    HELIUM_ASSERT( converted );

    m_File->m_LocalRevision = dict->GetVar( g_WorkRevTag )->Atoi();

    StrPtr* localPath = dict->GetVar( g_ClientFileTag );
    if ( localPath )
    {
        converted = Platform::ConvertString( localPath->Text(), m_File->m_LocalPath );
        HELIUM_ASSERT( converted );
    }

    StrPtr* action = dict->GetVar( g_ActionTag );
    if ( action )
    {
        tstring actionString;
        converted = Platform::ConvertString( action->Text(), actionString );
        HELIUM_ASSERT( converted );

        m_File->m_Operation = GetOperationEnum( actionString );
    }

    StrPtr* type = dict->GetVar( g_TypeTag );
    if ( type )
    {
        tstring fileType;
        converted = Platform::ConvertString( type->Text(), fileType );
        HELIUM_ASSERT( converted );

        m_File->m_FileType = GetFileType( fileType );
    }

    StrPtr* change = dict->GetVar( g_ChangeTag );
    if ( change )
    {
        m_File->m_ChangesetId = change->Atoi();
    }
}

void OpenCommand::Run()
{
    AddArg( TXT( "-c" ) );
    AddArg( RCS::GetChangesetIdAsString( m_File->m_ChangesetId ) );

    AddArg( m_File->m_LocalPath.c_str() );

    Command::Run();
}

void IntegrateCommand::Run()
{
    AddArg( TXT( "-c" ) );
    AddArg( RCS::GetChangesetIdAsString( m_Dest->m_ChangesetId ) );

    AddArg( TXT( "-d" ) );
    AddArg( m_Source->m_LocalPath );
    AddArg( m_Dest->m_LocalPath );

    Command::Run();
}

void IntegrateCommand::OutputStat( StrDict* dict )
{
    bool converted = Platform::ConvertString( dict->GetVar( g_DepotFileTag )->Text(), m_Dest->m_DepotPath );
    HELIUM_ASSERT( converted );

    converted = Platform::ConvertString( dict->GetVar( g_ClientFileTag )->Text(), m_Dest->m_LocalPath );
    HELIUM_ASSERT( converted );

    m_Dest->m_LocalRevision = dict->GetVar( g_WorkRevTag )->Atoi();
    
    tstring actionString;
    converted = Platform::ConvertString( dict->GetVar( g_ActionTag )->Text(), actionString );
    HELIUM_ASSERT( converted );

    m_Dest->m_Operation = GetOperationEnum( actionString );
}