#pragma once

#include <set>

#include "Foundation/API.h"
#include "Foundation/Memory/SmartPtr.h"
#include "Foundation/String/Utilities.h"
#include "Platform/String.h"

namespace Platform
{
    struct Stat;
}

namespace Nocturnal
{
    const static tchar s_InternalPathSeparator = '/';

    class FOUNDATION_API Path : public Nocturnal::RefCountBase< Path >
    {
    private:

        tstring m_Path;

        void Init( const tchar* path );

    public:
        static void Normalize( tstring& path );
        static void MakeNative( tstring& path );
        static void GuaranteeSlash( tstring& path );

        static bool Exists( const tstring& path );
        static bool IsAbsolute( const tstring& path );
        static bool IsUnder( const tstring& location, const tstring& path );

    public:
        Path( const tchar* path );
        Path( const tstring& path = TXT( "" ) );
        Path( const Path& path );

        Path& operator=( const Path& rhs );
        bool operator==( const Path& rhs ) const;

        bool operator<( const Path& rhs ) const;

        const tstring& Get() const;
        const tstring& Set( const tstring& path );

        void Split( tstring& directory, tstring& filename ) const;
        void Split( tstring& directory, tstring& filename, tstring& extension ) const;

        tstring Basename() const;
        tstring Filename() const;
        tstring Directory() const;

        tstring Extension() const;
        tstring FullExtension() const;
        void RemoveExtension();
        void RemoveFullExtension();

        tstring Native() const;
        tstring Absolute() const;
        tstring Normalized() const;

        u64 Hash() const;
        tstring Signature();

        bool Exists() const;
        bool Stat( Platform::Stat& stat ) const;
        bool IsAbsolute() const;
        bool IsUnder( const tstring& location );
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

        tstring FileCRC() const;
        bool VerifyFileCRC( const tstring& hash ) const;

        tstring FileMD5() const;
        bool VerifyFileMD5( const tstring& hash ) const;


    public:

        void ReplaceExtension( const tstring& newExtension );
        void ReplaceFullExtension( const tstring& newExtension );

    public:

        size_t length() const;
        bool empty() const;
        const tchar* c_str() const;
        operator const tchar*() const
        {
            return c_str();
        }
        operator const tstring&() const
        {
            return m_Path;
        }

        friend FOUNDATION_API std::ostream& operator<<( std::ostream& outStream, const Path& p );
        friend FOUNDATION_API std::istream& operator>>( std::istream& inStream, Path& p );
        friend FOUNDATION_API std::wostream& operator<<( std::wostream& outStream, const Path& p );
        friend FOUNDATION_API std::wistream& operator>>( std::wistream& inStream, Path& p );
    };

    inline std::ostream& operator<<( std::ostream& outStream, const Path& p )
    {
        std::string narrowPath;
        bool converted = Platform::ConvertString( p.Get(), narrowPath );
        NOC_ASSERT( converted );
        outStream << narrowPath.c_str();

        return outStream;
    }

    inline std::istream& operator>>( std::istream& inStream, Path& p )
    {
        std::string buf;
        std::streamsize size = inStream.rdbuf()->in_avail();
        buf.resize( (size_t) size );
        inStream.read( const_cast<char*>( buf.c_str() ), size );
        
        tstring temp;
        bool converted = Platform::ConvertString( buf, temp );
        NOC_ASSERT( converted );

        p.Set( temp );

        return inStream;
    }
    inline std::wostream& operator<<( std::wostream& outStream, const Path& p )
    {
        outStream << p.Get();

        return outStream;
    }

    inline std::wistream& operator>>( std::wistream& inStream, Path& p )
    {
        std::wstring buf;
        std::streamsize size = inStream.rdbuf()->in_avail();
        buf.resize( (size_t) size );
        inStream.read( const_cast<wchar_t*>( buf.c_str() ), size );

        tstring temp;
        bool converted = Platform::ConvertString( buf, temp );
        NOC_ASSERT( converted );

        p.Set( temp );

        return inStream;
    }
}
