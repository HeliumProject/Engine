#pragma once

#include <vector>

#include "Platform/Types.h"
#include "Foundation/Memory/SmartPtr.h"
#include "Application/API.h"
#include "RCSTypes.h"

namespace Helium
{
    namespace RCS
    {
        //
        // Revision
        //

        class File;
        typedef Helium::SmartPtr< File > FilePtr;
        typedef std::vector< FilePtr > V_FilePtr;

        class Revision : public Helium::RefCountBase< Revision >
        {
        public:
            i32          m_Revision;
            i32          m_ChangesetId;
            Operation    m_Operation;
            FileType     m_FileType;
            time_t       m_Time;
            u64          m_Size;

            tstring  m_Username;
            tstring  m_Client;
            tstring  m_Digest;
            tstring  m_Description;

            V_FilePtr m_IntegrationTargets;
            V_FilePtr m_IntegrationOrigins;

            Revision()
                : m_Revision( 0 )
                , m_ChangesetId( DefaultChangesetId )
                , m_Operation( Operations::None )
                , m_FileType( FileTypes::Unknown )
                , m_Time( 0 )
                , m_Size( 0 )
                , m_Username( TXT( "" ) )
                , m_Client( TXT( "" ) )
                , m_Digest( TXT( "" ) )
                , m_Description( TXT( "" ) )
            {
            }
        };

        typedef std::vector< Revision > V_Revision;
        typedef Helium::SmartPtr< Revision > RevisionPtr;
        typedef std::vector< RevisionPtr > V_RevisionPtr;

        //
        // Enumerate each piece of information we can get for an action
        //

        namespace ActionData
        {
            enum ActionDatum
            {
                None            = 0,

                Operation       = 1 << 0,
                Username        = 1 << 1,
                Client          = 1 << 2,
                ChangesetId     = 1 << 3,

                All             = 0xFFFFFFFF
            };
        }
        typedef ActionData::ActionDatum ActionDatum;

        class APPLICATION_API Action : public Helium::RefCountBase< Action >
        {
        public:
            // this gates which of the fields below are populated when info is gathered explicitly
            u32           m_ActionData;

            Operation     m_Operation;
            tstring       m_Username;
            tstring       m_Client;
            u32           m_ChangesetId;

            Action( u32 data = ActionData::All )
                : m_ActionData ( data )
                , m_Operation( Operations::None )
                , m_Username( TXT( "" ) )
                , m_Client( TXT( "" ) )
                , m_ChangesetId( DefaultChangesetId )
            {
            }
        };

        typedef std::vector< Action > V_Action;
        typedef Helium::SmartPtr< Action > ActionPtr;
        typedef std::vector< ActionPtr > V_ActionPtr;

        //
        // Enumerate each piece of information we can get for a file
        //

        namespace FileData
        {
            enum FileDatum
            {
                None            = 0,

                DepotPath       = 1 << 0,
                LocalPath       = 1 << 1,
                Digest          = 1 << 2,
                Size            = 1 << 3,
                Operation       = 1 << 4,
                State           = 1 << 5,
                Flags           = 1 << 6,
                LocalRevision   = 1 << 7,
                HeadRevision    = 1 << 8,
                HeadTime        = 1 << 9,
                HeadModTime     = 1 << 10,
                Username        = 1 << 11,
                Client          = 1 << 12,
                ChangesetId     = 1 << 13,
                Actions         = 1 << 14,
                Revisions       = 1 << 15,
                FileType        = 1 << 16,
                Exclusive       = 1 << 17,

                All             = 0xFFFFFFFF
            };
        }
        typedef FileData::FileDatum FileDatum;

        class APPLICATION_API File : public Helium::RefCountBase< File >
        {
        public:
            // this gates which of the fields below are populated when info is gathered explicitly
            u32           m_FileData;
            u32           m_ActionData;

            tstring       m_DepotPath;
            tstring       m_LocalPath;
            tstring       m_Digest;

            u64           m_Size;

            Operation     m_Operation;
            u32           m_State;
            u32           m_Flags;

            i32           m_LocalRevision;
            i32           m_HeadRevision;

            time_t        m_HeadTime;
            time_t        m_HeadModTime;

            tstring       m_Username;
            tstring       m_Client;

            u64           m_ChangesetId;

            V_ActionPtr   m_Actions;
            V_RevisionPtr m_Revisions;

            FileType      m_FileType;

            bool          m_Exclusive;

            File( const tstring& localPath = TXT( "" ), u32 fileData = FileData::All, u32 actionData = ActionData::All )
                : m_FileData ( fileData )
                , m_ActionData ( actionData )
                , m_DepotPath( TXT( "" ) )
                , m_LocalPath( localPath )
                , m_Digest( TXT( "" ) )
                , m_Size( 0 )
                , m_Operation( Operations::None )
                , m_State( FileStates::Unknown )
                , m_Flags( 0 )
                , m_LocalRevision( -1 )
                , m_HeadRevision( -1 )
                , m_HeadTime( 0 )
                , m_HeadModTime( 0 )
                , m_Username( TXT( "" ) )
                , m_Client( TXT( "" ) )
                , m_ChangesetId( DefaultChangesetId )
                , m_FileType( FileTypes::Text )
                , m_Exclusive( false )
            {
            }

            inline void Clear()
            {
                // This looks a little weird but the idea is to have a clear function that doesn't have to be changed everytime
                // the constructor is changed. The reason this works and doesn't cause any leaks is because currently the
                // operator= in our SmartPtr explicitly doesn't copy the refcount, if that ever changes then this will no longer work
                (*this) = File(); 
            }

            void GetInfo( const GetInfoFlag flags = GetInfoFlags::Default );

            void Sync( const u64 timestamp = 0 );

            void Open( const OpenFlag flags = OpenFlags::Default, const u64 changesetId = DefaultChangesetId );
            void Add( const OpenFlag flags = OpenFlags::Default, const u64 changesetId = DefaultChangesetId );
            void Edit( const OpenFlag flags = OpenFlags::Default, const u64 changesetId = DefaultChangesetId );
            void Delete( const OpenFlag flags = OpenFlags::Default, const u64 changesetId = DefaultChangesetId );
            void Reopen( const class Changeset& changeset, const OpenFlag flags = OpenFlags::Default );

            void Revert( const OpenFlag flags = OpenFlags::Default );

            // Like above, but will use ::MessageBox-style GUI to prompt for common handling in a program
            bool QueryOpen( const OpenFlag flags = OpenFlags::Default, const u64 changesetId = DefaultChangesetId );

            void Copy( RCS::File& target, const OpenFlag flags = OpenFlags::Default, const u64 changesetId = DefaultChangesetId );
            void Rename( RCS::File& target, const OpenFlag flags = OpenFlags::Default, const u64 changesetId = DefaultChangesetId );

            void Commit( const tstring& description );

            void GetCreatedByUser( tstring& username );
            void GetLastModifiedByUser( tstring& username );
            void GetOpenedByUsers( tstring& usernames );

            inline bool ExistsInDepot() const
            {
                return ( m_State & FileStates::ExistsInDepot ) != 0;
            }

            inline bool IsUpToDate() const
            {
                return ExistsInDepot() && ( HeadDeleted() ? m_LocalRevision == -1 : m_LocalRevision == m_HeadRevision );
            }

            inline bool IsOpen() const
            {
                return ExistsInDepot() && ( m_State & FileStates::CheckedOutByMe );
            }

            inline bool IsCheckedOut() const
            {
                return ExistsInDepot() && ( m_State & FileStates::CheckedOut );
            }

            inline bool IsCheckedOutByMe() const
            {
                return ExistsInDepot() && ( m_State & FileStates::CheckedOutByMe );
            }

            inline bool IsCheckedOutBySomeoneElse() const
            {
                return ExistsInDepot() && ( IsCheckedOut() && !IsCheckedOutByMe() );
            }

            inline bool IsBinary() const
            {
                return ( m_FileType == FileTypes::Binary );
            }

            inline bool HeadDeleted() const
            {
                return ( m_State & FileStates::HeadDeleted ) != 0;
            }

            inline bool LocalDeleted() const
            {
                return ( m_State & FileStates::LocalDeleted ) != 0;
            }

            inline u64 GetChangesetId() const
            {
                return m_ChangesetId;
            }

            inline bool IsText() const
            {
                return ( m_FileType == FileTypes::Text );
            }

            inline bool IsLocking() const
            {
                return ExistsInDepot() && ( m_Flags & FileFlags::Locking );
            }
        };

        typedef std::vector< File > V_File;
        typedef Helium::SmartPtr< File > FilePtr;
        typedef std::vector< FilePtr > V_FilePtr;
    }
}