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
    typedef std::vector< Path > V_Path;
    typedef std::set< Path > S_Path;

    class COMMON_API Path : public Nocturnal::RefCountBase< Path >
    {
    private:

        std::string m_Path;

        void Init( const char* path );

    public:
        static void Normalize( std::string& path );
        static void MakeNative( std::string& path );

        static bool Exists( const std::string& path );
        static bool IsAbsolute( const std::string& path );
        static bool IsUnder( const std::string& location, const std::string& path );

    public:
        Path( const char* path );
        Path( const std::string& path = "" );
        Path( const Path& path );

        Path& operator=( const Path& rhs );
        bool operator==( const Path& rhs ) const;

        bool operator<( const Path& rhs ) const;

        const std::string& Get() const;
        const std::string& Set( const std::string& path );

        void Split( std::string& directory, std::string& filename ) const;
        void Split( std::string& directory, std::string& filename, std::string& extension ) const;

        std::string Basename() const;
        std::string Filename() const;
        std::string Directory() const;
        std::string Extension() const;

        std::string Native() const;
        std::string Absolute() const;
        std::string Normalized() const;

        u64 Hash() const;
        std::string Signature();

        bool Exists() const;
        bool IsAbsolute() const;
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

        friend COMMON_API std::ostream& operator<<( std::ostream& outStream, const Path& p );
  	    friend COMMON_API std::istream& operator>>( std::istream& inStream, Path& p );
    };

    inline std::ostream& operator<<( std::ostream& outStream, const Path& p )
    {
        outStream << p.c_str();

        return outStream;
    }

    inline std::istream& operator>>( std::istream& inStream, Path& p )
    {
        std::string buf;
        std::streamsize size = inStream.rdbuf()->in_avail();
        buf.resize( (size_t) size );
        inStream.read( const_cast<char*>( buf.c_str() ), size );
        p.Set( buf );

        return inStream;
    }
}
