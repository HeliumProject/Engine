#include "Help.h"

#include "Foundation/CommandLine/Processor.h"
#include "Foundation/Log.h"

using namespace Nocturnal;
using namespace Nocturnal::CommandLine;

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

    const Verb* verb = m_Owner->GetVerb( arg );
    if ( verb )
    {
        Log::Print( verb->GetHelp().c_str() );
    }
    else
    {
        error = std::string( "No help for verb: " ) + arg;
        return  false;
    }

    return true;
}