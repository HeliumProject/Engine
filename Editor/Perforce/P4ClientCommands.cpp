#include "EditorPch.h"
#include "P4ClientCommands.h"
#include "P4Tags.h"

#include "Foundation/Log.h"
#include "Platform/Encoding.h"

#include <p4/errornum.h>
#include <sstream>
#define _LARGE_TIME_API
#include <time.h>

using namespace Helium::Perforce;

void SyncCommand::Run()
{
    if ( m_SyncTime )
    {
        std::string spec = m_File->m_LocalPath;

#if HELIUM_OS_WIN
        struct tm* t = _localtime64( (__time64_t*)&m_SyncTime );
#else
        time_t syncTime = static_cast< time_t >( m_SyncTime );
        struct tm* t = localtime( &syncTime );
#endif
        char timeBuf[ 32 ];
        strftime( timeBuf, 32, TXT( "%Y/%m/%d:%H:%M:%S" ), t );
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
        Command::HandleError( error );
    }
}


void SyncCommand::OutputStat( StrDict* dict )
{
    bool converted = Helium::ConvertString( dict->GetVar( g_DepotFileTag )->Text(), m_File->m_DepotPath );
    HELIUM_ASSERT( converted );

    converted = Helium::ConvertString( dict->GetVar( g_ClientFileTag )->Text(), m_File->m_LocalPath );
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
    bool converted = Helium::ConvertString( dict->GetVar( g_DepotFileTag )->Text(), m_File->m_DepotPath );
    HELIUM_ASSERT( converted );

    m_File->m_LocalRevision = dict->GetVar( g_WorkRevTag )->Atoi();

    StrPtr* localPath = dict->GetVar( g_ClientFileTag );
    if ( localPath )
    {
        converted = Helium::ConvertString( localPath->Text(), m_File->m_LocalPath );
        HELIUM_ASSERT( converted );
    }

    StrPtr* action = dict->GetVar( g_ActionTag );
    if ( action )
    {
        std::string actionString;
        converted = Helium::ConvertString( action->Text(), actionString );
        HELIUM_ASSERT( converted );

        m_File->m_Operation = GetOperationEnum( actionString );
    }

    StrPtr* type = dict->GetVar( g_TypeTag );
    if ( type )
    {
        std::string fileType;
        converted = Helium::ConvertString( type->Text(), fileType );
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
    bool converted = Helium::ConvertString( dict->GetVar( g_DepotFileTag )->Text(), m_Dest->m_DepotPath );
    HELIUM_ASSERT( converted );

    converted = Helium::ConvertString( dict->GetVar( g_ClientFileTag )->Text(), m_Dest->m_LocalPath );
    HELIUM_ASSERT( converted );

    m_Dest->m_LocalRevision = dict->GetVar( g_WorkRevTag )->Atoi();
    
    std::string actionString;
    converted = Helium::ConvertString( dict->GetVar( g_ActionTag )->Text(), actionString );
    HELIUM_ASSERT( converted );

    m_Dest->m_Operation = GetOperationEnum( actionString );
}