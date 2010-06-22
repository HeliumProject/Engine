#pragma once

#include <string>
#include <vector>
#include <set>
#include <map>

#include "Foundation/API.h"
#include "Platform/Compiler.h"

namespace Nocturnal
{
    namespace CommandLine
    {
        class FOUNDATION_API Option
        {
		protected:
			std::string m_Token;
			std::string m_Usage;
			std::string m_Help;
			std::string m_Value;

        public:
			Option( const char* token, const char* usage = "<ARG>", const char* help = "" )
				: m_Token( token )
				, m_Usage( usage )
				, m_Help( help )
				, m_Value( "" )
            {
            }

            virtual ~Option()
            {
            }

			virtual const std::string& Token() const
			{
				return m_Token;
			}

			virtual const std::string& Usage() const
			{
				return m_Usage;
			}

			virtual const std::string& Help() const
			{
				return m_Help;
			}

			virtual bool Parse( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error )
			{
				if ( argsBegin != argsEnd )
				{
					m_Value = (*argsBegin);
					++argsBegin;

					//NOC_ASSERT( !m_Value.empty() );

					return true;
				}
				
				error = std::string( "Missing parameter for option: " ) + m_Token;

				return false;
			}

			template <class T>
			inline bool GetValue( T& value )
			{
				if ( !m_Value.empty() )
				{
					std::istringstream str ( m_Value );
					str >> value;
					return !str.fail();
				}
				return false;
			}

			template <>
			inline bool GetValue( std::string& value )
			{
				if ( !m_Value.empty() )
				{
					value = m_Value;
					return true;
				}
				return false;
			}

			template<>
			inline bool GetValue( bool& value )
			{
				if ( stricmp( m_Value.c_str(), "false" ) == 0 || stricmp( m_Value.c_str(), "0" ) == 0 )
				{
					value = false;
					return true;
				}
				else if ( stricmp( m_Value.c_str(), "true" ) == 0 || stricmp( m_Value.c_str(), "1" ) == 0 )
				{
					value = true;
					return true;
				}

				return false;
			}

        };

        typedef std::map< std::string, Option* > M_StringToOptionDumbPtr;
    }
}
