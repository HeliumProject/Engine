#include "Precompile.h"
#include "Processor.h"

using namespace Nocturnal;
using namespace Nocturnal::Luna;
using namespace Nocturnal::Luna::CommandLine;

Processor::Processor()
{
}

Processor::~Processor()
{
    for ( M_StringToCommandDumbPtr::iterator itr = m_Commands.begin(), end = m_Commands.end(); itr != end; ++itr )
    {
        delete (*itr).second;
    }

    for ( M_StringToOptionDumbPtr::iterator itr = m_Options.begin(), end = m_Options.end(); itr != end; ++itr )
    {
        delete (*itr).second;
    }
}

bool Processor::Process( const std::vector< std::string >& arguments, std::string& error )
{
    bool result = true;

    std::vector< std::string >::const_iterator itr = arguments.begin(), end = arguments.end();

    while ( result && ( itr != end ) )
    {
        const std::string& arg = (*itr);
        itr++;

        if ( arg.length() >= 1 && arg[ 0 ] == '-' )
        {
            M_StringToOptionDumbPtr::iterator optionItr = m_Options.find( arg.substr( 1 ) );
            if ( optionItr != m_Options.end() )
            {
                result &= (*optionItr).second->Process( error );
            }
            else
            {
                error = std::string( "Unknown option: " ) + arg;
                return false;
            }
        }
        else
        {
            M_StringToCommandDumbPtr::iterator commandItr = m_Commands.find( arg );
            if ( commandItr != m_Commands.end() )
            {
                result &= (*commandItr).second->Process( itr, arguments.end(), error );
            }
            else
            {
                error = std::string( "Unknown command: " ) + arg + "\n\n";
                for ( M_StringToCommandDumbPtr::const_iterator cItr = m_Commands.begin(), cEnd = m_Commands.end(); cItr != cEnd; ++cItr )
                {
                    error += (*cItr).second->GetShortHelp() + "\n";
                }
            }
        }
    }

    return result;
}

bool Processor::RegisterCommand( Command* command )
{
    m_Commands[ command->Token() ] = command;
    command->SetOwner( this );
    return true;
}

void Processor::UnregisterCommand( Command* command )
{
    UnregisterCommand( command->Token() );
    command->SetOwner( NULL );
}

void Processor::UnregisterCommand( const std::string& token )
{
    m_Commands.erase( token );
}

const Command* Processor::GetCommand( const std::string& token )
{
    Command* command = NULL;
    M_StringToCommandDumbPtr::iterator itr = m_Commands.find( token );
    if ( itr != m_Commands.end() )
    {
        command = (*itr).second;
    }
    return command;
}

bool Processor::RegisterOption( Option* option )
{
    m_Options[ option->Token() ] = option;
    option->SetOwner( this );
    return true;
}
void Processor::UnregisterOption( Option* option )
{
    UnregisterOption( option->Token() );
    option->SetOwner( NULL );
}
void Processor::UnregisterOption( const std::string& token )
{
    m_Options.erase( token );
}

const Option* Processor::GetOption( const std::string& token )
{
    Option* option = NULL;
    M_StringToOptionDumbPtr::iterator itr = m_Options.find( token );
    if ( itr != m_Options.end() )
    {
        option = (*itr).second;
    }
    return option;
}
