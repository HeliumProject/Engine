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
        class FOUNDATION_API Command
        {
        protected:
			std::string m_Token;
			std::string m_Usage;
			std::string m_ShortHelp;
			mutable std::string m_Help;

			OptionsMap m_OptionsMap;

        public:
			Command( const char* token, const char* usage = "[OPTIONS]", const char* shortHelp = "" );
            virtual ~Command();

			virtual bool Initialize( std::string& error )
			{
				return true;
			}

            const std::string& Token() const
			{
				return m_Token;
			}

			const std::string& ShortHelp() const
			{
				return m_ShortHelp;
			}

            virtual const std::string& Help() const;

			virtual bool Process( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error ) = 0;
        };

        typedef std::map< std::string, Command* > M_StringToCommandDumbPtr;
    }
}
