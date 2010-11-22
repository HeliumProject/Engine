#pragma once

#include <string>
#include <vector>
#include <set>
#include <map>

#include "Foundation/API.h"
#include "Foundation/CommandLine/Option.h"

namespace Helium
{
    namespace CommandLine
    {
        class FOUNDATION_API Command
        {
        protected:
			tstring m_Token;
			tstring m_Usage;
			tstring m_ShortHelp;
			mutable tstring m_Help;

			OptionsMap m_OptionsMap;

        public:
			Command( const tchar_t* token, const tchar_t* usage = TXT( "[OPTIONS]" ), const tchar_t* shortHelp = TXT( "" ) );
            virtual ~Command();

			virtual bool Initialize( tstring& error )
			{
				return true;
			}

            virtual void Cleanup()
            {
            }

            const tstring& Token() const
			{
				return m_Token;
			}

			const tstring& ShortHelp() const
			{
				return m_ShortHelp;
			}

            virtual const tstring& Help() const;

            bool AddOption( const OptionPtr& option, tstring& error );
            bool ParseOptions( std::vector< tstring >::const_iterator& argsBegin, const std::vector< tstring >::const_iterator& argsEnd, tstring& error );

			virtual bool Process( std::vector< tstring >::const_iterator& argsBegin, const std::vector< tstring >::const_iterator& argsEnd, tstring& error ) = 0;
        };

        typedef std::map< tstring, Command* > M_StringToCommandDumbPtr;
    }
}
