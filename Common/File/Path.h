#pragma once

#include "../API.h"

#include "../Memory/SmartPtr.h"
#include "../String/Utilities.h"

namespace Nocturnal
{
  namespace Platforms
  {
    enum Platform
    {
      Internal,
      Windows,
      Unix,

      Count
    };
  }
  typedef Platforms::Platform Platform;

  // must line up with enum above
  const static char s_PlatformSeparators[] = {
    '/',   // Internal
    '\\',  // Windows
    '/'    // Unix
  };

  class Path;
  typedef Nocturnal::SmartPtr< Path > PathPtr;
  typedef std::vector< PathPtr > V_Path;

  class COMMON_API Path
  {
  private:

    std::string m_Path;
    std::string m_Native;
    std::string m_Absolute;
    std::string m_Normalized;
    std::string m_Signature;

    void Init( const char* path );

  public:
    static void Normalize( std::string& path );
    static void MakeNative( std::string& path );

    static bool IsUnder( const std::string& location, const std::string& path );

  public:
    Path( const char* path );
    Path( const std::string& path = "" );
    Path( const Path& path );

    Path& operator=( const Path& rhs );
    bool operator==( const Path& rhs ) const;

    const std::string& Get() const;
    const std::string& Set( const std::string& path );

    void Split( std::string& directory, std::string& filename ) const;
    void Split( std::string& directory, std::string& filename, std::string& extension ) const;

    std::string Filename() const;
    std::string Directory() const;
    std::string Extension() const;

    const std::string& Native();
    const std::string& Absolute();
    const std::string& Normalized();

    const std::string& Signature();

    bool IsUnder( const std::string& location );

  public:

    void ReplaceExtension( const std::string& newExtension );

  public:

    size_t length() const;
    bool empty() const;
    const char* c_str() const;
    operator const char*() const
    {
      return c_str();
    }
    operator const std::string&() const
    {
      return m_Path;
    }
  };
}
