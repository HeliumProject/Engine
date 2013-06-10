#include "EditorPch.h"
#include "P4Command.h"
#include "Perforce.h"

#include "Foundation/Log.h"
#include "Platform/Encoding.h"

using namespace Helium::Perforce;

void Command::Run()
{
    m_Provider->RunCommand( this );
}

std::string Command::AsString()
{
    std::stringstream str;
    str << m_Command;

    std::vector< std::string >::const_iterator itr = m_Arguments.begin();
    std::vector< std::string >::const_iterator end = m_Arguments.end();
    for ( ; itr != end; ++itr )
    {
        str << " " << (*itr);
    }

    return str.str();
}

void Command::HandleError( Error *error )
{
    StrBuf buf;
    std::string errString;

    if ( error->IsWarning() )
    {
        error->Fmt( &buf );

        bool converted = Helium::ConvertString( buf.Text(), errString );
        HELIUM_ASSERT( converted );

        Log::Warning( errString.c_str() );
    }
    else if ( error->IsError() )
    {
        ++m_ErrorCount;
        error->Fmt( &buf );

        if ( m_ErrorCount == 1 )
        {
            bool converted = Helium::ConvertString( buf.Text(), m_ErrorString );
            HELIUM_ASSERT( converted );
        }
        else
        {
            //
            // Beyond 10 errors just print ellipsis
            //

            if ( m_ErrorCount < 10 )
            {
                m_ErrorString += TXT( "\n" );

                bool converted = Helium::ConvertString( buf.Text(), errString );
                HELIUM_ASSERT( converted );

                m_ErrorString += errString;
            }
            else if ( m_ErrorCount == 10 )
            {
                m_ErrorString += TXT( "\n..." );
            }
        }
    }
    else if ( error->IsFatal() )
    {
        error->Fmt( &buf );
        bool converted = Helium::ConvertString( buf.Text(), errString );
        HELIUM_ASSERT( converted );
        throw Helium::Exception( errString.c_str() );
    }
}

void Command::OutputStat( StrDict* dict )
{
    Log::Warning( TXT( "Unhandled perforce response for command '%s':\n" ), m_Command );
    Log::Indentation indent;

    StrRef var;
    StrRef value;

    for ( int i = 0; i < PERFORCE_MAX_DICT_ENTRIES; ++i )
    {
        if ( !dict->GetVar( i, var, value ) )
        {
            break;
        }
        Log::Warning( TXT( "%s: %s\n" ), var.Text(), value.Text() );
    }

    Log::Warning( TXT( "\n" ) );
}