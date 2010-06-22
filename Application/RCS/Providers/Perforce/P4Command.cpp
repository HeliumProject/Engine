#include "P4Command.h"
#include "Perforce.h"

#include "Foundation/Log.h"
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

  if ( error->IsWarning() )
  {
    error->Fmt( &buf );
    Log::Warning( buf.Text() );
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
  Log::Warning( "Unhandled perforce response for command '%s':\n", m_Command );
  Log::Indentation indent;

  StrRef var;
  StrRef value;

  for ( int i = 0; i < PERFORCE_MAX_DICT_ENTRIES; ++i )
  {
    if ( !dict->GetVar( i, var, value ) )
    {
      break;
    }
    Log::Warning( "%s: %s\n", var.Text(), value.Text() );
  }

  Log::Warning( "\n" );
}