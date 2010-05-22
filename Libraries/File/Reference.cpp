#include "StdAfx.h"

#include "File.h"
#include "Reference.h"

#include "RCS/RCS.h"

#include "Common/File/File.h"
#include "Common/Checksum/MurmurHash2.h"

using namespace File;

REFLECT_DEFINE_CLASS( Reference );

void Reference::EnumerateClass( Reflect::Compositor< Reference >& comp )
{
    comp.AddField( &Reference::m_Id, "m_Id" );
    comp.AddField( &Reference::m_CreationTime, "m_CreationTime" );
    comp.AddField( &Reference::m_ModifiedTime, "m_ModifiedTime" );
    comp.AddField( &Reference::m_LastUsername, "m_LastUsername" );
    comp.AddField( &Reference::m_RelativePath, "m_RelativePath" );
    comp.AddField( &Reference::m_FullPathForSerialization, "m_FullPathForSerialization" );
    comp.AddField( &Reference::m_LastSignature, "m_LastSignature" );
    comp.AddField( &Reference::m_LastFileModifiedTime, "m_LastFileModifiedTime" );
}

std::string Reference::s_GlobalBaseDirectory = "";

Reference::Reference( tuid id )
: m_Id( id )
, m_CreationTime( (u64) _time64( NULL ) )
, m_ModifiedTime( 0 )
, m_LastUsername( getenv( "USERNAME" ) )
, m_RelativePath( "" )
, m_FullPathForSerialization( "" )
, m_LastSignature( "" )
, m_LastFileModifiedTime( 0 )
{
}

Reference::Reference( const std::string& path )
: m_Id( TUID::Null )
, m_CreationTime( (u64) _time64( NULL ) )
, m_ModifiedTime( 0 )
, m_LastUsername( getenv( "USERNAME" ) )
, m_RelativePath( "" )
, m_FullPathForSerialization( "" )
, m_LastSignature( "" )
, m_LastFileModifiedTime( 0 )
{
    Set( path );
}

Reference::~Reference()
{
}

void Reference::PostDeserialize()
{
    __super::PostDeserialize();

    m_FileObject.SetPath( m_FullPathForSerialization );
    m_FullPathForSerialization = m_FileObject.GetPath().Get(); // normalize
}

void Reference::Set( const Reference& rhs )
{
    m_Id = rhs.m_Id;
    m_LastUsername = rhs.m_LastUsername;
    m_CreationTime = rhs.m_CreationTime;
    m_ModifiedTime = rhs.m_ModifiedTime;
    m_RelativePath = rhs.m_RelativePath;
    m_FullPathForSerialization = rhs.m_FullPathForSerialization;
    m_LastSignature = rhs.m_LastSignature;
    
    Set( m_FullPathForSerialization );
}

void Reference::Set( const std::string& path )
{
    m_FileObject.SetPath( path );

    if ( !s_GlobalBaseDirectory.empty() )
    {
        if ( m_FileObject.GetPath().Get().compare( 0, s_GlobalBaseDirectory.length(), s_GlobalBaseDirectory ) == 0 )
        {
            m_RelativePath = m_FileObject.GetPath().Get().substr( s_GlobalBaseDirectory.length() );
        }
    }

    m_FullPathForSerialization = m_FileObject.GetPath().Get();

    m_Id = TUID::Null;

    if ( !File::GlobalResolver().Find( *this ) )
    {
        TUID::Generate( m_Id );
        File::GlobalResolver().Insert( *this );
    }

    m_LastUsername = getenv( "USERNAME" );

    m_ModifiedTime = (u64) _time64( NULL );
}


std::string Reference::AsString( bool verbose )
{
    char fileString[ 512 ];

    if ( verbose )
    {
        sprintf_s( fileString, 512, "id           : "TUID_HEX_FORMAT"\n" \
            "path         : %s\n" \
            "user         : %s\n" \
            "created      : %I64u\n" \
            "modified     : %I64u\n" \
            "lastsignature: %s\n" \
            ,
            m_Id,
            m_FileObject.GetPath().c_str(),
            m_LastUsername.c_str(),
            m_CreationTime,
            m_ModifiedTime,
            m_LastSignature.c_str() );
    }
    else
    {
        sprintf_s( fileString, 512, "%s ("TUID_HEX_FORMAT")", m_FileObject.GetPath().c_str(), m_Id );
    }

    return fileString;
}

bool Reference::IsValid()
{
    return !m_FileObject.GetPath().Get().empty();
}

u64 Reference::GetHash() const
{
    if ( !m_RelativePath.empty() )
    {
        return Nocturnal::MurmurHash2( m_RelativePath );
    }
    else
    {
        return Nocturnal::MurmurHash2( m_FileObject.GetPath().Get() );
    }
}

////////////////////////////////
// Resolve
//   - if we have an id, look it up in our local cache and fix up our path accordingly
//   - ensure our full and relative paths are in accord


void Reference::Resolve()
{
    if ( s_GlobalBaseDirectory.empty() )
    {
        NOC_ASSERT( m_RelativePath.empty() );
    }
    else
    {
        if ( m_RelativePath.empty() )
        {
            if ( m_FileObject.GetPath().Get().compare( 0, s_GlobalBaseDirectory.length(), s_GlobalBaseDirectory ) == 0 )
            {
                m_RelativePath = m_FileObject.GetPath().Get().substr( s_GlobalBaseDirectory.length() );
            }
        }
        else
        {
            m_FileObject.SetPath( s_GlobalBaseDirectory + m_RelativePath );
            m_FullPathForSerialization = m_FileObject.GetPath().Get();
        }
    }

    if ( !File::GlobalResolver().Find( *this ) )
    {
        if ( m_Id == TUID::Null )
        {
            TUID::Generate( m_Id );
        }

        File::GlobalResolver().Insert( *this );
    }

    m_ModifiedTime = (u64) _time64( NULL );

    struct _stat64 fileStatus;
    
    if ( !m_FileObject.GetStats64( fileStatus ) )
    {
        bool found = false;

        // file doesn't exist on disk, try to use revision control to track it down
        RCS::File rcsFile( m_FileObject.GetPath().Get() );
        rcsFile.GetInfo( (RCS::GetInfoFlag) ( RCS::GetInfoFlags::GetHistory | RCS::GetInfoFlags::GetIntegrationHistory ) );

        if ( !rcsFile.ExistsInDepot() )
        {
            return;
        }

        if ( !rcsFile.m_Revisions.empty() )
        {
            RCS::V_RevisionPtr::reverse_iterator itr = rcsFile.m_Revisions.rbegin();
            RCS::V_RevisionPtr::reverse_iterator end = rcsFile.m_Revisions.rend();
            for( ; itr != end; ++itr )
            {
                if ( !(*itr)->m_IntegrationTargets.empty() )
                {
                    RCS::V_FilePtr::iterator infoItr = (*itr)->m_IntegrationTargets.begin();
                    RCS::V_FilePtr::iterator infoEnd = (*itr)->m_IntegrationTargets.end();
                    for( ; infoItr != infoEnd; ++infoItr )
                    {
                        // likely match
                        if ( (*infoItr)->m_Digest == m_LastSignature )
                        {
                            memset( &fileStatus, 0, sizeof( fileStatus ) );
                            if ( _stat64( m_FileObject.GetPath().c_str(), &fileStatus ) != 0 )
                            {
                                Set( (*infoItr)->m_LocalPath );
                                if ( m_FileObject.GetStats64( fileStatus ) )
                                {
                                    found = true;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }

        if ( !found )
        {
            return;
        }
    }

    if ( m_LastFileModifiedTime != (u64) fileStatus.st_mtime )
    {
        m_LastSignature = m_FileObject.MD5();
        m_LastFileModifiedTime = fileStatus.st_mtime;
    }
}

void Reference::Retarget( const std::string& newPath )
{
    NOC_ASSERT( m_Id != TUID::Null );

    m_RelativePath.clear();
    m_FileObject.SetPath( newPath );
    m_FullPathForSerialization = m_FileObject.GetPath().Get();

    File::GlobalResolver().Update( *this );

    Resolve();
}
