#include "Processor.h"

using namespace Nocturnal::CommandLine;

Processor::Processor()
{
}

Processor::~Processor()
{
    for ( M_StringToVerbDumbPtr::iterator itr = m_Verbs.begin(), end = m_Verbs.end(); itr != end; ++itr )
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
            M_StringToOptionDumbPtr::const_iterator optionItr = m_Options.find( arg.substr( 1 ) );
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
            M_StringToVerbDumbPtr::iterator commandItr = m_Verbs.find( arg );
            if ( commandItr != m_Verbs.end() )
            {
                result &= (*commandItr).second->Process( itr, arguments.end(), error );
            }
            else
            {
                error = std::string( "Unknown command: " ) + arg + "\n\n";

                for ( M_StringToVerbDumbPtr::const_iterator cItr = m_Verbs.begin(), cEnd = m_Verbs.end(); cItr != cEnd; ++cItr )
                {
                    error += (*cItr).second->GetShortHelp() + "\n";
                }
            }
        }
    }

    return result;
}

bool Processor::RegisterVerb( Verb* verb )
{
    m_Verbs[ verb->Token() ] = verb;
    verb->SetOwner( this );
    return true;
}

void Processor::UnregisterVerb( Verb* verb )
{
    UnregisterVerb( verb->Token() );
    verb->SetOwner( NULL );
}

void Processor::UnregisterVerb( const std::string& token )
{
    m_Verbs.erase( token );
}

const Verb* Processor::GetVerb( const std::string& token )
{
    Verb* command = NULL;
    M_StringToVerbDumbPtr::iterator itr = m_Verbs.find( token );
    if ( itr != m_Verbs.end() )
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
