#pragma once

#include "Config.h"

#include <stdlib.h>
#include <assert.h>
#include <string>
#include <sstream>

#include "Platform/Assert.h"

class TiXmlNode;
class TiXmlDocument;

namespace Config
{

  class CONFIG_API XMLConfig : public ConfigBase
  {

  public:
    XMLConfig( const std::string& configFilePath = "", const std::string& root = "" );
    ~XMLConfig();

    bool Read();
    bool Write();

    bool GetValue(const std::string& propertyPath, V_string& value);
    bool GetValue(const std::string& propertyPath, std::string& value);
    bool GetValue(const std::string& propertyPath, bool& value);
    bool GetValue(const std::string& propertyPath, i32& value);
    bool GetValue(const std::string& propertyPath, u32& value);
    bool GetValue(const std::string& propertyPath, f32& value);
    bool GetValue(const std::string& propertyPath, f64& value);

    bool SetValue(const std::string& propertyPath, const V_string& value);
    bool SetValue(const std::string& propertyPath, const std::string& value);
    bool SetValue(const std::string& propertyPath, const char* value);
    bool SetValue(const std::string& propertyPath, const bool& value);
    bool SetValue(const std::string& propertyPath, const i32& value);
    bool SetValue(const std::string& propertyPath, const u32& value);
    bool SetValue(const std::string& propertyPath, const f32& value);
    bool SetValue(const std::string& propertyPath, const f64& value);

    bool DeleteValue(const std::string& propertyPath);

    typedef TiXmlNode Node;

  private:
    Node* XMLConfig::GetNode( const std::string& propertyPath );

  public:

    std::string    m_ConfigFilePath; // file path to use
    std::string    m_RootNodeName;

  private:

    TiXmlDocument* m_Document;
    
    bool           m_Modified;              // modified flag
    bool           m_ConfigFileLoaded;      // a config file is currently loaded
  };
}