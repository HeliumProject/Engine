#pragma once

#include "Foundation/API.h"
#include "Foundation/CommandLine/Command.h"

namespace Helium
{
    namespace CommandLine
    {
		class Processor;

        class FOUNDATION_API Help : public Command
        {
		protected:
			Processor* m_Owner;
			tstring m_CommandName;

        public:
            Help( Processor* owner = NULL );
            virtual ~Help();

			void SetOwner( Processor* owner )
			{
				m_Owner = owner;
			}

			virtual bool Process( std::vector< tstring >::const_iterator& argsBegin, const std::vector< tstring >::const_iterator& argsEnd, tstring& error ) HELIUM_OVERRIDE;
        };
    }
}
