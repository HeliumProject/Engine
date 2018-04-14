#include "EditorPch.h"
#include "P4QueryCommands.h"
#include "P4Tags.h"

#include "Foundation/Log.h"
#include "Platform/Encoding.h"

#include <p4/errornum.h>

using namespace Helium::Perforce;

void WhereCommand::Run()
{
    if ( !m_File->m_DepotPath.empty() )
    {
        AddArg( m_File->m_DepotPath );
    }
    else
    {
        AddArg( m_File->m_LocalPath );
    }
    Command::Run();
}

void WhereCommand::OutputStat( StrDict *dict )
{
    bool converted = false;

    Helium::ConvertString( dict->GetVar( g_DepotFileTag )->Text(), m_File->m_DepotPath );
    HELIUM_ASSERT( converted );

    Helium::ConvertString( dict->GetVar( g_PathTag )->Text(), m_File->m_LocalPath );
    HELIUM_ASSERT( converted );
}

void HaveCommand::OutputStat(StrDict *dict)
{
	VersionInfo info;

	info.m_DepotPath = dict->GetVar(g_DepotFileTag)->Text();
	info.m_LocalPath = dict->GetVar(g_PathTag)->Text();
	info.m_LocalRevision = dict->GetVar(g_HaveRevTag)->Atoi();

	m_VersionInfo.push_back(info);
}

void DirsCommand::OutputStat(StrDict *dict)
{
	VersionInfo info;
	info.m_DepotPath = dict->GetVar(g_DirTag)->Text();
	m_VersionInfo.push_back(info);
}

void FilesCommand::OutputStat(StrDict *dict)
{
	VersionInfo info;

	info.m_DepotPath = dict->GetVar(g_DepotFileTag)->Text();
	info.m_HeadRevision = dict->GetVar(g_RevisionTag)->Atoi();
	info.m_Changelist = dict->GetVar(g_ChangeTag)->Atoi();
	info.m_Operation = GetOperationEnum(dict->GetVar(g_ActionTag)->Text());
	info.m_FileType = GetFileType(dict->GetVar(g_TypeTag)->Text());
	info.m_HeadTime = dict->GetVar(g_TimeTag)->Atoi();

	m_VersionInfo.push_back(info);
}

void FStatCommand::OutputStat( StrDict *dict )
{
    HELIUM_ASSERT_MSG( m_File, TXT("No file info object to store file information to") );

    if ( m_File->m_FileData & RCS::FileData::State )
    {
        // file necessarily exists in the depot to have gotten here
        m_File->m_State |= RCS::FileStates::ExistsInDepot;
    }

    if ( m_File->m_FileData & RCS::FileData::DepotPath )
    {
        bool converted = Helium::ConvertString( dict->GetVar( g_DepotFileTag )->Text(), m_File->m_DepotPath );
        HELIUM_ASSERT( converted );
    }

    if ( m_File->m_FileData & RCS::FileData::LocalPath )
    {
        if ( dict->GetVar( g_ClientFileTag ) )
        {
            bool converted = Helium::ConvertString( dict->GetVar( g_ClientFileTag )->Text(), m_File->m_LocalPath );
            HELIUM_ASSERT( converted );
        }
    }

    if ( m_File->m_FileData & RCS::FileData::Username )
    {
        StrPtr* username = dict->GetVar( g_ActionOwnerTag );
        if ( username )
        {
            bool converted = Helium::ConvertString( username->Text(), m_File->m_Username );
            HELIUM_ASSERT( converted );
        }
    }

    if ( m_File->m_FileData & RCS::FileData::ChangesetId )
    {
        StrPtr* changelist = dict->GetVar( g_ChangeTag );
        if ( changelist )
        {
            m_File->m_ChangesetId = changelist->Atoi();
            if ( m_File->m_ChangesetId == 0 )
            {
                m_File->m_ChangesetId = RCS::DefaultChangesetId;
            }
        }
    }

    if ( m_File->m_FileData & RCS::FileData::Operation )
    {
        StrPtr* action = dict->GetVar( g_ActionTag );
        if ( action )
        {
            std::string actionString;
            bool converted = Helium::ConvertString( action->Text(), actionString );
            HELIUM_ASSERT( converted );
            m_File->m_Operation = GetOperationEnum( actionString );

            if ( m_File->m_FileData & RCS::FileData::State )
            {
                m_File->m_State |= RCS::FileStates::CheckedOutByMe;

                if ( m_File->m_Operation == RCS::Operations::Delete )
                {
                    m_File->m_State |= RCS::FileStates::LocalDeleted;
                }
            }
        }
    }

    if ( m_File->m_FileData & RCS::FileData::Actions && m_File->m_ActionData )
    {
        StrPtr* otherOpen = dict->GetVar( g_OtherOpenTag );
        if ( otherOpen )
        {
            if ( m_File->m_FileData & RCS::FileData::State )
            {
                m_File->m_State |= RCS::FileStates::CheckedOut;
            }

            int totalOpens = otherOpen->Atoi();
            m_File->m_Actions.reserve( totalOpens );
            for ( int i = 0; i < totalOpens; ++i )
            {
                RCS::ActionPtr action = new RCS::Action( m_File->m_ActionData );

                if ( action->m_ActionData & RCS::ActionData::Username || action->m_ActionData & RCS::ActionData::Client )
                {
                    StrPtr* userInfo = dict->GetVar( g_OtherOpenTag, i );
                    if ( userInfo != NULL )
                    {
                        std::string otherUserInfo;
                        bool converted = Helium::ConvertString( userInfo->Text(), otherUserInfo );
                        HELIUM_ASSERT( converted );

                        size_t atLocation = otherUserInfo.find( TXT( "@" ) );

                        if ( action->m_ActionData & RCS::ActionData::Username )
                        {
                            action->m_Username = otherUserInfo.substr( 0, atLocation );
                        }
                        if ( action->m_ActionData & RCS::ActionData::Client )
                        {
                            action->m_Client = otherUserInfo.substr( atLocation + 1 );
                        }
                    }
                }

                if ( action->m_ActionData & RCS::ActionData::ChangesetId )
                {
                    StrPtr* changeInfo = dict->GetVar( g_OtherChangeTag, i );
                    if ( changeInfo )
                    {
                        action->m_ChangesetId = changeInfo->Atoi();
                        if ( action->m_ChangesetId == 0 )
                        {
                            action->m_ChangesetId = RCS::DefaultChangesetId;
                        }
                    }
                }

                if ( action->m_ActionData & RCS::ActionData::Operation )
                {
                    StrPtr* actionInfo = dict->GetVar( g_OtherActionTag, i );
                    if ( actionInfo )
                    {
                        std::string actionString;
                        bool converted = Helium::ConvertString( actionInfo->Text(), actionString );
                        HELIUM_ASSERT( converted );
                        action->m_Operation = GetOperationEnum( actionString );
                    }
                }

                m_File->m_Actions.push_back( action );
            }
        }
    }

    if ( m_File->m_Operation == RCS::Operations::Add )
    {
        std::string fileType;
        bool converted = Helium::ConvertString( dict->GetVar( g_TypeTag )->Text(), fileType );
        HELIUM_ASSERT( converted );

        if ( m_File->m_FileData & RCS::FileData::FileType )
        {
            m_File->m_FileType = GetFileType( fileType );
        }

        if ( m_File->m_FileData & RCS::FileData::Flags )
        {
            SetFlags( fileType, m_File );
        }

        return; // out of info at this point
    }

    if ( m_File->m_FileData & RCS::FileData::LocalRevision )
    {
        StrPtr* haveRev = dict->GetVar( g_HaveRevTag );
        if ( haveRev )
        {
            m_File->m_LocalRevision = haveRev->Atoi();
        }
    }

    // If a file is deleted at head, it does not have a digest or size
    if ( m_File->m_FileData & RCS::FileData::Digest || m_File->m_FileData & RCS::FileData::Size )
    {
        StrPtr* headAction = dict->GetVar( g_HeadActionTag );
        if ( headAction )
        {
            std::string actionString;
            bool converted = Helium::ConvertString( headAction->Text(), actionString );
            HELIUM_ASSERT( converted );

            if ( GetOperationEnum( actionString ) == RCS::Operations::Delete )
            {
                if ( m_File->m_FileData & RCS::FileData::State )
                {
                    m_File->m_State |= RCS::FileStates::HeadDeleted;
                }
            }
            else
            {
                if ( m_File->m_FileData & RCS::FileData::Digest )
                {
                    StrPtr* digest = dict->GetVar( g_DigestTag );
                    if ( digest )
                    {
                        bool converted = Helium::ConvertString( digest->Text(), m_File->m_Digest );
                        HELIUM_ASSERT( converted );
                    }
                }

                if ( m_File->m_FileData & RCS::FileData::Digest )
                {
                    StrPtr* size = dict->GetVar( g_FileSizeTag );
                    if ( size )
                    {
                        m_File->m_Size = (uint64_t) size->Atoi64();
                    }
                }
            }
        }
    }

    if ( m_File->m_FileData & RCS::FileData::FileType || m_File->m_FileData & RCS::FileData::Flags )
    {
        StrPtr* headType = dict->GetVar( g_HeadTypeTag );
        if ( headType )
        {
            std::string fileType;
            bool converted = Helium::ConvertString( headType->Text(), fileType );
            HELIUM_ASSERT( converted );

            if ( m_File->m_FileData & RCS::FileData::FileType)
            {
                m_File->m_FileType = GetFileType( fileType );
            }

            if ( m_File->m_FileData & RCS::FileData::Flags )
            {
                SetFlags( fileType, m_File );
            }
        }
    }

    if ( m_File->m_FileData & RCS::FileData::HeadModTime )
    {
        StrPtr* headModTime = dict->GetVar( g_HeadModTimeTag );
        if ( headModTime )
        {
            m_File->m_HeadModTime = headModTime->Atoi();
        }
    }

    if ( m_File->m_FileData & RCS::FileData::HeadRevision )
    {
        StrPtr* headRevTag = dict->GetVar( g_HeadRevTag );
        if ( headRevTag )
        {
            m_File->m_HeadRevision = headRevTag->Atoi();
        }
    }

    if ( m_File->m_FileData & RCS::FileData::HeadTime )
    {
        StrPtr* headTimeTag = dict->GetVar( g_HeadTimeTag );
        if ( headTimeTag )
        {
            m_File->m_HeadTime = headTimeTag->Atoi();
        }
    }
}

void FStatCommand::HandleError( Error* error )
{
    // check to see if it's the 'no such files' warning
    // if it is, then just eat it, otherwise pass it down to the base class to be handled
    if ( error->GetGeneric() != EV_EMPTY )
    {
        Command::HandleError( error );
    }
}

void SingleFStatCommand::Run()
{
    if ( !m_File->m_DepotPath.empty() )
    {
        AddArg( m_File->m_DepotPath );
    }
    else
    {
        AddArg( m_File->m_LocalPath );
    }

    FStatCommand::Run();  
}

void SingleFStatCommand::OutputStat( StrDict *dict )
{
    m_File->Clear();

    FStatCommand::OutputStat( dict );
}

void MultiFStatCommand::Run()
{
    if ( *m_Folder.rbegin() != '/' && *m_Folder.rbegin() != '\\' )
    {
        m_Folder.push_back( '/' );
    }

    AddArg( m_Folder + std::string( m_Recursive ? TXT( "..." ) : TXT( "*" ) ) );

    FStatCommand::Run();  
}

void MultiFStatCommand::OutputStat( StrDict *dict )
{
    m_Files->push_back( RCS::File () );

    m_File = &m_Files->back();
    m_File->m_FileData = m_FileData;
    m_File->m_ActionData = m_ActionData;

    FStatCommand::OutputStat( dict );
}

void FileLogCommand::Run()
{
    if ( !m_File->m_DepotPath.empty() )
    {
        AddArg( m_File->m_DepotPath );
    }
    else
    {
        AddArg( m_File->m_LocalPath );
    }
    Command::Run();  
}

void FileLogCommand::OutputStat( StrDict *dict )
{
    StrPtr* latestRevision = dict->GetVar( g_RevisionTag, 0 );
    if ( latestRevision )
    {
        uint32_t numRevisions = latestRevision->Atoi();

        for( uint32_t i = 0; i < numRevisions; ++i )
        {
            RCS::Revision* revision = new RCS::Revision();

            revision->m_Revision = dict->GetVar( g_RevisionTag, i )->Atoi();
            revision->m_ChangesetId = dict->GetVar( g_ChangeTag, i )->Atoi();

            std::string actionString;
            bool converted = Helium::ConvertString( dict->GetVar( g_ActionTag, i )->Text(), actionString );
            HELIUM_ASSERT( converted );

            revision->m_Operation = GetOperationEnum( actionString );

            std::string fileType;
            converted = Helium::ConvertString( dict->GetVar( g_TypeTag, i )->Text(), fileType ); 
            HELIUM_ASSERT( converted );

            revision->m_FileType = GetFileType( fileType );

            revision->m_Time = (time_t) dict->GetVar( g_TimeTag, i )->Atoi();
            
            converted = Helium::ConvertString( dict->GetVar( g_UserTag, i )->Text(), revision->m_Username );
            HELIUM_ASSERT( converted );

            converted = Helium::ConvertString( dict->GetVar( g_ClientTag, i )->Text(), revision->m_Client );
            HELIUM_ASSERT( converted );

            // This should not happen, but it has happened before when "move/delete" was added
            // to Perforce's vocabulary without our knowledge.
            HELIUM_ASSERT( revision->m_Operation != RCS::Operations::Unknown );

            if ( revision->m_Operation != RCS::Operations::Delete && revision->m_Operation != RCS::Operations::Unknown )
            {
                revision->m_Size = dict->GetVar( g_FileSizeTag, i )->Atoi64();
                
                converted = Helium::ConvertString( dict->GetVar( g_DigestTag, i )->Text(), revision->m_Digest );
                HELIUM_ASSERT( converted );
            }

            converted = Helium::ConvertString( dict->GetVar( g_DescriptionTag, i )->Text(), revision->m_Description );
            HELIUM_ASSERT( converted );

            for ( uint32_t target = 0; target < PERFORCE_MAX_DICT_ENTRIES; ++target )
            {
                StrPtr* how = dict->GetVar( g_HowTag, i, target );
                if ( !how )
                {
                    break;
                }

                // Integration Actions
                // http://www.perforce.com/perforce/doc.current/manuals/cmdref/integrated.html
                if ( ( std::string( how->Text() ) == "branch into" )
                    || ( std::string( how->Text() ) == "add into" ) )
                {
                    RCS::File* targetInfo = new RCS::File();
                    
                    converted = Helium::ConvertString( dict->GetVar( g_FileTag, i, target )->Text(), targetInfo->m_DepotPath );
                    HELIUM_ASSERT( converted );

                    revision->m_IntegrationTargets.push_back( targetInfo );
                }
                else if ( ( std::string( how->Text() ) == "branch from" ) )
                {
                    RCS::File* targetInfo = new RCS::File();
                    
                    converted = Helium::ConvertString( dict->GetVar( g_FileTag, i, target )->Text(), targetInfo->m_DepotPath );
                    HELIUM_ASSERT( converted );

                    revision->m_IntegrationOrigins.push_back( targetInfo );
                }
            }

            m_File->m_Revisions.push_back( revision );
        }
    }
}

void FileLogCommand::HandleError( Error* error )
{
    // check to see if it's the 'no such files' warning
    // if it is, then just eat it, otherwise pass it down to the base class to be handled
    if ( error->GetGeneric() != EV_EMPTY )
    {
        Command::HandleError( error );
    }
}
