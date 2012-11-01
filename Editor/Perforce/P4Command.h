#pragma once

#include "P4API.h"
#include "P4Provider.h"
#include "P4Exceptions.h"
#include "Platform/String.h"
#include "Platform/Thread.h"
#include "Platform/Profile.h"
#include "Application/RCSTypes.h"

#define PERFORCE_MAX_DICT_ENTRIES 64

namespace Helium
{
    namespace Perforce
    {
        class Command : public ClientUser
        {
        public:
            Command( Provider* provider, const tchar_t* command = TXT( "" ) )
                : m_Provider( provider )
                , m_Command( command )
                , m_ErrorCount( 0 )
            {

            }

            virtual void Run();

            void AddArg( const tstring& arg )
            {
#ifdef UNICODE
                std::string narrowArg;
                bool converted = Helium::ConvertString( arg, narrowArg );
                HELIUM_ASSERT( converted );
                AddArg( narrowArg.c_str() );
#else
                m_Arguments.push_back( arg.c_str() );
#endif
            }
            void AddArg( const char* arg )
            {
                m_Arguments.push_back( arg );
            }

            virtual void HandleError( Error* error );
            virtual void OutputStat( StrDict* dict );

            std::string AsString();

            Provider*              m_Provider;
            const tchar_t*           m_Command;
            std::vector< std::string > m_Arguments;
            int                    m_ErrorCount;
            tstring                m_ErrorString;
        };


        //
        // Translate string to enum
        //
        inline RCS::Operation GetOperationEnum( const tstring &operation )
        {
            if( operation == TXT( "add" ) )
                return RCS::Operations::Add;
            else if( operation == TXT( "move/add" ) )
                return RCS::Operations::Add;
            else if( operation == TXT( "delete" ) )
                return RCS::Operations::Delete;
            else if( operation == TXT( "move/delete" ) )
                return RCS::Operations::Delete;
            else if( operation == TXT( "edit" ) )
                return RCS::Operations::Edit;
            else if( operation == TXT( "branch" ) )
                return RCS::Operations::Branch;
            else if( operation == TXT( "integrate" ) )
                return RCS::Operations::Integrate;
            else if( operation == TXT( "" ) )
                return RCS::Operations::None;

            return RCS::Operations::Unknown;
        }

        inline RCS::FileType GetFileType( const tstring& fileType )
        {
            if ( fileType.find( TXT( "binary" ) ) != tstring::npos )
            {
                return RCS::FileTypes::Binary;
            }
            else if ( fileType.find( TXT( "text" ) ) != tstring::npos )
            {
                return RCS::FileTypes::Text;
            }

            return RCS::FileTypes::Unknown;
        }

        template <class ResultType>
        inline RCS::Operation ResultAsOperation( ResultType& results, int i )
        {
            return GetOperationEnum( Helium::MatchResultAsString( results, i ) );
        }

        inline void SetFlags( const tstring& flags, RCS::File* info )
        {
            info->m_Flags = 0;
            if ( flags.find_first_of( TXT( "l" ) ) != flags.npos )
                info->m_Flags |= RCS::FileFlags::Locking;
            if ( flags.find_first_of( TXT( "S" ) ) != flags.npos )
                info->m_Flags |= RCS::FileFlags::HeadOnly;
        }
    }
}