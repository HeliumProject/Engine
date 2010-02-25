#include "Windows.h"
#include "Mail.h"

#include <atlsmtpconnection.h>

#include "Common/Config.h"
#include "Common/Version.h"
#include "Common/Environment.h"

////////////////////////////////////////////////////////////////////////////////////////////////
//
//  SendMail()
//
//  written by: Geoff Evans
//
////////////////////////////////////////////////////////////////////////////////////////////////
inline void Tokenize( const std::string& str, std::set< std::string >& tokens, const std::string& delimiters )
{
  // Skip delimiters at beginning.
  std::string::size_type lastPos = str.find_first_not_of( delimiters, 0 );
  // Find first "non-delimiter".
  std::string::size_type pos     = str.find_first_of( delimiters, lastPos );

  while ( std::string::npos != pos || std::string::npos != lastPos )
  {
    // Add the token to the vector
    tokens.insert( str.substr( lastPos, pos - lastPos ) );
    // Skip delimiters.  Note the "not_of"
    lastPos = str.find_first_not_of( delimiters, pos );
    // Find next "non-delimiter"
    pos = str.find_first_of( delimiters, lastPos );
  }
}

bool Windows::SendMail(const std::string& subject, const std::string& text, const V_string& files, std::string destination, std::string source)
{
  if (getenv( NOCTURNAL_STUDIO_PREFIX "DEBUG_DONT_EMAIL" ) != NULL)
  {
    return true;
  }

  if ( destination.empty() )
  {
    if ( !Nocturnal::GetEnvVar( NOCTURNAL_STUDIO_PREFIX"CRASH_REPORT_EMAIL", destination ) )
    {
      return false;
    }
  }

  if ( source.empty() )
  {
    if ( !Nocturnal::GetEnvVar( NOCTURNAL_STUDIO_PREFIX"CRASH_REPORT_EMAIL", source ) )
    {
      return false;
    }
  }

  //
  // Init COM
  //

  CoInitialize(NULL);


  //
  // Message to send
  //

  CMimeMessage msg;
  msg.SetSenderName( source.c_str() );
  msg.SetSender( source.c_str() );

  if ( destination.find( ";" ) != std::string::npos )
  {
    S_string recipients;
    Tokenize( destination, recipients, ";" );
    S_string::iterator itr = recipients.begin();
    S_string::iterator end = recipients.end();
    for ( ; itr != end; ++itr )
    {
      msg.AddRecipient( (*itr).c_str() );
    }
  }
  else
  {
    msg.AddRecipient( destination.c_str() );
  }

  if (text.empty())
  {
    msg.SetSubject( CA2CT((subject + " (n/t)").c_str()) );
  }
  else
  {
    msg.SetSubject( CA2CT(subject.c_str()) );
  }

  if (!text.empty())
  {
    std::string fixed;

    std::string::const_iterator itr = text.begin();
    std::string::const_iterator end = text.end();
    for ( ; itr != end; ++itr )
    {
      if (*itr == '\n')
      {
        fixed.append( "\r\n" );
      }
      else
      {
        fixed.append( itr, itr+1 );
      }
    }

    msg.AddText( CA2CT(fixed.c_str()) );
  }

  V_string::const_iterator itr = files.begin();
  V_string::const_iterator end = files.end();
  for ( ; itr != end; ++itr )
  {
    msg.AttachFile( CA2CT(itr->c_str()) );
  }


  //
  // Send
  //

  std::string server;
  if ( !Nocturnal::GetEnvVar( NOCTURNAL_STUDIO_PREFIX"MAIL_SERVER", server ) )
  {
    return false;
  }

  CSMTPConnection connection;

  if( connection.Connect( server.c_str() ) )
  {
    BOOL sent = connection.SendMessage(msg,
                                       destination.c_str(),
                                       source.c_str());

    connection.Disconnect();

    return sent == TRUE;
  }

  return false;
}