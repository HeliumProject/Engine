#pragma once

#include "P4API.h"
#include "P4Provider.h"
#include "P4Exceptions.h"
#include "Platform/Encoding.h"
#include "Platform/Thread.h"
#include "Platform/Timer.h"
#include "Foundation/Regex.h"
#include "Application/RCSTypes.h"

#define PERFORCE_MAX_DICT_ENTRIES 64

namespace Helium
{
    namespace Perforce
    {
        class Command : public ClientUser
        {
        public:
            Command( Provider* provider, const char* command = "" )
                : m_Provider( provider )
                , m_Command( command )
                , m_ErrorCount( 0 )
            {

            }

            virtual void Run();

            void AddArg( const std::string& arg )
            {
                m_Arguments.push_back( arg.c_str() );
            }
            void AddArg( const char* arg )
            {
                m_Arguments.push_back( arg );
            }

            virtual void HandleError( Error* error );
            virtual void OutputStat( StrDict* dict );

            std::string AsString();

            Provider*              m_Provider;
            const char*           m_Command;
            std::vector< std::string > m_Arguments;
            int                    m_ErrorCount;
            std::string                m_ErrorString;
        };


        //
        // Translate string to enum
        //
        inline RCS::Operation GetOperationEnum( const std::string &operation )
        {
            if( operation == "add" )
                return RCS::Operations::Add;
            else if( operation == "move/add" )
                return RCS::Operations::Add;
            else if( operation == "delete" )
                return RCS::Operations::Delete;
            else if( operation == "move/delete" )
                return RCS::Operations::Delete;
            else if( operation == "edit" )
                return RCS::Operations::Edit;
            else if( operation == "branch" )
                return RCS::Operations::Branch;
            else if( operation == "integrate" )
                return RCS::Operations::Integrate;
            else if( operation == "" )
                return RCS::Operations::None;

            return RCS::Operations::Unknown;
        }

        inline RCS::FileType GetFileType( const std::string& fileType )
        {
            if ( fileType.find( "binary" ) != std::string::npos )
            {
                return RCS::FileTypes::Binary;
            }
            else if ( fileType.find( "text" ) != std::string::npos )
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

        inline void SetFlags( const std::string& flags, RCS::File* info )
        {
            info->m_Flags = 0;
            if ( flags.find_first_of( "l" ) != flags.npos )
                info->m_Flags |= RCS::FileFlags::Locking;
            if ( flags.find_first_of( "S" ) != flags.npos )
                info->m_Flags |= RCS::FileFlags::HeadOnly;
        }
    }
}