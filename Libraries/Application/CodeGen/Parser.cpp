#include "Parser.h"
#include "FileSystem/FileSystem.h"

#include <tinyxml.h>

using namespace CodeGen;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
Parser::Parser()
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
Parser::~Parser()
{
}

///////////////////////////////////////////////////////////////////////////////
// Parses the specified file and returns true if it was parsed successfully.
// 
bool Parser::Parse( const std::string& file )
{
  m_Root = NULL;
  m_LastError.clear();
  m_File = file;

  if ( m_File.empty() )
  {
    m_LastError = "No file specified.";
    return false;
  }

  if ( !FileSystem::Exists( m_File ) )
  {
    m_LastError = "File does not exist: " + m_File;
    return false;
  }

  TiXmlDocument doc;
  if ( !doc.LoadFile( m_File.c_str() ) )
  {
    m_LastError = "File is not a valid XML document: " + m_File;
    return false;
  }

  TiXmlElement* firstElement = doc.FirstChildElement();
  if ( firstElement && std::string( firstElement->Value() ) == "CodeGen" )
  {
    for ( TiXmlElement* current = firstElement->FirstChildElement(); current != NULL; current = current->NextSiblingElement() )
    {
      const std::string value( current->Value() );
      if ( value == "Namespace" )
      {
        m_Root = ParseNamespace( current );
      }
      else
      {
        m_LastError = "Unexpected tag '" + value + "' while parsing '" + m_File + "'.";
        return false;
      }
    }
  }
  else
  {
    m_LastError = "Document '" + m_File + "' did not start with the proper tag 'CodeGen'.";
    return false;
  }

  return m_Root.ReferencesObject();
}

///////////////////////////////////////////////////////////////////////////////
// Returns a description of the last error encountered while parsing.
// 
const std::string& Parser::GetLastError() const
{
  return m_LastError;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the root namespace that was parsed from a file.  Only valid if a
// successful call to Parse is made.
// 
NamespacePtr Parser::GetRootNamespace() const
{
  return m_Root;
}

///////////////////////////////////////////////////////////////////////////////
// Helper function to parse data from a namespace element in the file.
// 
NamespacePtr Parser::ParseNamespace( TiXmlElement* element )
{
  const char* name = element->Attribute( "name" );
  if ( strlen( name ) == 0 )
  {
    m_LastError = "There is a namespace with no name!";
    return NULL;
  }

  NamespacePtr parentNamespace = new Namespace( name );

  for ( TiXmlElement* current = element->FirstChildElement(); current != NULL; current = current->NextSiblingElement() )
  {
    const std::string value( current->Value() );
    if ( value == "Enum" )
    {
      EnumPtr enumPtr = ParseEnum( current );
      if ( !enumPtr.ReferencesObject() )
      {
        return NULL;
      }

      if ( !parentNamespace->AppendEnum( enumPtr, m_LastError ) )
      {
        return NULL;
      }
    }
    else if ( value == "Namespace" )
    {
      NamespacePtr childNamespace = ParseNamespace( current );
      if ( !childNamespace.ReferencesObject() )
      {
        return NULL;
      }
      parentNamespace->AddNamespace( childNamespace );
    }
    else
    {
      m_LastError = "Unrecognized tag '" + value + "' inside namespace '" + std::string( name ) + "'.";
      return NULL;
    }
  }

  return parentNamespace;
}

///////////////////////////////////////////////////////////////////////////////
// Helper function to parse an enumeration element from the file.
// 
EnumPtr Parser::ParseEnum( TiXmlElement* element )
{
  const char* name = element->Attribute( "name" );
  if ( strlen( name ) == 0 )
  {
    m_LastError = "There is an enumeration with no name!";
    return NULL;
  }

  EnumPtr enumPtr = new Enum( name );

  for ( TiXmlElement* current = element->FirstChildElement(); current != NULL; current = current->NextSiblingElement() )
  {
    const std::string tag( current->Value() );
    if ( tag != "Enumval" )
    {
      m_LastError = "Unrecognized XML tag '" + tag + "', found while parsing the enumeration named '" + std::string( name ) + "'.";
      return NULL;
    }

    const char* enumName = current->Attribute( "name" );
    if ( !enumName )
    {
      m_LastError = "One of the enumeration values in the '" + std::string( name ) + "' enumeration, did not have a 'name' attribute!";
      return NULL;
    }
    else if ( strlen( enumName ) == 0 )
    {
      m_LastError = "One of the enumeration values in the '" + std::string( name ) + "' enumeration, has an empty 'name' attribute!";
      return NULL;
    }

    i32 value = 0;
    bool useValue = false;
    const char* enumVal = current->Attribute( "value" );
    if ( enumVal && strlen( enumVal ) != 0 )
    {
      value = atoi( enumVal );
      useValue = true;
    }

    bool added = false;
    if ( useValue )
    {
      added = enumPtr->AddValue( enumName, value );
    }
    else
    {
      added = enumPtr->AddValue( enumName );
    }

    if ( !added )
    {
      m_LastError = "Unable to add value '";
      m_LastError += enumName;
      m_LastError += "' to enumeration '";
      m_LastError += name;
      m_LastError += "'.";
      return NULL;
    }
  }

  return enumPtr;
}
