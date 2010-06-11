#include "Precompile.h"
#include "Help.h"

#include "Luna/CommandLine/Processor.h"
#include "Foundation/Log.h"

using namespace Nocturnal;
using namespace Nocturnal::Luna;
using namespace Nocturnal::Luna::CommandLine;

const std::string Help::s_Token = "help";
const std::string Help::s_ShortHelp = "help [command] - Displays the help for the command (or application if no command specified)";
const std::string Help::s_Help = "help [command]\n  Displays the help for the command (or application if no command specified)";

Help::Help()
{
}

Help::~Help()
{
}

bool Help::Process( std::vector< std::string >::const_iterator& itr, const std::vector< std::string >::const_iterator& argsEnd, std::string& error )
{
    const std::string& arg = (*itr);
    itr++;

    const Command* command = m_Owner->GetCommand( arg );
    if ( command )
    {
#pragma TODO( "Fix this up not to use Console" )
        Log::Print( command->GetHelp().c_str() );
    }
    else
    {
        error = std::string( "No help for command: " ) + arg;
        return  false;
    }

    return true;
}