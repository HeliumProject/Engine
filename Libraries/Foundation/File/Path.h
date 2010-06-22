#pragma once

#include "Foundation/API.h"
#include "Foundation/Memory/SmartPtr.h"
#include "Foundation/String/Utilities.h"

namespace Platform
{
    struct Stat;
}

namespace Nocturnal
{
    const static char s_InternalPathSeparator = '/';

    class FOUNDATION_API Path : public Nocturnal::RefCountBase< Path >
    {
    private:

        std::string m_Path;

        void Init( const char* path );

    public:
        static void Normalize( std::string& path );
        static void MakeNative( std::string& path );
        static void GuaranteeSlash( std::string& path );

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
        std::string FullExtension() const;
        void RemoveExtension();
        void RemoveFullExtension();

        std::string Native() const;
        std::string Absolute() const;
        std::string Normalized() const;

        u64 Hash() const;
        std::string Signature();

        bool Exists() const;
        bool Stat( Platform::Stat& stat ) const;
        bool IsAbsolute() const;
        bool IsUnder( const std::string& location );
        bool IsFile() const;
        bool IsDirectory() const;
        bool Writable() const;
        bool Readable() const;
        bool ChangedSince( u64 lastTime ) const;
        u64 ModifiedTime() const;
        u64 CreatedTime() const;
        u64 AccessTime() const;
        i64 Size() const;

        bool MakePath() const;
        bool Create() const;
        bool Copy( const Nocturnal::Path& target, bool overwrite = true ) const;
        bool Move( const Nocturnal::Path& target ) const;
        bool Delete() const;

        std::string FileCRC() const;
        bool VerifyFileCRC( const std::string& hash ) const;

        std::string FileMD5() const;
        bool VerifyFileMD5( const std::string& hash ) const;


    public:

        void ReplaceExtension( const std::string& newExtension );
        void ReplaceFullExtension( const std::string& newExtension );

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

        friend FOUNDATION_API std::ostream& operator<<( std::ostream& outStream, const Path& p );
        friend FOUNDATION_API std::istream& operator>>( std::istream& inStream, Path& p );
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
