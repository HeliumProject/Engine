#include "XMLConfig.h"

#include "tinyxml.h"

#include "Common/Exception.h"

namespace Config
{
  XMLConfig::XMLConfig( const std::string& configFilePath, const std::string& rootNodeName )
    : m_ConfigFilePath( configFilePath )
    , m_Document( NULL )
    , m_RootNodeName( rootNodeName )
    , m_Modified( false )
    , m_ConfigFileLoaded( false )
  {
  }

  XMLConfig::~XMLConfig()
  {
    if ( m_Document )
    {
      delete m_Document;
    }
  }

  //////////////////////////////////////////////////////////////
  // Write -- Writes out a modified config to disk

  bool XMLConfig::Write()
  {
    if ( !m_Modified )
    {
      return true;
    }

    m_Modified = false;
    return true;
  }

  //////////////////////////////////////////////////////////////
  // Read -- Reads the config file from disk

  bool XMLConfig::Read()
  {
    if ( m_Document )
    {
      delete m_Document;
    }

    m_Document = new TiXmlDocument();

    if ( m_Document->LoadFile( m_ConfigFilePath.c_str() ) )
    {
      m_Modified = false;
      m_ConfigFileLoaded = true;
      return true;
    }

    return false;
  }

  //////////////////////////////////////////////////////////////
  // GetValue -- Overloads for getting a value from the config

  bool XMLConfig::GetValue( const std::string& propertyPath, V_string& value )
  {
    Node* node = GetNode( propertyPath );
    
    if ( !node )
    {
      return false;
    }
    
    while( node )
    {
      value.push_back( node->ToElement()->GetText() );
      node = node->NextSibling();
    }

    return true;
  }

  bool XMLConfig::GetValue( const std::string& propertyPath, std::string& value)
  {
    Node* node = GetNode( propertyPath );
    if ( node )
    {
      value = node->ToElement()->GetText();
      return true;
    }

    return false;
  }

  bool XMLConfig::GetValue( const std::string& propertyPath, bool& value)
  {
    Node* node = GetNode( propertyPath );
    if ( node )
    {
      std::string text = node->ToElement()->GetText();

      if ( text == "0" || _stricmp( text.c_str(), "false" ) == 0 )
      {
        value = false;
      }
      else
      {
        value = true;
      }

      return true;
    }

    return false;
  }

  bool XMLConfig::GetValue( const std::string& propertyPath, i32& value)
  {
    Node* node = GetNode( propertyPath );
    if ( node )
    {
      value = atoi( node->ToElement()->GetText() );
      return true;
    }

    return false;
  }

  bool XMLConfig::GetValue( const std::string& propertyPath, u32& value)
  {
    Node* node = GetNode( propertyPath );
    if ( node )
    {
      value = (u32) atoi( node->ToElement()->GetText() );
      return true;
    }

    return false;
  }

  bool XMLConfig::GetValue( const std::string& propertyPath, f32& value)
  {
    Node* node = GetNode( propertyPath );
    if ( node )
    {
      value = (f32) atof( node->ToElement()->GetText() );
      return true;
    }

    return true;
  }

  bool XMLConfig::GetValue( const std::string& propertyPath, f64& value)
  {
    return false;
  }

  XMLConfig::Node* XMLConfig::GetNode( const std::string& propertyPath )
  {
    V_string path;

    std::stringstream stream( propertyPath );
    std::string key;
    while( std::getline( stream, key, '/' ) )
    {
        path.push_back( key );
    }
    

    TiXmlHandle handle( m_Document );

    if ( !m_RootNodeName.empty() )
    {
      handle = handle.FirstChildElement( m_RootNodeName.c_str() );
    }

    V_string::const_iterator itr = path.begin();
    V_string::const_iterator end = path.end();
    for( ; itr != end; ++itr )
    {
      handle = handle.FirstChildElement( (*itr).c_str() );
    }

    return handle.Node();
  }

  //////////////////////////////////////////////////////////////
  // SetValue -- Overloads for setting a value in the config

  bool XMLConfig::SetValue( const std::string& propertyPath, const V_string& value)
  {
    m_Modified = true;
    return true;
  }

  bool XMLConfig::SetValue( const std::string& propertyPath, const std::string& value)
  {
    return SetValue( propertyPath, value.c_str() );
  }

  bool XMLConfig::SetValue( const std::string& propertyPath, const char* value )
  {
    m_Modified = true;
    return true;
  }

  bool XMLConfig::SetValue( const std::string& propertyPath, const bool& value )
  {
    m_Modified = true;
    return true;
  }

  bool XMLConfig::SetValue( const std::string& propertyPath, const i32& value )
  {
    m_Modified = true;
    return true;
  }

  bool XMLConfig::SetValue( const std::string& propertyPath, const u32& value )
  {
    m_Modified = true;
    return true;
  }

  bool XMLConfig::SetValue( const std::string& propertyPath, const f32& value )
  {
    m_Modified = true;
    return true;
  }

  bool XMLConfig::SetValue( const std::string& propertyPath, const f64& value )
  {
    return false;
  }

  //////////////////////////////////////////////////////////////
  // DeleteValue -- Remove a value from the config

  bool XMLConfig::DeleteValue( const std::string& propertyPath )
  {
    m_Modified = true;
    return true;
  }
}
