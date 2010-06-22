#pragma once

#include <string>
#include <vector>
#include <set>
#include <map>

#include "Foundation/API.h"
#include "Foundation/CommandLine/Option.h"
#include "Foundation/CommandLine/Flag.h"
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

			Nocturnal::CommandLine::Flag m_HelpFlag;

			M_StringToOptionDumbPtr m_Options;

        public:
			Command( const char* token, const char* usage = "[OPTIONS]", const char* shortHelp = "" );
            virtual ~Command();

            const std::string& Token() const
			{
				return m_Token;
			}

			const std::string& ShortHelp() const
			{
				return m_ShortHelp;
			}

            virtual const std::string& Help() const;

            bool RegisterOption( Option* option );
            void UnregisterOption( Option* option );
            void UnregisterOption( const std::string& token );
            const Option* GetOption( const std::string& token );

			virtual void RegisterOptions();

			virtual bool Parse( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error );
			
			virtual bool Process( std::string& error );
        };

        typedef std::map< std::string, Command* > M_StringToCommandDumbPtr;
    }
}
