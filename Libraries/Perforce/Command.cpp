#include "Command.h"
#include "Perforce.h"

#include "Console/Console.h"
#include <sstream>

using namespace Perforce;

void Command::Run()
{
   m_Provider->RunCommand( this );
}

std::string Command::AsString()
{
  std::stringstream str;
  str << m_Command;

  V_string::const_iterator itr = m_Arguments.begin();
  V_string::const_iterator end = m_Arguments.end();
  for ( ; itr != end; ++itr )
  {
    str << " " << (*itr);
  }

  return str.str();
}

void Command::HandleError( Error *error )
{
  StrBuf buf;

  if ( error->IsWarning() )
  {
    error->Fmt( &buf );
    Console::Warning( buf.Text() );
  }
  else if ( error->IsError() )
  {
    ++m_ErrorCount;
    error->Fmt( &buf );

    if ( m_ErrorCount == 1 )
    {
      m_ErrorString = buf.Text();
    }
    else
    {
      //
      // Beyond 10 errors just print ellipsis
      //

      if ( m_ErrorCount < 10 )
      {
        m_ErrorString += "\n";
        m_ErrorString += buf.Text();
      }
      else if ( m_ErrorCount == 10 )
      {
        m_ErrorString += "\n...";
      }
    }
  }
  else if ( error->IsFatal() )
  {
    error->Fmt( &buf );
    throw Nocturnal::Exception( buf.Text() );
  }
}

void Command::OutputStat( StrDict* dict )
{
  Console::Warning( "Unhandled perforce response for command '%s':\n", m_Command );
  Console::Indentation indent;

  StrRef var;
  StrRef value;

  for ( int i = 0; i < PERFORCE_MAX_DICT_ENTRIES; ++i )
  {
    if ( !dict->GetVar( i, var, value ) )
    {
      break;
    }
    Console::Warning( "%s: %s\n", var.Text(), value.Text() );
  }

  Console::Warning( "\n" );
}