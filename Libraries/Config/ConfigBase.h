#pragma once

#include "API.h"
#include <stdlib.h>
#include <string>

#include "Common/Types.h"

// Config API
namespace Config
{
  class CONFIG_API ConfigBase
  {
  public:

    ConfigBase(){};
    virtual ~ConfigBase(){};

    virtual bool Read() = 0;
    virtual bool Write() = 0;

    virtual bool GetValue( const std::string& propertyPath, V_string& value ) = 0;
    virtual bool GetValue( const std::string& propertyPath, std::string& value ) = 0;
    virtual bool GetValue( const std::string& propertyPath, bool& value ) = 0;
    virtual bool GetValue( const std::string& propertyPath, i32& value ) = 0;
    virtual bool GetValue( const std::string& propertyPath, u32& value ) = 0;
    virtual bool GetValue( const std::string& propertyPath, f32& value ) = 0;
    virtual bool GetValue( const std::string& propertyPath, f64& value ) = 0;
                                             
    virtual bool SetValue( const std::string& propertyPath, const V_string& value ) = 0;
    virtual bool SetValue( const std::string& propertyPath, const std::string& value ) = 0;
    virtual bool SetValue( const std::string& propertyPath, const char* value ) = 0;
    virtual bool SetValue( const std::string& propertyPath, const bool& value ) = 0;
    virtual bool SetValue( const std::string& propertyPath, const i32& value ) = 0;
    virtual bool SetValue( const std::string& propertyPath, const u32& value ) = 0;
    virtual bool SetValue( const std::string& propertyPath, const f32& value ) = 0;
    virtual bool SetValue( const std::string& propertyPath, const f64& value ) = 0;

    virtual bool DeleteValue( const std::string &propertyPath ) = 0;
  };
}
