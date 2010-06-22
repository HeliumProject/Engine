#pragma once

#include <string>
#include <vector>
#include <set>
#include <map>

#include "Foundation/API.h"
#include "Foundation/CommandLine/Option.h"
#include "Platform/Compiler.h"

namespace Nocturnal
{
    namespace CommandLine
    {
		class FOUNDATION_API Flag : public Option
        {
        public:
            Flag( const char* token, const char* help = "" )
				: Option( token, "", help )
            {
				m_Value = "0";
            }

            virtual ~Flag()
            {
            }

			virtual bool Parse( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error ) NOC_OVERRIDE
			{
				m_Value = "1";
				return true;
			}


			bool IsSet()
			{
				bool result = false;
				if ( GetValue( result ) )
				{
					return result;
				}

				return false;
			}
        };
    }
}
