#pragma once

#include "Foundation/API.h"
#include "Foundation/CommandLine/Command.h"

#include "Platform/Compiler.h"

namespace Nocturnal
{
    namespace CommandLine
    {
		class Processor;

        class FOUNDATION_API Help : public Command
        {
		protected:
			Processor* m_Owner;
			std::string m_CommandName;

        public:
            Help( Processor* owner = NULL );
            virtual ~Help();

			void SetOwner( Processor* owner )
			{
				m_Owner = owner;
			}

			virtual bool Process( std::vector< std::string >::const_iterator& argsBegin, const std::vector< std::string >::const_iterator& argsEnd, std::string& error ) NOC_OVERRIDE;
        };
    }
}
